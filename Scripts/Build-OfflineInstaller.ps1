[CmdletBinding()]
param(
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string]$Version = "0.1.0"
)

$ErrorActionPreference = "Stop"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$gameDir = Join-Path $root "Build\Windows"
$outputDir = Join-Path $root "Build\Installer"
$scriptPath = Join-Path $root "Installer\GTA6.iss"
$exeName = "OpenWorldNextGen.exe"

if (-not (Test-Path -LiteralPath $gameDir -PathType Container)) {
    throw "Chybí zabalená Unreal hra: $gameDir"
}
if (-not (Test-Path -LiteralPath (Join-Path $gameDir $exeName) -PathType Leaf)) {
    throw "V herním balíku chybí $exeName."
}
if (-not (Test-Path -LiteralPath $scriptPath -PathType Leaf)) {
    throw "Chybí Inno Setup script: $scriptPath"
}

$isccCandidates = @(
    (Join-Path $env:ProgramFiles "Inno Setup 6\ISCC.exe"),
    (Join-Path ${env:ProgramFiles(x86)} "Inno Setup 6\ISCC.exe"),
    (Join-Path $env:ProgramFiles "Inno Setup 7\ISCC.exe")
) | Where-Object { $_ -and (Test-Path -LiteralPath $_ -PathType Leaf) }

$iscc = $isccCandidates | Select-Object -First 1
if (-not $iscc) {
    throw "Inno Setup 6.5.2+ compiler (ISCC.exe) is required."
}

New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
$gameDirForCompiler = $gameDir.Replace('"', '""')
& $iscc "/DAppVersion=$Version" "/DGameDir=$gameDirForCompiler" $scriptPath
if ($LASTEXITCODE -ne 0) {
    throw "Inno Setup compilation failed with exit code $LASTEXITCODE."
}

$installerPath = Join-Path $outputDir "GTA6-Setup-$Version-x64.exe"
if (-not (Test-Path -LiteralPath $installerPath -PathType Leaf)) {
    throw "Výsledný instalátor nebyl vytvořen: $installerPath"
}

$installer = Get-Item -LiteralPath $installerPath
# Windows PE executables cannot exceed the 4 GiB file-size boundary.
if ($installer.Length -ge 4GB) {
    Remove-Item -LiteralPath $installerPath -Force
    throw "Komprimovaný instalátor dosáhl limitu jednoho Windows EXE (téměř 4 GiB). Pro tuto velikost už nelze zachovat jeden EXE soubor."
}

$hash = Get-FileHash -LiteralPath $installerPath -Algorithm SHA256
[pscustomobject]@{
    Installer = $installerPath
    InstallerSizeGiB = [math]::Round($installer.Length / 1GB, 3)
    InstallerSHA256 = $hash.Hash
    Distribution = "Single self-contained Inno Setup EXE; no network or sidecar files"
} | Format-List
