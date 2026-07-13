#define AppName "Pure Gaussian Blur for Streamlabs"
#define AppVersion "0.1.6"
#define Payload "..\outputs\MotionFrostedGlass-0.1.6\plugin"

[Setup]
AppId={{A8BD55D5-878B-4A4F-9EE2-3DBFF90D91C7}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher=Pure Gaussian Blur
DefaultDirName={autopf}\Pure Gaussian Blur for Streamlabs
DisableDirPage=yes
DisableProgramGroupPage=yes
UninstallDisplayName={#AppName}
OutputDir=..\outputs
OutputBaseFilename=MotionFrostedGlass-Setup-{#AppVersion}
Compression=lzma2/max
SolidCompression=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
SetupLogging=yes
CloseApplications=no
RestartApplications=no
VersionInfoVersion=0.1.6.0
VersionInfoDescription=Pure Gaussian Blur filter installer for Streamlabs Desktop 1.21.4
VersionInfoProductName={#AppName}
VersionInfoProductVersion={#AppVersion}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesetraditional"; MessagesFile: "Languages\ChineseTraditional.isl"

[Files]
Source: "{#Payload}\motion-frosted-glass.dll"; DestDir: "{code:GetPluginDirectory}"; Flags: ignoreversion
Source: "{#Payload}\data\effects\blur.effect"; DestDir: "{code:GetDataDirectory}\effects"; Flags: ignoreversion
Source: "{#Payload}\data\effects\frosted-glass.effect"; DestDir: "{code:GetDataDirectory}\effects"; Flags: ignoreversion
Source: "{#Payload}\data\effects\output.effect"; DestDir: "{code:GetDataDirectory}\effects"; Flags: ignoreversion
Source: "{#Payload}\data\locale\en-US.ini"; DestDir: "{code:GetDataDirectory}\locale"; Flags: ignoreversion
Source: "{#Payload}\data\locale\zh-TW.ini"; DestDir: "{code:GetDataDirectory}\locale"; Flags: ignoreversion

[Run]
Filename: "{code:GetStreamlabsRoot}\Streamlabs OBS.exe"; Description: "{cm:LaunchProgram,Streamlabs Desktop}"; Flags: nowait postinstall skipifsilent

[Code]
var
  StreamlabsRoot: String;

function GetStreamlabsRoot(Param: String): String;
begin
  Result := StreamlabsRoot;
end;

function GetRuntimeRoot: String;
begin
  Result := StreamlabsRoot + '\resources\app.asar.unpacked\node_modules\obs-studio-node';
end;

function GetPluginDirectory(Param: String): String;
begin
  Result := GetRuntimeRoot + '\obs-plugins\64bit';
end;

function GetDataDirectory(Param: String): String;
begin
  Result := GetRuntimeRoot + '\data\obs-plugins\motion-frosted-glass';
end;

function StreamlabsIsRunning: Boolean;
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{cmd}'),
    '/C tasklist /FI "IMAGENAME eq Streamlabs OBS.exe" /NH | find /I "Streamlabs OBS.exe" >nul',
    '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Result := ResultCode = 0;
end;

function InitializeSetup: Boolean;
var
  StreamlabsVersion: String;
  ObsVersion: String;
begin
  Result := False;
  StreamlabsRoot := ExpandConstant('{autopf}\Streamlabs OBS');

  if not FileExists(StreamlabsRoot + '\Streamlabs OBS.exe') then
  begin
    MsgBox('找不到 Streamlabs Desktop。請先安裝 64 位元 Streamlabs Desktop 1.21.4。'#13#10 +
      'Streamlabs Desktop was not found. Install the 64-bit version 1.21.4 first.', mbError, MB_OK);
    Exit;
  end;

  if not GetVersionNumbersString(StreamlabsRoot + '\Streamlabs OBS.exe', StreamlabsVersion) or
     (CompareText(StreamlabsVersion, '1.21.4.0') <> 0) then
  begin
    MsgBox('此安裝器只支援 Streamlabs Desktop 1.21.4。偵測到版本：' + StreamlabsVersion + #13#10 +
      'This installer only supports Streamlabs Desktop 1.21.4.', mbError, MB_OK);
    Exit;
  end;

  if not GetVersionNumbersString(GetRuntimeRoot + '\obs.dll', ObsVersion) or
     (CompareText(ObsVersion, '31.1.2.0') <> 0) then
  begin
    MsgBox('Streamlabs OBS runtime 不相容。偵測到版本：' + ObsVersion, mbError, MB_OK);
    Exit;
  end;

  if StreamlabsIsRunning then
  begin
    MsgBox('請先完全關閉 Streamlabs Desktop，再重新執行安裝器。'#13#10 +
      'Close Streamlabs Desktop completely, then run this installer again.', mbError, MB_OK);
    Exit;
  end;

  Result := True;
end;
