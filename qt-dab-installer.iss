
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "qt-dab"
#define MyAppVersion "2.0.1"
#define MyAppPublisher "Lazy Chair Computing"
#define MyAppURL "https://github.com/JvanKatwijk/qt-dab"
#define MyAppExeName "qt-dab-2.0.exe";

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId= {{0BD3638B-D634-4F75-B8BF-756A9FB7A370}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=E:\sdr-j-development\windows-qt-dab\COPYRIGHT.this_software
InfoBeforeFile=E:\sdr-j-development\windows-qt-dab\preamble.txt
OutputBaseFilename=setup-qt-dab
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "E:\sdr-j-development\windows-qt-dab\qt-dab-2.0.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\sdr-j-development\windows-qt-dab\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "E:\sdr-j-development\SDRplay_RSP_API-Windows-2.13.1.exe"; DestDir: "{app}"; AfterInstall : install_sdrplayApi

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
