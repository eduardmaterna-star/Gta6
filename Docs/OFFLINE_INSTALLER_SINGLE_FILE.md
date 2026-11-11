# GTA6 — single-file offline installer

The selected distribution is one Windows x64 EXE containing the installer and the complete packaged game. Installation must work without internet access or separate ZIP/manifest files. The EXE will be much larger than 50 MB: it includes a self-contained .NET runtime and the compressed Unreal game payload.

## Build pipeline
1. Package the Unreal project for Windows x64 using UE 5.8.
2. Stage the complete output in Build/Windows. Expected entry point: OpenWorldNextGen.exe in the package root.
3. Create a ZIP archive and generate a manifest with version, relative executable path and SHA-256.
4. Publish the WinForms installer as a self-contained single-file .NET 8 Windows x64 EXE, embedding the ZIP and manifest as resources.
5. Test installation and launch on a clean Windows machine with networking disabled.

## Layout
- Build/Windows: packaged Unreal build (not committed)
- Build/Offline: generated ZIP and manifest (not committed)
- Build/Installer: final EXE (not committed)
- Launcher/GTA6.Installer: installer source
- Scripts/Build-OfflineInstaller.ps1: packaging script

Generated binary payloads are not committed to ordinary Git history. Release EXEs should be distributed as GitHub Release assets or through binary hosting.

## Install behavior
- Embedded manifest and game package only; no network requests.
- SHA-256 verification before extraction.
- Staging directory, executable validation and rollback-capable directory replacement.
- Per-user install under LocalAppData; no elevation.
- User saves/configuration must remain outside the replaceable game directory.

## Current limitation
No packaged Unreal Windows build exists yet. Therefore the final installer cannot yet be produced or measured. The source work does not claim an installer EXE already exists.
