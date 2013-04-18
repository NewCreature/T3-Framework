[Setup]
AppName=T3F_APP_NAME
AppVerName=T3F_APP_NAME vT3F_APP_VERSION
AppCopyright=T3F_APP_COPYRIGHT
AppPublisher=T3F_APP_PUBLISHER
AppPublisherURL=T3F_APP_URL
AppSupportURL=T3F_APP_URL
AppUpdatesURL=T3F_APP_URL
DefaultDirName={pf}\T3F_APP_NAME
DefaultGroupName=T3F_APP_NAME
LicenseFile=T3F_APP_LICENSE_FILE
InfoAfterFile=T3F_APP_README_FILE
OutputDir=T3F_APP_INSTALLER_DIR
OutputBaseFilename=T3F_APP_INSTALLER_NAME
Compression=lzma
SolidCompression=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "T3F_APP_EXECUTABLE"; Destdir: "{app}"
Source: "T3F_APP_LIBS_DIR\*.dll"; Destdir: "{app}"
Source: "T3F_APP_USER_LIBS_DIR\*.dll"; Destdir: "{app}"; Flags: skipifsourcedoesntexist
Source: "T3F_APP_DATA_DIR\*.*"; DestDir: "{app}\data"; Flags: recursesubdirs

;[UninstallDelete]
;Type: files; Name: "{app}\alleg42.dll"
;Type: files; Name: "{app}\readme.txt"
;Type: files; Name: "{app}\soup.dat"
;Type: files; Name: "{app}\soup.exe"

[Icons]
Name: "{group}\T3F_APP_NAME"; Filename: "{app}\T3F_APP_EXECUTABLE"; WorkingDir: "{app}"
Name: "{group}\Readme"; Filename: "{app}\readme.txt"; WorkingDir: "{app}"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"; WorkingDir: "{app}"
Name: "{userdesktop}\T3F_APP_NAME"; Filename: "{app}\T3F_APP_EXECUTABLE"; WorkingDir: "{app}"
