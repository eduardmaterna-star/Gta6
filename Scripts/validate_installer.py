#!/usr/bin/env python3
"""Static checks for the GTA6 installer prototype; not a .NET compilation test."""
from pathlib import Path
import json
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / "Launcher/GTA6.Installer/GTA6.Installer.csproj"
MANIFEST = ROOT / "Launcher/RELEASE_MANIFEST.example.json"
SOURCE = ROOT / "Launcher/GTA6.Installer"


def check(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    project = PROJECT.read_text(encoding="utf-8")
    check("<TargetFramework>net8.0-windows</TargetFramework>" in project, "Expected .NET 8 Windows target.")
    check("<UseWindowsForms>true</UseWindowsForms>" in project, "Expected WinForms UI.")
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    check(manifest.get("product") == "GTA6", "Example manifest product must be GTA6.")
    check(re.fullmatch(r"[0-9a-fA-F]{64}", manifest.get("sha256", "")) is not None, "Example SHA-256 must be 64 hex chars.")
    for name in ("Program.cs", "MainForm.cs", "PackageInstaller.cs", "ReleaseManifest.cs"):
        check((SOURCE / name).is_file(), f"Missing installer source: {name}")
    installer = (SOURCE / "PackageInstaller.cs").read_text(encoding="utf-8")
    check("SHA256.HashDataAsync" in installer, "Package hash verification missing.")
    check("ExtractZipSafely" in installer, "Safe ZIP extraction missing.")
    check("Uri.UriSchemeHttps" in installer, "HTTPS restriction missing.")
    print("Installer static validation passed.")
    print("Note: .NET compilation, Windows UI execution and package installation were not run.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError, json.JSONDecodeError) as error:
        print(f"Validation failed: {error}", file=sys.stderr)
        raise SystemExit(1)
