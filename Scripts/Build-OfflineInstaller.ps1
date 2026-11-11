[CmdletBinding()]
param([ValidatePattern('^\d+\.\d+\.\d+$')][string]$Version = "0.1.0")
$ErrorActionPreference = "Stop"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$gameDir = Join-Path $root "Build\Windows"
$offlineDir = Join-Path $root "Build\Offline"
$outputDir = Join-Path $root "Build\Installer"
$project = Join-Path $root "Launcher\GTA6.Installer\GTA6.Installer.csproj"
$exeName = "OpenWorldNextGen.exe"
$footerMagic = [Text.Encoding]::ASCII.GetBytes("GTA6PAY1")

if (-not (Test-Path -LiteralPath $gameDir -PathType Container)) { throw "Missing Unreal packaged build: $gameDir" }
if (-not (Test-Path -LiteralPath (Join-Path $gameDir $exeName) -PathType Leaf)) { throw "Missing game executable: $exeName" }
if (-not (Get-Command dotnet -ErrorAction SilentlyContinue)) { throw ".NET 8 SDK is required." }
$sevenZip = Get-Command 7z.exe -ErrorAction SilentlyContinue
if (-not $sevenZip) { throw "7-Zip command-line tool 7z.exe is required to create large ZIP64 archives." }

New-Item -ItemType Directory -Force -Path $offlineDir, $outputDir | Out-Null
$zipPath = Join-Path $offlineDir "GTA6-Windows-$Version.zip"
$manifestPath = Join-Path $offlineDir "offline-manifest.json"
if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }

Push-Location $gameDir
try {
    & $sevenZip.Source a -tzip -mm=Deflate -mx=5 -mt=on $zipPath ".\*" -r
    if ($LASTEXITCODE -ne 0) { throw "7-Zip archive creation failed: $LASTEXITCODE" }
}
finally { Pop-Location }

$hash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash.ToLowerInvariant()
$manifest = [ordered]@{
    product = "GTA6"
    version = $Version
    packageFile = [IO.Path]::GetFileName($zipPath)
    packageUrl = $null
    sha256 = $hash
    executable = $exeName
}
$manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $manifestPath -Encoding utf8

# Publish a small self-contained launcher first. The multi-GB ZIP is appended afterwards,
# outside the .NET assembly/resource table, avoiding embedded-resource size constraints.
$publishArgs = @("publish", $project, "-c", "Release", "-r", "win-x64", "--self-contained", "true",
    "-p:PublishSingleFile=true", "-p:IncludeNativeLibrariesForSelfExtract=true", "-p:Version=$Version")
& dotnet @publishArgs
if ($LASTEXITCODE -ne 0) { throw "dotnet publish failed with exit code $LASTEXITCODE" }

$published = Join-Path $root "Launcher\GTA6.Installer\bin\Release\net8.0-windows\win-x64\publish\GTA6Installer.exe"
if (-not (Test-Path -LiteralPath $published -PathType Leaf)) { throw "Published installer executable not found: $published" }
$finalPath = Join-Path $outputDir "GTA6-Setup-$Version-x64.exe"
Copy-Item -LiteralPath $published -Destination $finalPath -Force

$manifestBytes = [IO.File]::ReadAllBytes($manifestPath)
$payloadLength = (Get-Item -LiteralPath $zipPath).Length
if ($payloadLength -le 0) { throw "Generated game payload is empty." }
if ($manifestBytes.Length -le 0 -or $manifestBytes.Length -gt 1MB) { throw "Offline manifest size is invalid." }

# Layout: [launcher PE][ZIP64 payload][UTF-8 manifest][footer].
# Footer = 8-byte magic + Int64 payload length + Int32 manifest length (little-endian).
$stream = [IO.File]::Open($finalPath, [IO.FileMode]::Append, [IO.FileAccess]::Write, [IO.FileShare]::None)
try {
    $zipStream = [IO.File]::OpenRead($zipPath)
    try { $zipStream.CopyTo($stream, 4MB) }
    finally { $zipStream.Dispose() }
    $stream.Write($manifestBytes, 0, $manifestBytes.Length)
    $footer = New-Object byte[] 20
    [Array]::Copy($footerMagic, 0, $footer, 0, 8)
    [Array]::Copy([BitConverter]::GetBytes([long]$payloadLength), 0, $footer, 8, 8)
    [Array]::Copy([BitConverter]::GetBytes([int]$manifestBytes.Length), 0, $footer, 16, 4)
    $stream.Write($footer, 0, $footer.Length)
    $stream.Flush()
}
finally { $stream.Dispose() }

$installerHash = Get-FileHash -LiteralPath $finalPath -Algorithm SHA256
[pscustomobject]@{
    Installer = $finalPath
    InstallerSizeGiB = [math]::Round((Get-Item $finalPath).Length / 1GB, 3)
    InstallerSHA256 = $installerHash.Hash
    EmbeddedGamePackageSHA256 = $hash
    PayloadFormat = "Appended ZIP64 + manifest + GTA6PAY1 footer"
} | Format-List
