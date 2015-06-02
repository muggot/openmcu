;--------------------------------

!include "MUI.nsh"
!include LogicLib.nsh

;--------------------------------

Name "OpenMCU-ru"

;--------------------------------

OutFile "openmcu-ru-VERSION-win32_setup.exe"

SetCompressor lzma

;--------------------------------

InstallDir "$PROGRAMFILES\OpenMCU-ru"
InstallDirRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "InstallationDir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

;Variables
Var StartMenuFolder

;Interface Settings
!define MUI_ABORTWARNING

;--------------------------------

; Pages

!define MUI_ICON "openmcu-ru\share\mcu.ico"
!define MUI_LICENSEPAGE_TEXT_BOTTOM "$(LicenseText)"
!define MUI_LICENSEPAGE_BUTTON "$(LicenseNext)"
!insertmacro MUI_PAGE_LICENSE "openmcu-ru\COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE "$(FinishTitle)"
!define MUI_FINISHPAGE_TEXT "$(FinishText)"
!define MUI_FINISHPAGE_SHOWREADME "http://127.0.0.1:1420/"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "$(FinishWebInt)"
;!define MUI_FINISHPAGE_LINK "$(FinishLink) http://openmcu.ru/"
;!define MUI_FINISHPAGE_LINK_LOCATION "http://openmcu.ru/"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

;Languages

!insertmacro MUI_LANGUAGE "English"
!include "OpenMCU-ru_en.nsh"
!insertmacro MUI_LANGUAGE "Russian"
!include "OpenMCU-ru_ru.nsh"

;--------------------------------

;Reserve Files

;If you are using solid compression, files that are required before
;the actual installation should be stored first in the data block,
;because this will make your installer start faster.
!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------

Function .onInit

;   ;Show language dialog on start
;   !insertmacro MUI_LANGDLL_DISPLAY

;   ;Doesn't allow install if already installed 
;   ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "DisplayName"
;   ${IfNot} ${Errors}
;     MessageBox MB_OK "$(RemovePrevious)"
;     Quit
;   ${EndIf}

  ;Check administrator rights
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "DisplayName" "OpenMCU-ru"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(NotAdmin)"
    Quit
  ${EndIf}
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru"
FunctionEnd

;--------------------------------

Section $(SecMainName) SecMain
SectionIn 1 2 RO

  SetOutPath "$INSTDIR"

  File /r "openmcu-ru\*.*"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "DisplayName" "$(DisplayName)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder\$(MenuService)"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuService)\$(MenuServiceInstall).lnk" "$INSTDIR\openmcu.exe" "Install" "" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuService)\$(MenuServiceUninstall).lnk" "$INSTDIR\openmcu.exe" "DeInstall" "" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuService)\$(MenuServiceStart).lnk" "net" "start OpenMCU-ru" "" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuService)\$(MenuServiceStop).lnk" "net" "stop OpenMCU-ru" "" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuStartDebug).lnk" "$INSTDIR\openmcu.exe" "debug" "$INSTDIR\openmcu.exe" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuStartTray).lnk" "$INSTDIR\openmcu.exe" "Tray" "$INSTDIR\openmcu.exe" 0 SW_SHOWNORMAL
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuWebinterface).lnk" "http://127.0.0.1:1420/" "" "$INSTDIR\openmcu.exe" 0 SW_SHOWNORMAL
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder\$(MenuUninstall)"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(MenuUninstall)\$(MenuUninstallLink).lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 SW_SHOWNORMAL

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------

Section $(SecVCName) SecVC
SectionIn 1 2

  SetOutPath "$INSTDIR"

  File vcredist_2005_x86.exe
  ExecWait "$INSTDIR\vcredist_2005_x86.exe /Q"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(ErrorOn) Microsoft Visual C++ redistributable 2005"
    Quit
  ${EndIf}
  Delete "$INSTDIR\vcredist_2005_x86.exe"

  File vcredist_2010_x86.exe
  ExecWait "$INSTDIR\vcredist_2010_x86.exe /passive"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(ErrorOn) Microsoft Visual C++ redistributable 2010"
    Quit
  ${EndIf}
  Delete "$INSTDIR\vcredist_2010_x86.exe"

SectionEnd

;--------------------------------

Section $(SecServiceName) SecService
SectionIn 1 2

  ExecWait "$INSTDIR\openmcu.exe Install"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(ErrorOn) openmcu.exe Install"
    Quit
  ${EndIf}
  Sleep 1000
  ExecWait "net start OpenMCU-ru"
  Sleep 1000
  Exec "$INSTDIR\openmcu.exe Tray"

SectionEnd

;--------------------------------

;Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(SecMainDesc)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecVC} $(SecVCDesc)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecService} $(SecServiceDesc)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------

Section "Uninstall"

  ExecWait "net stop OpenMCU-ru"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(ErrorOn) net stop OpenMCU-ru"
  ${EndIf}
  Sleep 1000
  ExecWait "$INSTDIR\openmcu.exe DeInstall"
  ${If} ${Errors}
    MessageBox MB_ICONSTOP "$(ErrorOn) openmcu.exe DeInstall"
  ${EndIf}
  Sleep 1000

  FindWindow $0 "OpenMCU-ru"
  ${IfNot} $0 == 0
    SendMessage $0 ${WM_CLOSE} 0 0 /TIMEOUT=1000
    Sleep 1000
  ${EndIf}

  RMDir /r "$INSTDIR\"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenMCU-ru"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  ${IfNot} $StartMenuFolder == ""
    SetShellVarContext all
;     Delete "$SMPROGRAMS\$StartMenuFolder\$(MenuService)\*.*"
;     RMDir "$SMPROGRAMS\$StartMenuFolder\$(MenuService)"
;     Delete "$SMPROGRAMS\$StartMenuFolder\$(MenuUninstall)\*.*"
;     RMDir "$SMPROGRAMS\$StartMenuFolder\$(MenuUninstall)"
;     Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
    RMDir /r "$SMPROGRAMS\$StartMenuFolder"
  ${EndIf}

SectionEnd
