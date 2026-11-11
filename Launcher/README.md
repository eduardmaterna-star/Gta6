# GTA6 offline installer

The shipping installer is built with Inno Setup, not the legacy .NET launcher prototype.

## Requirements
- Windows build machine.
- Inno Setup 6.5.2 or newer (ISCC.exe).
- Complete Unreal Engine 5.8 Windows x64 packaged build in Build/Windows.
- OpenWorldNextGen.exe at the root of the packaged folder.

## Build

Run from the repository root:

    .\\Scripts\\Build-OfflineInstaller.ps1 -Version 0.1.0

The resulting single offline installer is written to Build/Installer/GTA6-Setup-0.1.0-x64.exe. The complete contents of Build/Windows are embedded by Inno Setup; no external archive, sidecar file, downloader, or internet connection is required at install time.

## Size limit

Inno Setup 6.5.2+ supports a single Setup EXE up to nearly 4 GiB. The build script rejects an output at or above 4 GiB. If the compressed game exceeds this Windows executable limit, a single-file installer cannot be produced in this format.

## Validation

GitHub Actions compiles the installer script with a placeholder executable. This checks installer syntax and output creation only. A real release still requires a packaged Unreal build, clean Windows offline installation test, game launch test, and SHA-256 publication.
