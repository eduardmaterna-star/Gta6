# GTA6 Installer — architecture prototype

Windows desktop bootstrapper for a combined online/offline distribution model.

## User flow
- **Offline:** select a folder containing `manifest.json` and the referenced ZIP package.
- **Online:** place `installer.settings.json` next to the installer; it must contain an HTTPS `manifestUrl`. The manifest points to an HTTPS package URL.
- The installer validates product/version metadata, verifies the complete package SHA-256, extracts into a staging directory, checks for the expected game executable, then swaps the staged install into place.
- Existing installation is moved to a temporary backup before replacement and restored if the final directory move fails.

## Security properties in this prototype
- HTTPS-only remote manifest/package URLs.
- SHA-256 verification before extraction.
- ZIP path traversal protection.
- Extraction into a staging directory before replacing the existing installation.
- No elevation request; installs to a user-writable location by default in a production UI.
- No arbitrary commands are read from the manifest.

## Build
Requires Windows and the .NET 8 Desktop Runtime SDK:
`dotnet publish GTA6.Installer.csproj -c Release -r win-x64 --self-contained false`

This framework-dependent build is intentionally small but requires the matching .NET 8 Desktop Runtime on the target PC. A self-contained build bundles the runtime and will be much larger. The 50 MB target is a packaging goal and must be measured on the produced artifact; it is not yet verified.

## Release hosting
The sample URL uses the reserved `.invalid` domain. Before online distribution, configure a real HTTPS release endpoint and publish the manifest plus ZIP. Never ship a placeholder hash or placeholder endpoint as a real release.

## Current limitations
- This is an initial installer prototype, not a finished signed production installer.
- No game payload or Unreal Engine runtime is included yet.
- No delta patching, resumable downloads, code signing, automatic rollback after a launched-game health check, or launcher self-update yet.
- The current application UI supports install and optional launch; a separate in-game launcher/update UI is planned.
