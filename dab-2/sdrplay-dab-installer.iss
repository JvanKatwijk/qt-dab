
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "sdrplayDab"
#define MyAppVersion "3.4"
#define MyAppPublisher "Lazy Chair Computing"
#define MyAppURL "https://github.com/JvanKatwijk/sdrplayDab"
#define MyAppExeName "sdrplayDab-3.4.exe";

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A534EB72-411B-49A7-9E9D-5B8A7E90536A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=D:\sdr-j-development\windows-sdrplay\COPYRIGHT.this_software
InfoBeforeFile=D:\sdr-j-development\windows-sdrplay\preamble.txt
OutputBaseFilename=setup-sdrplayDab
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\sdr-j-development\windows-sdrplay\sdrplayDab-3.4.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\sdr-j-development\windows-sdrplay\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "C:\Users\JvanK\Downloads\SDRplay_RSP_API-Windows-2.13.1.exe"; DestDir: "{app}"; AfterInstall : install_sdrplayApi

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[code]
procedure install_sdrplayApi;
var
    resultCode : Integer;
    Names      : TArrayOfString;
    I          : Integer;
    found      : Boolean;

 begin

    RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\MiricsSDR', Names);
    for I := 0 to GetArrayLength(Names)-1 do
       if Names [I] = 'API' then found := true;

    if not found
    then
       begin
          MsgBox ('Software\MiricsSDR\API not found', mbInformation, MB_OK);
          Exec (ExpandConstant('{app}\SDRplay_RSP_API-Windows-2.13.1.exe'), '', '', SW_SHOWNORMAL,
          ewWaitUntilTerminated, ResultCode);
       end
end;

