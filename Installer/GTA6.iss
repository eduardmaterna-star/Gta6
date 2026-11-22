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
; Keep the existing AppId stable so future updates use the same installation identity.
AppId={{B8E6A52D-73CF-4D12-AF66-5C5E0E31C6A2}
AppName=NEXXT GEN
AppVersion={#AppVersion}
AppPublisher=Nexxt Gen Project
DefaultDirName={localappdata}\Programs\NexxtGen
DefaultGroupName=NEXXT GEN
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
UninstallDisplayIcon={app}\OpenWorldNextGen.exe
OutputDir="{#OutputDir}"
OutputBaseFilename=NexxtGen-Setup-{#AppVersion}-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
Uninstallable=yes
CloseApplications=yes
RestartApplications=no
VersionInfoVersion={#AppVersion}.0
VersionInfoProductVersion={#AppVersion}.0
VersionInfoDescription=Nexxt Gen Offline Installer
VersionInfoProductName=NEXXT GEN

[Languages]
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"

[Tasks]
Name: "desktopicon"; Description: "Vytvořit ikonu na ploše"; GroupDescription: "Další možnosti:"; Flags: unchecked

[Files]
Source: "{#GameDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\NEXXT GEN"; Filename: "{app}\OpenWorldNextGen.exe"
Name: "{autodesktop}\NEXXT GEN"; Filename: "{app}\OpenWorldNextGen.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenWorldNextGen.exe"; Description: "Spustit NEXXT GEN"; Flags: postinstall nowait skipifsilent unchecked
