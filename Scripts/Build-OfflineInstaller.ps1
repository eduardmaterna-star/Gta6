[CmdletBinding()]
param([ValidatePattern('^\d+\.\d+\.\d+$')][string]$Version = "0.1.0")
$ErrorActionPreference = "Stop"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$gameDir = Join-Path $root "Build\Windows"
$offlineDir = Join-Path $root "Build\Offline"
$outputDir = Join-Path $root "Build\Installer"
$project = Join-Path $root "Launcher\GTA6.Installer\GTA6.Installer.csproj"
$exeName = "OpenWorldNextGen.exe"

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
$manifest = [ordered]@{ product = "GTA6"; version = $Version; packageFile = [IO.Path]::GetFileName($zipPath); packageUrl = $null; sha256 = $hash; executable = $exeName }
$manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $manifestPath -Encoding utf8

$publishArgs = @("publish", $project, "-c", "Release", "-r", "win-x64", "--self-contained", "true", "-p:PublishSingleFile=true", "-p:IncludeNativeLibrariesForSelfExtract=true", "-p:GamePackagePath=$zipPath", "-p:OfflineManifestPath=$manifestPath", "-p:Version=$Version")
& dotnet @publishArgs
if ($LASTEXITCODE -ne 0) { throw "dotnet publish failed with exit code $LASTEXITCODE" }

$published = Join-Path $root "Launcher\GTA6.Installer\bin\Release\net8.0-windows\win-x64\publish\GTA6Installer.exe"
if (-not (Test-Path -LiteralPath $published -PathType Leaf)) { throw "Published installer executable not found: $published" }
$finalPath = Join-Path $outputDir "GTA6-Setup-$Version-x64.exe"
Copy-Item -LiteralPath $published -Destination $finalPath -Force
$installerHash = Get-FileHash -LiteralPath $finalPath -Algorithm SHA256
[pscustomobject]@{ Installer = $finalPath; InstallerSizeGiB = [math]::Round((Get-Item $finalPath).Length / 1GB, 3); InstallerSHA256 = $installerHash.Hash; EmbeddedGamePackageSHA256 = $hash } | Format-List
