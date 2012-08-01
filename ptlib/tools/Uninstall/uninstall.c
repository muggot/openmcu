/*
 *  Equivalence InstallShield Support DLL
 *
 *  Main source
 *
 *  Copyright 1997 by Equivalence Pty Ltd
 *
 *  $Log: uninstall.c,v $
 *  Revision 1.4  2001/03/29 23:33:00  robertj
 *  Added missing structure initialisation, thanks Victor H.
 *
 *  Revision 1.3  1999/08/20 05:56:22  robertj
 *  Fixed display of error on removing directory already removed.
 *
 *  Revision 1.2  1999/07/16 03:21:26  robertj
 *  Changed directory removal so does not show error it is already gone
 *
 *  Revision 1.1  1999/03/16 07:22:37  robertj
 *  Uninstall now kills tray icon before uninstalling.
 *  Install now checks that "localhost" works.
 *
 */

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <direct.h>

#include "resource.h"


#define MakeStr2(s) #s
#define MakeStr(s) MakeStr2(s)

#ifdef APP_NAME
#define APP_NAME_STR MakeStr(APP_NAME)
#pragma message("Compiling uninstaller for " APP_NAME_STR)
#else
#error Must define APP_NAME
#endif

#define STR_BUFF_SIZE   1024

static HANDLE dllInstance;

static const char AppName[] = APP_NAME_STR;
static const char UninstallKey[] =
      "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" APP_NAME_STR;
static const char UninstallValue[] = "UninstallString";



/***********************************

    Misc. functions

 ***********************************/

#ifdef _DEBUG

void Display_DebugWindow(char * fmt, ...)
{
  char str[STR_BUFF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsprintf(str, fmt, args);
  MessageBox(NULL, str, "Info", MB_ICONINFORMATION | MB_APPLMODAL);
}

#define DebugWindow(args) Display_DebugWindow args

#else

#define DebugWindow(args)

#endif


BOOL GetRegistryKey(const char * keyStr, const char * name, char * strbuff)
{
  HKEY key;
  DWORD valLen = STR_BUFF_SIZE;

  // open key
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyStr, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
    DebugWindow(("GetRegistryKey: RegOpenKeyEx of %s failed", keyStr));
    return FALSE;
  }

  // get the key value
  if (RegQueryValueEx(key, name, NULL, NULL, (LPBYTE)strbuff, &valLen) != ERROR_SUCCESS) {
    DebugWindow(("GetRegistryKey: RegQueryValueEx of %s failed", name));
    return FALSE;
  }

  return TRUE;
}


BOOL SetRegistryKey(const char * keyStr, const char * name, const char * val)
{
  HKEY key;

  // open key
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyStr, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
    DebugWindow(("SetRegistryKey: RegOpenKeyEx of %s failed", keyStr));
    return FALSE;
  }

  // put the new value back
  if (RegSetValueEx(key, name, 0, REG_SZ, (LPBYTE)val, strlen(val)+1) != ERROR_SUCCESS) {
    DebugWindow(("SetRegistryKey: RegSetValueEx of %s failed", name));
    return FALSE;
  }

  return TRUE;
}


BOOL DeleteRegistryKey(HKEY baseKey, const char * keyStr, const char * name)
{
  HKEY key;

  // open key
  if (RegOpenKeyEx(baseKey, keyStr, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
    DebugWindow(("DeleteRegistryKey: RegOpenKeyEx of %s failed", keyStr));
    return FALSE;
  }

  // put the new value back
  if (RegDeleteKey(key, name) != ERROR_SUCCESS) {
    DebugWindow(("DeleteRegistryKey: RegDeleteKey of %s failed", name));
    return FALSE;
  }

  return TRUE;
}


int vMessageBox(HWND hWnd, UINT id, UINT flags, ...)
{
  char fmt[1024];
  char buffer[2048];
  va_list args;
  va_start(args, flags);

  LoadString(dllInstance, id, fmt, sizeof(fmt));
  vsprintf(buffer, fmt, args);

  return MessageBox(hWnd, buffer, "Uninstaller", flags);
}


#if defined(DEL_DIR) || defined(DEL_FILE)

BOOL GoToInstallDir()
{
  char  str[STR_BUFF_SIZE];
  char *pos, *pos2;

  // get the deinstall command from the registry
  if (!GetRegistryKey(UninstallKey, UninstallValue, str))
    return FALSE;

  // extract the last argument
  pos = str;
  for (;;) {
    pos2 = strstr(pos, "-c");
    if (pos2 == NULL)
      break;
    pos = pos2+2;
  }
  if (pos == str)
    return FALSE;

  if (*pos = '\"')
    pos++;

  // remove the DLL name from the argument
  pos2 = strrchr(pos, '\\');
  if (pos2 == NULL)
    return FALSE;

  *pos2 = '\0';
  _chdir(str);

  return TRUE;
}

#endif

#ifdef DEL_DIR

struct FileElement {
  char   * name;
  DWORD  attr;
  struct FileElement * next;
};


BOOL DelTree(const char * dir)
{
  HANDLE          h;
  char            str[STR_BUFF_SIZE];
  char            subStr[STR_BUFF_SIZE];
  WIN32_FIND_DATA findData;

  struct FileElement * head = NULL;
  struct FileElement * newItem, * next; 

  strcpy(str, dir);
  if (str[strlen(str)-1] != '\\')
    strcat(str, "\\");
  strcat(str, "*.*");

  // make a pass through the directory collecting information
  h = FindFirstFile(str, &findData);
  if (h != INVALID_HANDLE_VALUE) do {

    if (((findData.cFileName[0] == '.') && (findData.cFileName[1] == '\0')) ||
        ((findData.cFileName[0] == '.') && (findData.cFileName[1] == '.') && (findData.cFileName[2] == '\0')))
      continue;

    newItem = (struct FileElement * )malloc(sizeof(struct FileElement));
    newItem->name = malloc(strlen(findData.cFileName)+1);
    strcpy(newItem->name, findData.cFileName);
    newItem->attr = findData.dwFileAttributes;
    newItem->next = head;
    head = newItem;

  } while (FindNextFile(h, &findData));

  FindClose(h);

  // make a second pass through the list deleting files
  while (head != NULL) {
    sprintf(subStr, "%s\\%s", dir, head->name);

    if ((head->attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
      DelTree(subStr);

    else {
      if (DeleteFile(subStr) == 0)
        vMessageBox(NULL, IDS_ERROR_DELETING_FILE, MB_ICONWARNING|MB_OK, subStr, GetLastError());
    }

    next = head->next;
    free(head->name);
    free(head);
    head = next;
  }

  if (!RemoveDirectory(dir)) {
    DWORD err = GetLastError();
    if (err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND)
      vMessageBox(NULL, IDS_ERROR_DELETING_DIR, MB_ICONWARNING|MB_OK, dir, err);
  }

  return TRUE;
}

#endif


/***********************************


  DLL startup function - needed to obtain the handle for the DLL


***********************************/

BOOL APIENTRY DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      dllInstance = hInst;
      break;

    case DLL_PROCESS_DETACH:
      break;

    default:
      break;
  }

  return TRUE;
}

/***********************************

  INSTALL FUNCTION

  This function is called during the install procedure. It modifies the
  registry string which specifies the uninstall command so that this DLL is
  called during the uninstall process.

  The registry entry to be modified is:

  HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\<Application Name>\UninstallString

  Before entry, the string will look like:

     D:\Win95\Uninst.exe -fC:\Program Files\MyApp\Deisl1.isu

  After exit, ste string must look like:

     D:\Win95\Uninst.exe -f"C:\Program Files\MyApp\Deisl1.isu" -c"C:\Program Files\MyApp\MyUninst.dll
     -f"<INSTALLDIR>\Deisl1.isu" -c"<INSTALLDIR>\uninstall.dll

  Note the addition of " characters around the first argument and the absence
  of a trailing "

***********************************/

//
//  Called during installation
//    Return:  1 to continue installation
//             0 to stop installation
//

char APIENTRY InstallFunc(HWND hwnd,
                          LPSTR szSrcDir,
                          LPSTR szSupport,
                          LPSTR szInst,
                          LPSTR szRes)
{
  char str[STR_BUFF_SIZE];
  char dllName   [STR_BUFF_SIZE];
  WSADATA wsdat;

  DebugWindow(("Custom DLL: running InstallFunc for " APP_NAME_STR));

  // open key
  if (!GetRegistryKey(UninstallKey, UninstallValue, str))
    return 0;

  // add the start of the new command
  strcat(str, " -c\"");

  // get full path of the DLL
  if (GetModuleFileName(dllInstance, dllName, STR_BUFF_SIZE) == 0) {
    DebugWindow(("InstallFunc: GetModuleFileName failed"));
    return 0;
  }

  // add the name of the DLL to the command line
  strcat(str, strlwr(dllName));

  // put the new value back
  if (!SetRegistryKey(UninstallKey, UninstallValue, str))
    return 0;

  DebugWindow(("InstallFunc: New uninstall command line is \"%s\"", str));

  if (WSAStartup(0x101, &wsdat) != 0) {
    vMessageBox(hwnd, IDS_NO_WINSOCK, MB_OK | MB_ICONWARNING | MB_APPLMODAL, AppName);
    return 0;
  }

  if (gethostbyname("localhost") != NULL)
    DebugWindow(("InstallFunc: localhost OK"));
  else {
    FILE * file;
    OSVERSIONINFO  osInfo;
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);

    GetWindowsDirectory(str, sizeof(str));
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
      strcat(str, "\\system32\\drivers\\etc\\");
    strcat(str, "\\hosts");

    if ((file = fopen(str, "a+")) == NULL)
      DebugWindow(("InstallFunc: Could not open ", str));
    else {
      fputs("127.0.0.1 localhost\n", file);
      fclose(file);
      DebugWindow(("InstallFunc: Adding local host to ", str));
    }
  }

  WSACleanup();

  return 1;
}

/***********************************

 DEINSTALL FUNCTION

 These functions are used to remove the if it was installed as a service (under NT) 
 or to remove the program from the RunServices list (under Win95)

 ***********************************/

// Enumeration function to find running applications
BOOL CALLBACK FindApplication(HWND hwnd, LPARAM lParam)
{
  BOOL * found = (BOOL *)lParam;
  char str[100];

  if (GetWindowText(hwnd, str, sizeof(str)) == 0)
    return TRUE;
  
  DebugWindow(("Custom DLL: FindApplication window %s", str));
  if (stricmp(str, AppName) != 0)
    return TRUE;

  if (GetClassName(hwnd, str, sizeof(str)) == 0)
    return TRUE;

  DebugWindow(("Custom DLL: FindApplication class %s", str));
  if (stricmp(str, AppName) != 0)
    return TRUE;

  *found = TRUE;
  DebugWindow(("Custom DLL: FindApplication destroying window"));
  SendMessage(hwnd, WM_CLOSE, 0, 0L);
  return TRUE;
}


//
//  Called before doing any deinstallation
//    Return:   0 to continue deinstallation
//            < 0 to stop deinstallation
//

LONG APIENTRY UninstInitialize(HWND hwndDlg,      // main dialog handle
                               HANDLE hInstance,  // main application instance
                               LONG lReserved)    // reserved
{
  OSVERSIONINFO  osInfo;
  char           str[STR_BUFF_SIZE];
  HANDLE         hEvent;
  SC_HANDLE      scHandle, serviceHandle;
  SERVICE_STATUS serviceStatus;
  BOOL           waitForWindowDestruction;

  DebugWindow(("Custom DLL: UninstInitialize for " APP_NAME_STR));

  // initialise the structure
  osInfo.dwOSVersionInfoSize  = sizeof(OSVERSIONINFO);

  // get the version information
  if (!GetVersionEx(&osInfo)) {
    DebugWindow(("UninstInitialize: GetVersionEx failed"));
    return 0;
  }

  // if the process is still running, then warn the user and stop the install process
  if ((hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, AppName)) == NULL)
    DebugWindow(("UninstInitialize: Process " APP_NAME_STR " not running"));
  else {
    if (vMessageBox(hwndDlg,
                    IDS_STILL_RUNNING,
                    MB_YESNO | MB_ICONWARNING | MB_APPLMODAL,
                    AppName) == IDYES)
      return -1;

    //
    // if Windows 95....
    //
    if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      DebugWindow(("UninstInitialize: Windows 95 deinstall"));
      while (hEvent != NULL) {
        SetEvent(hEvent);
        CloseHandle(hEvent);
        Sleep(3000);
        if ((hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, AppName)) != NULL) {
          switch (vMessageBox(hwndDlg,
                              IDS_STOP_FAILED,
                              MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_APPLMODAL,
                              AppName)) {
            case IDABORT :
              return -1;

            case IDIGNORE :
              CloseHandle(hEvent);
              hEvent = NULL;
          }
        }
      }
    }

    //
    // if Windows NT....
    //
    else if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      DebugWindow(("UninstInitialize: Windows NT deinstall"));

      if ((scHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) != NULL) {

        // get a handle to the service
        if ((serviceHandle = OpenService(scHandle, AppName, SERVICE_ALL_ACCESS)) != NULL) {

          // stop the service
          ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus);

          CloseServiceHandle(serviceHandle);
        }
        CloseServiceHandle(scHandle);
      }
    }

    if (hEvent != NULL)
      CloseHandle(hEvent);
  }

  // See if there are any tray icons or control windows active, and kill them
  waitForWindowDestruction = FALSE;
  EnumWindows(FindApplication, (LPARAM)&waitForWindowDestruction);
  if (waitForWindowDestruction)
    Sleep(1000);


  //
  // if Windows 95....
  //
  if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {

    // remove key
    DeleteRegistryKey(HKEY_LOCAL_MACHINE, 
                      "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",
                      AppName);

    // get directory containing log file
    if (GetWindowsDirectory(str, STR_BUFF_SIZE) != 0) {

      // make name of log file
      strcat(str, "\\" APP_NAME_STR " Log.TXT");

      DebugWindow(("UninstInitialize: Removing log file %s", str));

      // delete the file
      if (remove(str) != 0)
        DebugWindow(("UninstInitialize: Log file not removed"));
    }
  }

  //
  // if Windows NT....
  //
  else if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {

    // get a handle to the service manager
    if ((scHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) != NULL) {

      // get a handle to the service
      if ((serviceHandle = OpenService(scHandle, AppName, SERVICE_ALL_ACCESS)) != NULL) {

        // delete the service
        DeleteService(serviceHandle);

        // close the service handle
        CloseServiceHandle(serviceHandle);
      }

      // close the service manager handle
      CloseServiceHandle(scHandle);
    }

    // clear the event log
    RegDeleteKey(HKEY_LOCAL_MACHINE,
                 "System\\CurrentControlSet\\Services\\EventLog\\Application\\" APP_NAME_STR);
  }

  // remove tray icon key
  DeleteRegistryKey(HKEY_LOCAL_MACHINE, 
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    AppName);

#if defined(DEL_DIR) || defined(DEL_FILE)
  GoToInstallDir();
#ifdef DEL_DIR_KEY
#pragma message("Deleting directory in registry key \"" MakeStr(DEL_DIR_KEY) "\\" MakeStr(DEL_DIR) "\"")
  if (!GetRegistryKey("SOFTWARE\\Equivalence\\" APP_NAME_STR "\\CurrentVersion\\" MakeStr(DEL_DIR_KEY),
                      MakeStr(DEL_DIR), str))
    str[0] = '\0';
  if (str[0] != '\0')
    DelTree(str);
#elif defined(DEL_DIR)
#pragma message("Deleting directory at \"" MakeStr(DEL_DIR) "\"")
  DelTree(MakeStr(DEL_DIR));
#else
#pragma message("Deleting file at \"" MakeStr(DEL_FILE) "\"")
  if (remove(MakeStr(DEL_FILE)) == 0)
    DebugWindow(("UninstInitialize: deleted " MakeStr(DEL_FILE)));
  else
    DebugWindow(("UninstInitialize: could not delet " MakeStr(DEL_FILE)));

#endif
  chdir("\\");
#endif

  DebugWindow(("UninstInitialize: cleanup complete"));

  return 0;
}


//
//  Called after logged items are deinstalled
//  Return value ignored
//

LONG APIENTRY UninstUnInitialize(HWND hwndDlg,      // main dialog handle
                                 HANDLE hInstance,  // main application instance
                                 LONG lReserved)    // reserved
{
  DebugWindow(("Custom DLL: UninstUnInitialize for " APP_NAME_STR));

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
