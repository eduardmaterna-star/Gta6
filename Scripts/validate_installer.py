#!/usr/bin/env python3
"""Static checks for the selected single-file Inno Setup installer pipeline.

This validates source configuration only. It does not compile the installer or
prove that a packaged Unreal game can be installed and launched offline.
"""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
INSTALLER_SCRIPT = ROOT / "Installer/GTA6.iss"
BUILD_SCRIPT = ROOT / "Scripts/Build-OfflineInstaller.ps1"


def check(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    installer = INSTALLER_SCRIPT.read_text(encoding="utf-8")
    build = BUILD_SCRIPT.read_text(encoding="utf-8")

    required_installer_settings = (
        ('AppName=GTA6', "Installer product name is missing."),
        ('PrivilegesRequired=lowest', "Installer must not require administrator privileges."),
        ('ArchitecturesAllowed=x64', "Installer must be restricted to Windows x64."),
        ('OutputDir="{#OutputDir}"', "Installer output directory must use the supplied macro and preserve spaces."),
        ('Compression=lzma2', "LZMA2 compression must be enabled."),
        ('SolidCompression=yes', "Solid compression must be enabled."),
        ('Source: "{#GameDir}\\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs',
         "Installer must recursively package the complete game directory."),
        ('Name: "czech"; MessagesFile: "compiler:Languages\\Czech.isl"',
         "Czech installer language must be configured."),
    )
    for expected, message in required_installer_settings:
        check(expected in installer, message)

    required_build_checks = (
        ('Build\\Windows', "Build script must use the packaged Windows game directory."),
        ('OpenWorldNextGen.exe', "Build script must require the actual game executable."),
        ('ISCC.exe', "Build script must invoke the Inno Setup compiler."),
        ('$LASTEXITCODE -ne 0', "Build script must fail when Inno Setup compilation fails."),
        ('Get-FileHash', "Build script must calculate the final installer SHA-256."),
        ('4GB', "Build script must reject installer files at the single-EXE size boundary."),
    )
    for expected, message in required_build_checks:
        check(expected in build, message)

    check("Launcher/GTA6.Installer" not in installer + build,
          "Selected offline installer must not depend on the retired .NET bootstrapper.")
    print("Inno Setup offline installer source validation passed.")
    print("Note: this is static validation; installer compilation and real offline installation were not run.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError) as error:
        print(f"Validation failed: {error}", file=sys.stderr)
        raise SystemExit(1)
