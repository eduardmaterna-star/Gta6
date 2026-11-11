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


## Installer safety checks

The installer source also performs the following checks before committing an installation:

- Verifies the package SHA-256 before extraction.
- Calculates the ZIP's declared uncompressed size and checks available space on the target drive, leaving a 256 MiB safety margin.
- Rejects absolute paths, traversal segments, drive prefixes, alternate-separator traversal, invalid path characters, duplicate paths, symbolic links and reparse-point entries.
- Creates files exclusively and checks that each extracted file matches its declared ZIP length.
- Extracts into a fresh staging directory and restores the previous installation if the final directory swap fails.
- Keeps the previous backup if cleanup cannot safely complete.

These are source-level safeguards, not a substitute for automated adversarial ZIP tests or a Windows installation test. The current resource-embedding pipeline is still provisional for multi-gigabyte packages; see [issue #6](https://github.com/eduardmaterna-star/Gta6/issues/6) for the required payload-format verification and test plan.
