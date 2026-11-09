#ifndef PluginVst3Path
  #error PluginVst3Path must point to the built Galaxy Tape.vst3 bundle.
#endif

#define AppName "Galaxy Tape"
#define AppVersion "2.0.0"
#define AppPublisher "Loops"
#define CompanyURL "https://loops.co.ke"
#define AppId "{{B63B6D87-59A7-47EA-A51E-8BB29F109C3C}"

[Setup]
AppId={#AppId}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={commoncf}\VST3\Galaxy Tape.vst3
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
Uninstallable=yes
OutputDir=dist
OutputBaseFilename=Galaxy-Tape-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "{#PluginVst3Path}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Run]
Filename: "{#CompanyURL}"; Description: "Visit Loops, your destination for curated, royalty-free sounds"; Flags: postinstall shellexec skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
