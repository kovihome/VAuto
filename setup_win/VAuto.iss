; Inno Setup Script
; Created with ScriptMaker Version 2.0.10
; 28 július 2003 at 23:34

[Setup]
MinVersion=4.1.1998,4.0.1381sp4
AppName=VAuto
AppId=VAuto
AppVersion=1.2
AppVerName=VAuto 1.2
AppCopyright=Copyright © KVI, 2009-2011.
DefaultDirName={pf}\Lufi\VAuto
CreateUninstallRegKey=true
UsePreviousAppDir=true
UsePreviousGroup=true
WizardStyle=modern
UninstallStyle=modern
WindowShowCaption=false
WindowStartMaximized=false
WindowVisible=false
WindowResizable=false
UninstallDisplayIcon={app}\VAuto.exe
UninstallDisplayName=VAuto
UninstallLogMode=append
DirExistsWarning=auto
UninstallFilesDir={app}
DisableDirPage=false
DisableStartupPrompt=true
CreateAppDir=true
DisableProgramGroupPage=false
Uninstallable=true
Compression=bzip
SolidCompression=false
DefaultGroupName=VAuto
InfoBeforeFile=Image\Presetup\readme.hun
OutputBaseFilename=VAuto12hu-alpha
ShowComponentSizes=true
SourceDir=.\
OutputDir=.\


ShowLanguageDialog=auto
WizardImageFile=Image\Presetup\vauto_setup_iss.bmp
WizardSmallImageFile=Image\Presetup\vauto_setup_iss_small.bmp
VersionInfoVersion=1.2
VersionInfoCompany=Lufi Klub
VersionInfoDescription=VAuto
WizardImageStretch=false
WizardImageBackColor=clBlack
PrivilegesRequired=none
SetupIconFile=Image\Presetup\VAuto.ico
VersionInfoCopyright=Kvi, 2009-2011.
VersionInfoProductName=VAuto
VersionInfoProductVersion=1.2
[Files]
Source: Image\Data\VAuto.exe; DestDir: {app}
Source: Image\Data\DPPDLL.dll; DestDir: {app}
Source: Image\Data\DPPLibCom.dll; DestDir: {app}
Source: Image\Data\DPPRSC.dll; DestDir: {app}
Source: Image\Data\EDSDK.dll; DestDir: {app}
Source: Image\Data\EdsImage.dll; DestDir: {app}
Source: Image\Data\MLib.dll; DestDir: {app}
Source: Image\Data\VAuto.xrc; DestDir: {app}
Source: Image\Data\MSVCR90.dll; DestDir: {sys}; Flags: comparetimestamp overwritereadonly
Source: Image\Data\msvcp90.dll; DestDir: {sys}; Flags: comparetimestamp overwritereadonly
Source: Image\Data\msvcm90.dll; DestDir: {sys}; Flags: comparetimestamp overwritereadonly 
Source: Image\Data\Microsoft.VC90.CRT.manifest; DestDir: {sys}; Flags: comparetimestamp overwritereadonly 
Source: Image\Data\dslr.ico; DestDir: {app}; Attribs: readonly
Source: Image\Data\delete24.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\dslr.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\help20.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\next18.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\openc24.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\settings20.png; DestDir: {app}; Attribs: readonly
Source: Image\Data\gcvs.csv; DestDir: {app}; Attribs: readonly
Source: Image\Data\nsv.csv; DestDir: {app}; Attribs: readonly
Source: Image\Data\ngc.csv; DestDir: {app}
Source: Image\Data\program.csv; DestDir: {app}
Source: Image\Data\locale\hu\LC_MESSAGES\VAuto.mo; DestDir: {app}\locale\hu\LC_MESSAGES\; Attribs: readonly

[Icons]
Name: {group}\VAuto; Filename: {app}\VAuto.exe; WorkingDir: {app}; IconFilename: {app}\VAuto.exe; IconIndex: 0
Name: {group}\VAuto Uninstall; Filename: {app}\unins000.exe; WorkingDir: {app}; IconFilename: {app}\unins000.exe; IconIndex: 0
Name: {userdesktop}\VAuto; Filename: {app}\VAuto.exe; WorkingDir: {app}; IconFilename: {app}\dslr.ico; IconIndex: 0; Languages: 

[INI]

[Registry]

[UninstallDelete]

[InstallDelete]

[Run]

[UninstallRun]

[Messages]
BeveledLabel=VAuto

[LangOptions]
LanguageName=Hungarian
LanguageID=$040e
DialogFontName=MS Shell Dlg
DialogFontSize=8
TitleFontName=Arial
TitleFontSize=29
WelcomeFontName=Arial
WelcomeFontSize=12
CopyrightFontName=Arial
CopyrightFontSize=8

[Languages]
Name: Magyar; MessagesFile: compiler:Languages\Hungarian.isl
