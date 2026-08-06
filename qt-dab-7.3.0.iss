
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "qt-dab-7.3.0"
#define MyAppVersion "7.3.0"
#define MyAppPublisher "Lazy Chair Computing"
#define MyAppURL "https://github.com/JvanKatwijk/qt-dab"
#define MyAppExeName "qt-dab-7.3.0.exe";

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId= {{5E505179-78F7-4720-8867-1DDFA85FFDFA}}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=D:\w64-programs\windows-dab64-qt\COPYRIGHT.this_software
InfoBeforeFile=D:\w64-programs\windows-dab64-qt\preamble.txt
OutputBaseFilename=setup-qt-dab-7.3.0
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\w64-library\qt-dab-7.3.0.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\w64-library\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
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
          MsgBox ('Software\MiricsSDR\API not found, installing', mbInformation, MB_OK);
          Exec (ExpandConstant('{app}\SDRplay_RSP_API-Windows-2.13.1.exe'), '', '', SW_SHOWNORMAL,
          ewWaitUntilTerminated, ResultCode);
       end
end;
