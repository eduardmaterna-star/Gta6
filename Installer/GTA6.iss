#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif
#ifndef GameDir
  #error GameDir must point to the packaged Unreal Windows build directory.
#endif
#ifndef OutputDir
  #error OutputDir must point to the generated installer directory.
#endif

[Setup]
AppId={{B8E6A52D-73CF-4D12-AF66-5C5E0E31C6A2}
AppName=GTA6
AppVersion={#AppVersion}
AppPublisher=GTA6 Project
DefaultDirName={localappdata}\Programs\GTA6
DefaultGroupName=GTA6
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
UninstallDisplayIcon={app}\OpenWorldNextGen.exe
OutputDir={#OutputDir}
OutputBaseFilename=GTA6-Setup-{#AppVersion}-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
Uninstallable=yes
CloseApplications=yes
RestartApplications=no
VersionInfoVersion={#AppVersion}.0
VersionInfoProductVersion={#AppVersion}.0
VersionInfoDescription=GTA6 Offline Installer
VersionInfoProductName=GTA6

[Languages]
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"

[Tasks]
Name: "desktopicon"; Description: "Vytvořit ikonu na ploše"; GroupDescription: "Další možnosti:"; Flags: unchecked

[Files]
Source: "{#GameDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\GTA6"; Filename: "{app}\OpenWorldNextGen.exe"
Name: "{autodesktop}\GTA6"; Filename: "{app}\OpenWorldNextGen.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenWorldNextGen.exe"; Description: "Spustit GTA6"; Flags: postinstall nowait skipifsilent unchecked
