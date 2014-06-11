
!verbose 3

!ifdef CURLANG
    !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

LangString RemovePrevious ${CURLANG} "Please uninstall the previous OpenMCU-ru installation first"

LangString LicenseText ${CURLANG} "OpenMCU-ru is released under the Mozilla Public License. Please carefully read the license. Click Accept to continue."
LangString LicenseNext ${CURLANG} "&Accept"
LangString SecMainName ${CURLANG} "OpenMCU-ru"
LangString SecMainDesc ${CURLANG} "Install OpenMCU-ru and other required files"
LangString SecVCName ${CURLANG} "vc_redist.exe (required)"
LangString SecVCDesc ${CURLANG} "Microsoft Visual C++ 2010 SP1 Redistributable Package (x86) (required)"
LangString SecServiceName ${CURLANG} "Install service"
LangString SecServiceDesc ${CURLANG} "Install OpenMCU-ru as service"

LangString NotAdmin ${CURLANG} "No write permission. $\nMaybe you are not an administrator of this computer. $\nThe installation can be done only by administrator."
LangString ErrorOn ${CURLANG} "Error on:"
LangString DisplayName ${CURLANG} "Videoconference server OpenMCU-ru"

LangString MenuService ${CURLANG} "Service"
LangString MenuServiceInstall ${CURLANG} "Install OpenMCU-ru service"
LangString MenuServiceUninstall ${CURLANG} "Uninstall OpenMCU-ru service"
LangString MenuServiceStart ${CURLANG} "Start OpenMCU-ru service"
LangString MenuServiceStop ${CURLANG} "Stop OpenMCU-ru service"
LangString MenuUninstall ${CURLANG} "Uninstall"
LangString MenuUninstallLink ${CURLANG} "Uninstall OpenMCU-ru"
LangString MenuStartDebug ${CURLANG} "Start OpenMCU-ru in debug mode"
LangString MenuStartTray ${CURLANG} "OpenMCU-ru tray icon"
LangString MenuWebinterface ${CURLANG} "OpenMCU-ru webinterface"

LangString FinishTitle ${CURLANG} "Installation complete"
LangString FinishText ${CURLANG} "Videoconference server OpenMCU-ru successfully installed."
LangString FinishWebInt ${CURLANG} "Open OpenMCU-ru webinterface"
LangString FinishLink ${CURLANG} "Project website:"
