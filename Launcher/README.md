# GTA6 Installer — single-file offline distribution

The selected release format is one Windows x64 executable containing both the installer application and the complete compressed Unreal game payload. Installation must work with networking disabled.

## Build input
- A complete Unreal Engine 5.8 Windows x64 packaged build in Build/Windows.
- The packaged game executable must be OpenWorldNextGen.exe at the payload root.
- .NET 8 SDK and 7-Zip command-line executable (7z.exe) on the Windows build machine.

## Build
Run from the repository root:

```powershell
.\Scripts\Build-OfflineInstaller.ps1 -Version 0.1.0
```

The script archives the game, creates a manifest with SHA-256, embeds both files into the WinForms installer project and publishes a self-contained single-file win-x64 EXE. The final file is written to Build/Installer.

## Install behavior
- Reads manifest and package from embedded resources only.
- No downloader, online manifest, external ZIP or internet connection.
- Verifies package SHA-256 before extraction.
- Extracts to a staging directory, validates the game executable and swaps the staged directory into place.
- Uses a per-user LocalAppData destination by default; no elevation.
- Does not delete user saves/configuration as part of replacing the game folder.

## Size
The installer includes a self-contained .NET runtime plus the complete compressed game. It will be much larger than 50 MB and will grow with the game. The final size is only known after a real Unreal packaged build and must be measured.

## Limitations before release
- The Unreal Windows payload is not present yet.
- A clean-machine offline installation and launch test is still required.
- Code signing and a release SHA-256 publication process are still required.
- Large generated binaries are not stored in ordinary Git history; publish the final EXE as a GitHub Release asset.
