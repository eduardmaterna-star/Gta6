# GTA6 distribution architecture

## Goals
- A small Windows installer application, targeting approximately 50 MB.
- One distribution path for both online and offline installation.
- A separate game payload, so game content does not inflate the bootstrapper.
- Integrity checks and safe installation replacement.

## Components

```
GTA6 Installer (small Windows app)
  ├─ Online source: HTTPS release manifest → HTTPS ZIP package
  └─ Offline source: local folder → manifest.json + ZIP package
                    ↓
              Validate manifest
                    ↓
              Verify package SHA-256
                    ↓
              Extract to staging directory
                    ↓
              Check expected game executable
                    ↓
              Atomic directory swap / rollback
                    ↓
              Installed GTA6 game
```

## Release layout
A release folder contains:
- `manifest.json` — product, semantic version, package filename, HTTPS package URL, SHA-256 and relative executable path.
- `GTA6-Windows-X.Y.Z.zip` — packaged Windows game payload.
- `installer.settings.json` — installed beside the bootstrapper; contains the HTTPS manifest URL.

Offline media contains the same manifest and package ZIP. The offline and online paths therefore use the same validation/install logic.

## Size policy
The ~50 MB target applies to the installer application only, not the game. A framework-dependent .NET 8 WinForms build should be smaller than a self-contained distribution but requires the .NET 8 Desktop Runtime on Windows. The actual output size must be measured after publishing. If requiring zero runtime prerequisites is more important, a self-contained installer will be larger.

## Future release pipeline
1. Build and package the Windows game from Unreal Engine on a Windows build agent with the licensed engine installed.
2. Generate SHA-256 for the package.
3. Generate a versioned release manifest.
4. Publish the package and manifest to HTTPS object storage/CDN.
5. Publish the small installer separately.
6. Test online and offline install, update, interrupted download, invalid hash, malformed ZIP and rollback.

## Security requirements before public release
- Sign installer and game executables with a trusted code-signing certificate.
- Keep release storage write access restricted.
- Use immutable versioned package URLs.
- Verify SHA-256 before extraction.
- Add per-file hashes or signed manifests before delta updates.
- Do not run installer elevated unless a concrete system-wide operation requires it.
- Never execute scripts or commands supplied by a remote manifest.
