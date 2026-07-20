#define AppName "Pure Gaussian Blur for Streamlabs"
#define AppVersion "0.1.7"
#define Payload "..\outputs\MotionFrostedGlass-0.1.7\plugin"

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
VersionInfoVersion=0.1.7.0
VersionInfoDescription=Pure Gaussian Blur filter installer for Streamlabs Desktop with OBS Core 31.1.2sl19b3
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

function IsBuildMarkerChar(Value: AnsiChar): Boolean;
begin
  Result := ((Value >= '0') and (Value <= '9')) or
            ((Value >= 'A') and (Value <= 'Z')) or
            ((Value >= 'a') and (Value <= 'z')) or
            (Value = '.') or (Value = '-');
end;

function GetObsBuildMarker: String;
var
  Data: AnsiString;
  StartIndex: Integer;
  EndIndex: Integer;
  Candidate: AnsiString;
begin
  Result := 'unknown';
  if not LoadStringFromFile(GetRuntimeRoot + '\obs.dll', Data) then
    Exit;

  for StartIndex := 1 to Length(Data) - 6 do
  begin
    if Copy(Data, StartIndex, 7) = '31.1.2' then
    begin
      EndIndex := StartIndex + 7;
      while (EndIndex <= Length(Data)) and
            ((EndIndex - StartIndex) < 64) and
            IsBuildMarkerChar(Data[EndIndex]) do
        EndIndex := EndIndex + 1;
      Candidate := Copy(Data, StartIndex, EndIndex - StartIndex);
      if (Pos('31.1.2sl', Candidate) = 1) or
         (Pos('31.1.2ndi', Candidate) = 1) then
      begin
        Result := String(Candidate);
        Exit;
      end;
    end;
  end;
end;

function InitializeSetup: Boolean;
var
  StreamlabsVersion: String;
  ObsVersion: String;
  ObsMajor: Word;
  ObsMinor: Word;
  ObsRevision: Word;
  ObsBuild: Word;
  ObsBuildMarker: String;
begin
  Result := False;
  StreamlabsRoot := ExpandConstant('{autopf}\Streamlabs OBS');

  if not FileExists(StreamlabsRoot + '\Streamlabs OBS.exe') then
  begin
    MsgBox('找不到 Streamlabs Desktop。請先安裝 64 位元 Streamlabs Desktop。'#13#10 +
      'Streamlabs Desktop was not found. Install the 64-bit version first.', mbError, MB_OK);
    Exit;
  end;

  if not GetVersionNumbersString(StreamlabsRoot + '\Streamlabs OBS.exe', StreamlabsVersion) then
    StreamlabsVersion := 'unknown';

  if not GetVersionNumbersString(GetRuntimeRoot + '\obs.dll', ObsVersion) then
    ObsVersion := 'unknown';

  if not GetVersionComponents(GetRuntimeRoot + '\obs.dll', ObsMajor, ObsMinor, ObsRevision, ObsBuild) then
  begin
    MsgBox('無法讀取 OBS 核心版本。Streamlabs Desktop：' + StreamlabsVersion + #13#10 +
      'Unable to read the OBS Core version.', mbError, MB_OK);
    Exit;
  end;
  ObsBuildMarker := GetObsBuildMarker;
  if (ObsMajor <> 31) or (ObsMinor <> 1) or
     (CompareText(ObsBuildMarker, '31.1.2sl19b3') <> 0) then
  begin
    MsgBox('此安裝器需要已驗證的 OBS Core 31.1.2sl19b3。偵測到 OBS：' + ObsVersion +
      '（' + ObsBuildMarker + '）' +
      '；Streamlabs Desktop：' + StreamlabsVersion + #13#10 +
      'This installer requires verified OBS Core 31.1.2sl19b3. Detected OBS: ' + ObsVersion +
      ' (' + ObsBuildMarker + ')' +
      '; Streamlabs Desktop: ' + StreamlabsVersion, mbError, MB_OK);
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
