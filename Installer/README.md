# GTA6 Online Installer

Windows online bootstrapper source.

## Current implementation
- WinForms user interface and per-user install path (`%LOCALAPPDATA%\Programs\GTA6`).
- Reads the release manifest over HTTPS.
- Rejects unsafe relative paths, duplicate paths, non-HTTPS URLs, invalid sizes and malformed SHA-256 values.
- Downloads to temporary files, verifies size and SHA-256, then replaces destination files.
- Can launch the game executable after installation.

The manifest currently has no payload files. The installer therefore reports that no game release is available; it does not pretend the game is ready.

## Build
Requires .NET 8 SDK on Windows:
```powershell
dotnet publish .\Installer\GTA6Installer.csproj -c Release -r win-x64 --self-contained false -o .\artifacts\GTA6Installer
```
This is a framework-dependent single-file app and requires .NET 8 Desktop Runtime. The installer executable is intended to be small; the approximate 50 MB goal is not yet measured. CI will report the actual size.

## Distribution
The installer reads `distribution/manifest.json` from GitHub. Each game file will be hosted outside Git source history (GitHub Release assets or a CDN) and listed with HTTPS URL, exact byte size and SHA-256.

## Required before public release
- Implement detached digital signature for the manifest and embed the public verification key. Current hashes verify payload consistency only; an unsigned manifest is not protected against a compromised manifest host.
- Add Authenticode signing for installer and game executable.
- Add free-disk-space checks, resumable downloads and rollback-safe update transactions.
- Select production CDN and retention policy.
- Keep private signing keys and CDN credentials out of Git.
