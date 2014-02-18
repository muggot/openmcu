/*
 * wincfg.cxx
 *
 * Miscellaneous implementation of classes for Win32
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: wincfg.cxx,v $
 * Revision 1.21  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.20  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.19  2007/09/17 04:15:40  rjongbloed
 * Tidy up of some traces.
 *
 * Revision 1.18  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.17  2006/04/09 11:04:00  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.16  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2005/09/24 09:11:42  dominance
 * use unix-style slashes to not confuse mingw on win32
 *
 * Revision 1.14  2003/03/19 01:35:42  robertj
 * Fixed bug getting large private .ini file data, thanks Michal Zygmuntowicz
 *
 * Revision 1.13  2003/02/12 01:59:44  robertj
 * Changed to allow for very large private .INI files.
 *
 * Revision 1.12  2001/08/17 20:09:12  yurik
 * Fixed RegEnumValue name clash with system function
 *
 * Revision 1.11  2001/08/14 21:56:39  yurik
 * More CE Unicode Registry API bug fixes
 *
 * Revision 1.10  2001/08/14 15:41:13  yurik
 * Fixed bug in EnumKey for CE
 *
 * Revision 1.9  2001/03/15 23:41:22  robertj
 * Fixed bug just introduced so can access regisrtry directly again from PConfig.
 *
 * Revision 1.8  2001/03/09 05:50:48  robertj
 * Added ability to set default PConfig file or path to find it.
 *
 * Revision 1.7  2001/01/24 06:45:41  yurik
 * Windows CE port-related changes
 *
 * Revision 1.6  2000/09/05 02:28:38  robertj
 * Removed PAssert with registry access denied error, changed to PTRACE.
 *
 * Revision 1.5  2000/08/03 22:47:48  robertj
 * Removed assert for empty key name so can set registry default key for a section.
 *
 * Revision 1.4  2000/05/25 11:08:46  robertj
 * Added PConfig::HasKey() function to determine if value actually set.
 * Fixed "system" PConfig to use the win.ini file in correct directory.
 *
 * Revision 1.3  1999/12/30 00:32:48  robertj
 * Allowed absolute registry paths in PConfig::Application instances.
 *
 * Revision 1.2  1998/12/04 10:10:48  robertj
 * Added virtual for determining if process is a service. Fixes linkage problem.
 *
 * Revision 1.1  1998/11/30 05:35:28  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>

#include <winuser.h>
#include <winnls.h>

#define new PNEW


const char LocalMachineStr[] = "HKEY_LOCAL_MACHINE\\";
const char CurrentUserStr[] = "HKEY_CURRENT_USER\\";

///////////////////////////////////////////////////////////////////////////////
// Configuration files

#ifndef _WIN32_WCE
class SecurityID
{
  public:
    SecurityID(PSID_IDENTIFIER_AUTHORITY  pIdentifierAuthority,  // pointer to identifier authority
               BYTE nSubAuthorityCount,  // count of subauthorities
               DWORD dwSubAuthority0,  // subauthority 0
               DWORD dwSubAuthority1,  // subauthority 1
               DWORD dwSubAuthority2,  // subauthority 2
               DWORD dwSubAuthority3,  // subauthority 3
               DWORD dwSubAuthority4,  // subauthority 4
               DWORD dwSubAuthority5,  // subauthority 5
               DWORD dwSubAuthority6,  // subauthority 6
               DWORD dwSubAuthority7  // subauthority 7
              )
    {
      if (!AllocateAndInitializeSid(pIdentifierAuthority,  // pointer to identifier authority
                                    nSubAuthorityCount,  // count of subauthorities
                                    dwSubAuthority0,  // subauthority 0
                                    dwSubAuthority1,  // subauthority 1
                                    dwSubAuthority2,  // subauthority 2
                                    dwSubAuthority3,  // subauthority 3
                                    dwSubAuthority4,  // subauthority 4
                                    dwSubAuthority5,  // subauthority 5
                                    dwSubAuthority6,  // subauthority 6
                                    dwSubAuthority7,  // subauthority 7
                                    &sidptr))
        sidptr = NULL;
    }

    SecurityID(LPCTSTR lpSystemName,  // address of string for system name
               LPCTSTR lpAccountName,  // address of string for account name
               LPTSTR ReferencedDomainName,  // address of string for referenced domain 
               LPDWORD cbReferencedDomainName,  // address of size of domain string
               PSID_NAME_USE peUse   // address of SID-type indicator
              )
    {
      DWORD len = 1024;
      sidptr = (PSID)LocalAlloc(LPTR, len);
      if (sidptr != NULL) {
        if (!LookupAccountName(lpSystemName,  // address of string for system name
                               lpAccountName,  // address of string for account name
                               sidptr,  // address of security identifier
                               &len,  // address of size of security identifier
                               ReferencedDomainName,  // address of string for referenced domain 
                               cbReferencedDomainName,  // address of size of domain string
                               peUse   // address of SID-type indicator
                              )) {
          LocalFree(sidptr);
          sidptr = NULL;
        }
      }
    }
    ~SecurityID()
    {
      FreeSid(sidptr);
    }

    operator PSID() const
    {
      return sidptr;
    }

    DWORD GetLength() const
    {
      return GetLengthSid(sidptr);
    }

    BOOL IsValid() const
    {
      return sidptr != NULL && IsValidSid(sidptr);
    }

  private:
    PSID sidptr;
};


static DWORD SecureCreateKey(HKEY rootKey, const PString & subkey, HKEY & key)
{
  SECURITY_DESCRIPTOR secdesc;
  if (!InitializeSecurityDescriptor(&secdesc, SECURITY_DESCRIPTOR_REVISION))
    return GetLastError();

  static SID_IDENTIFIER_AUTHORITY siaNTAuthority = SECURITY_NT_AUTHORITY;
  SecurityID adminID(&siaNTAuthority, 2,
                     SECURITY_BUILTIN_DOMAIN_RID,
                     DOMAIN_ALIAS_RID_ADMINS, 
                     0, 0, 0, 0, 0, 0);
  if (!adminID.IsValid())
    return GetLastError();

  static SID_IDENTIFIER_AUTHORITY siaSystemAuthority = SECURITY_NT_AUTHORITY;
  SecurityID systemID(&siaSystemAuthority, 1,
                      SECURITY_LOCAL_SYSTEM_RID,
                      0, 0, 0, 0, 0, 0, 0);
  if (!systemID.IsValid())
    return GetLastError();

  static SID_IDENTIFIER_AUTHORITY siaCreatorAuthority = SECURITY_CREATOR_SID_AUTHORITY;
  SecurityID creatorID(&siaCreatorAuthority, 1,
                       SECURITY_CREATOR_OWNER_RID,
                       0, 0, 0, 0, 0, 0, 0);
  if (!creatorID.IsValid())
    return GetLastError();

  SID_NAME_USE snuType;
  char szDomain[100];
  DWORD cchDomainName = sizeof(szDomain);
  SecurityID userID(NULL, PProcess::Current().GetUserName(),
                    szDomain, &cchDomainName, &snuType);
  if (!userID.IsValid())
    return GetLastError();

  DWORD acl_len = sizeof(ACL) + 4*sizeof(ACCESS_ALLOWED_ACE) +
                    adminID.GetLength() + creatorID.GetLength() +
                    systemID.GetLength() + userID.GetLength();
  PBYTEArray dacl_buf(acl_len);
  PACL dacl = (PACL)dacl_buf.GetPointer(acl_len);
  if (!InitializeAcl(dacl, acl_len, ACL_REVISION2))
    return GetLastError();

  if (!AddAccessAllowedAce(dacl, ACL_REVISION2, KEY_ALL_ACCESS, adminID))
    return GetLastError();

  if (!AddAccessAllowedAce(dacl, ACL_REVISION2, KEY_ALL_ACCESS, systemID))
    return GetLastError();

  if (!AddAccessAllowedAce(dacl, ACL_REVISION2, KEY_ALL_ACCESS, creatorID))
    return GetLastError();

  if (!AddAccessAllowedAce(dacl, ACL_REVISION2, KEY_ALL_ACCESS, userID))
    return GetLastError();

  if (!SetSecurityDescriptorDacl(&secdesc, TRUE, dacl, FALSE))
    return GetLastError();

  SECURITY_ATTRIBUTES secattr;
  secattr.nLength = sizeof(secattr);
  secattr.lpSecurityDescriptor = &secdesc;
  secattr.bInheritHandle = FALSE;

  DWORD disposition;

  return RegCreateKeyEx(rootKey, subkey, 0, (char*) "", REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS, &secattr, &key, &disposition);
}
#endif // _WIN32_WCE

RegistryKey::RegistryKey(const PString & subkeyname, OpenMode mode)
{
  PAssert(!subkeyname.IsEmpty(), PInvalidParameter);

  PProcess & proc = PProcess::Current();
  DWORD access = mode == ReadOnly ? KEY_READ : KEY_ALL_ACCESS;
  DWORD error;

  PString subkey;
  HKEY basekey;
  if (subkeyname.Find(LocalMachineStr) == 0) {
    subkey = subkeyname.Mid(19);
    basekey = HKEY_LOCAL_MACHINE;
  }
  else if (subkeyname.Find(CurrentUserStr) == 0) {
    subkey = subkeyname.Mid(18);
    basekey = HKEY_CURRENT_USER;
  }
  else {
    subkey = subkeyname;
    PINDEX lastCharPos = subkey.GetLength()-1;
    while (lastCharPos > 0 && subkey[lastCharPos] == '\\')
      subkey.Delete(lastCharPos--, 1);
    basekey = NULL;

    if (!proc.GetVersion(FALSE).IsEmpty()) {
      PString keyname = subkey;
      keyname.Replace("CurrentVersion", proc.GetVersion(FALSE));

      error = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, access, &key);
      if (error == ERROR_SUCCESS)
        return;

      PTRACE_IF(1, error == ERROR_ACCESS_DENIED, "PTLib\tAccess denied accessing registry entry HKEY_CURRENT_USER\\" << keyname);

      error = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, access, &key);
      if (error == ERROR_SUCCESS)
        return;

      PTRACE_IF(1, error == ERROR_ACCESS_DENIED, "PTLib\tAccess denied accessing registry entry HKEY_LOCAL_MACHINE\\" << keyname);
    }

    error = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, access, &key);
    if (error == ERROR_SUCCESS)
      return;

    PTRACE_IF(1, error == ERROR_ACCESS_DENIED, "PTLib\tAccess denied accessing registry entry HKEY_CURRENT_USER\\" << subkey);
  }

  error = RegOpenKeyEx(basekey != NULL ? basekey : HKEY_LOCAL_MACHINE,
                       subkey, 0, access, &key);
  if (error == ERROR_SUCCESS)
    return;

  PTRACE_IF(1, error == ERROR_ACCESS_DENIED, "PTLib\tAccess denied accessing registry entry "
            << (basekey != NULL ? "" : LocalMachineStr) << subkey);

  key = NULL;
  if (mode != Create)
    return;

  if (basekey == NULL) {
    if (PProcess::Current().IsServiceProcess())
      basekey = HKEY_LOCAL_MACHINE;
    else
      basekey = HKEY_CURRENT_USER;
  }

#ifndef _WIN32_WCE
  error = SecureCreateKey(basekey, subkey, key);
  if (error != ERROR_SUCCESS) {
#endif

    DWORD disposition;
    error = RegCreateKeyEx(basekey, subkey, 0, (char*) "", REG_OPTION_NON_VOLATILE,
                           KEY_ALL_ACCESS, NULL, &key, &disposition);
    if (error != ERROR_SUCCESS) {
      PTRACE(1, "PTLib\tCould not create registry entry "
             << (basekey != NULL ? "" : LocalMachineStr) << subkey);
      key = NULL;
    }
#ifndef _WIN32_WCE
  }
#endif // _WIN32_WCE
}


RegistryKey::~RegistryKey()
{
  if (key != NULL)
    RegCloseKey(key);
}


BOOL RegistryKey::EnumKey(PINDEX idx, PString & str)
{
  if (key == NULL)
    return FALSE;

#ifndef _WIN32_WCE
  if( RegEnumKey(key, idx, str.GetPointer(MAX_PATH),MAX_PATH) != ERROR_SUCCESS)

#else // CE has only Unicode based API
  USES_CONVERSION;
  TCHAR tstr[MAX_PATH];
  LONG lResult = RegEnumKey(key, idx, tstr, MAX_PATH);
  str = T2A(tstr);
  if( lResult != ERROR_SUCCESS )
#endif
    return FALSE;

  str.MakeMinimumSize();
  return TRUE;
}


BOOL RegistryKey::EnumValue(PINDEX idx, PString & str)
{
  if (key == NULL)
    return FALSE;

  DWORD sizeofname = MAX_PATH;
#ifndef _WIN32_WCE
  if (RegEnumValue(key, idx, str.GetPointer(sizeofname),
                         &sizeofname, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)

#else  // CE has only Unicode based API
  USES_CONVERSION;
  TCHAR tstr[MAX_PATH];
  LONG lResult = RegEnumValueCe(key, idx, tstr,
      &sizeofname, NULL, NULL, NULL, NULL);
  str = T2A(tstr);
  if( lResult != ERROR_SUCCESS )
#endif
    return FALSE;

  str.MakeMinimumSize();
  return TRUE;
}


BOOL RegistryKey::DeleteKey(const PString & subkey)
{
  if (key == NULL)
    return TRUE;

  return RegDeleteKey(key, subkey) == ERROR_SUCCESS;
}


BOOL RegistryKey::DeleteValue(const PString & value)
{
  if (key == NULL)
    return TRUE;

  return RegDeleteValue(key, (char *)(const char *)value) == ERROR_SUCCESS;
}


BOOL RegistryKey::QueryValue(const PString & value, PString & str)
{
  if (key == NULL)
    return FALSE;

  DWORD type, size;
  if (RegQueryValueEx(key, (char *)(const char *)value,
                                    NULL, &type, NULL, &size) != ERROR_SUCCESS)
    return FALSE;

  switch (type) {
    case REG_SZ :
    case REG_MULTI_SZ :
    case REG_EXPAND_SZ :
    case REG_BINARY :
#ifndef _WIN32_WCE
      return RegQueryValueEx(key, (char *)(const char *)value, NULL,
                  &type, (LPBYTE)str.GetPointer(size), &size) == ERROR_SUCCESS;
#else  // CE has only Unicode based API
    {   USES_CONVERSION; TCHAR tstr[MAX_PATH];
      if( RegQueryValueEx(key, (char *)(const char *)value, NULL,
            &type, (LPBYTE) tstr, &size) == ERROR_SUCCESS )
      {
        str = T2A(tstr);
        return TRUE; 
      } 
    }
#endif
    case REG_DWORD : {
      DWORD num;
      size = sizeof(num);
      if (RegQueryValueEx(key, (char *)(const char *)value, NULL,
                                &type, (LPBYTE)&num, &size) == ERROR_SUCCESS) {
        str = PString(PString::Signed, num);
        return TRUE;
      }
    }
    default :
      PAssertAlways("Unsupported registry type.");
  }
  return FALSE;
}


BOOL RegistryKey::QueryValue(const PString & value, DWORD & num, BOOL boolean)
{
  if (key == NULL)
    return FALSE;

  DWORD type, size;
  if (RegQueryValueEx(key, (char *)(const char *)value,
                                    NULL, &type, NULL, &size) != ERROR_SUCCESS)
    return FALSE;

  switch (type) {
    case REG_BINARY :
      if (size > sizeof(DWORD))
        return FALSE;

      num = 0;
      // Do REG_DWORD case

    case REG_DWORD :
      return RegQueryValueEx(key, (char *)(const char *)value, NULL,
                                  &type, (LPBYTE)&num, &size) == ERROR_SUCCESS;

    case REG_SZ : {
      PString str;
      if (RegQueryValueEx(key, (char *)(const char *)value, NULL,
                &type, (LPBYTE)str.GetPointer(size), &size) == ERROR_SUCCESS) {
        num = str.AsInteger();
        if (num == 0 && boolean) {
          int c = toupper(str[0]);
          num = c == 'T' || c == 'Y';
        }
        return TRUE;
      }
      break;
    }
    default :
      PAssertAlways("Unsupported registry type.");
  }

  return FALSE;
}


BOOL RegistryKey::SetValue(const PString & value, const PString & str)
{
  if (key == NULL)
    return FALSE;

#ifndef _WIN32_WCE
  return RegSetValueEx(key, (char *)(const char *)value, 0, REG_SZ,
                (LPBYTE)(const char *)str, str.GetLength()+1) == ERROR_SUCCESS;
#else  // CE has only Unicode based API
  USES_CONVERSION; 
  return RegSetValueEx(key, (char *)(const char *)value, 0, REG_SZ,
                (LPBYTE) A2T((const char *)str), 
          ( (str.GetLength()+1) * sizeof(TCHAR)/sizeof(char) )
            ) == ERROR_SUCCESS;
#endif
}


BOOL RegistryKey::SetValue(const PString & value, DWORD num)
{
  if (key == NULL)
    return FALSE;

  return RegSetValueEx(key, (char *)(const char *)value,
                     0, REG_DWORD, (LPBYTE)&num, sizeof(num)) == ERROR_SUCCESS;
}


static BOOL IsRegistryPath(const PString & path)
{
  BOOL isRP=(!path.IsEmpty());
  if(isRP) isRP = ((path.Find(LocalMachineStr) == 0) && (path != LocalMachineStr))
               || ((path.Find(CurrentUserStr)  == 0) && (path != CurrentUserStr ));

  return isRP;
}


///////////////////////////////////////////////////////////////////////////////

PString PProcess::GetConfigurationFile()
{
  PString getCF;

  // No paths set, use defaults
  if (configurationPaths.IsEmpty()) {
    configurationPaths.AppendString(executableFile.GetVolume() + executableFile.GetPath());
    configurationPaths.AppendString(CurrentUserStr);
    PTRACE(3,"WinCfg\tGetConfigurationFile() warning: paths were empty and now filled by default: " << configurationPaths);
  }

  // See if explicit filename
  if (configurationPaths.GetSize() == 1 && !PDirectory::Exists(configurationPaths[0])) getCF = configurationPaths[0];
  else
  {
    PString iniFilename = executableFile.GetTitle() + ".ini";
    for (PINDEX i = 0; i < configurationPaths.GetSize(); i++) {
      PString path = configurationPaths[i];
      if (IsRegistryPath(path)) { getCF=path; break; }
      PFilePath cfgFile = PDirectory(path) + iniFilename;
      if (PFile::Exists(cfgFile)) { getCF=cfgFile; break; }
    }
  }

  return getCF;
}


///////////////////////////////////////////////////////////////////////////////

void PConfig::Construct(Source src, const PString & appname, const PString & manuf)
{
  source = Application;

  switch (src) {
    case System :
      if (IsRegistryPath(appname))
        location = appname;
      else {
        PString dir;
        GetWindowsDirectory(dir.GetPointer(_MAX_PATH), _MAX_PATH);
        Construct(PDirectory(dir)+"WIN.INI");
      }
      break;

    case Application :
      if (IsRegistryPath(defaultSection))
        location = PString();
      else {
        PProcess & proc = PProcess::Current();
        PString cfgPath = proc.GetConfigurationFile();
        if (IsRegistryPath(cfgPath))
          location = cfgPath;
        else if (!cfgPath) {
          source = NumSources; // Make a file based config
          location = cfgPath;
        }
        else {
          location = "SOFTWARE\\";
          if (!manuf)
            location += manuf;
          else if (!proc.GetManufacturer())
            location += proc.GetManufacturer();
          else
            location += "PWLib";
          location += PDIR_SEPARATOR;
          if (appname.IsEmpty())
            location += proc.GetName();
          else
            location += appname;
          location += "\\CurrentVersion\\";
        }
      }
      break;

    default :
      source = src;
  }
}


void PConfig::Construct(const PFilePath & filename)
{
  location = filename;
  source = NumSources;
}


static void RecurseRegistryKeys(const PString & location,
                                PINDEX baseLength,
                                PStringList &sections)
{
  RegistryKey registry(location, RegistryKey::ReadOnly);
  PString name;
  for (PINDEX idx = 0; registry.EnumKey(idx, name); idx++) {
    RecurseRegistryKeys(location + name + '\\', baseLength, sections);
    sections.AppendString(location.Mid(baseLength) + name);
  }
}


static PString PGetPrivateProfileString(const char * lpAppName,
                                        const char * lpKeyName,
                                        const char * lpDefault,
                                        const char * lpFileName)
{
  PString buffer;

  DWORD numNulls = lpAppName != NULL && lpKeyName != NULL ? 1 : 2;
  DWORD size = 1000;
  while (size <= 100000 &&
                ::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault,
                                          buffer.GetPointer(size+numNulls), size+numNulls,
                                          lpFileName) == size)
    size *= 10;

  return buffer;
}


PStringList PConfig::GetSections() const
{
  PStringList sections;

  switch (source) {
    case Application :
      RecurseRegistryKeys(location, location.GetLength(), sections);
      break;

    case NumSources :
      PString buffer = PGetPrivateProfileString(NULL, NULL, "", location);
      char * ptr = buffer.GetPointer();
      while (*ptr != '\0') {
        sections.AppendString(ptr);
        ptr += strlen(ptr)+1;
      }
      break;
  }

  return sections;
}


PStringList PConfig::GetKeys(const PString & section) const
{
  PStringList keys;

  switch (source) {
    case Environment : {
      char ** ptr = _environ;
      while (*ptr != NULL) {
        PString buf = *ptr++;
        keys.AppendString(buf.Left(buf.Find('=')));
      }
      break;
    }

    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location + section, RegistryKey::ReadOnly);
      PString name;
      for (PINDEX idx = 0; registry.EnumValue(idx, name); idx++)
        keys.AppendString(name);
      break;
    }

    case NumSources :
      PAssert(!section.IsEmpty(), PInvalidParameter);
      PString buffer = PGetPrivateProfileString(section, NULL, "", location);
      char * ptr = buffer.GetPointer();
      while (*ptr != '\0') {
        keys.AppendString(ptr);
        ptr += strlen(ptr)+1;
      }
  }

  return keys;
}


void PConfig::DeleteSection(const PString & section)
{
  switch (source) {
    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location, RegistryKey::ReadWrite);
      registry.DeleteKey(section);
      break;
    }

    case NumSources :
      PAssert(!section.IsEmpty(), PInvalidParameter);
      PAssertOS(WritePrivateProfileString(section, NULL, NULL, location));
  }
}


void PConfig::DeleteKey(const PString & section, const PString & key)
{
  switch (source) {
    case Environment :
      PAssert(!key.IsEmpty() && key.Find('=') == P_MAX_INDEX, PInvalidParameter);
      _putenv(key + "=");
      break;

    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location + section, RegistryKey::ReadWrite);
      registry.DeleteValue(key);
      break;
    }

    case NumSources :
      PAssert(!key.IsEmpty(), PInvalidParameter);
      PAssert(!section.IsEmpty(), PInvalidParameter);
      PAssertOS(WritePrivateProfileString(section, key, NULL, location));
  }
}


BOOL PConfig::HasKey(const PString & section, const PString & key) const
{
  switch (source) {
    case Environment :
      PAssert(!key.IsEmpty(), PInvalidParameter);
      return getenv(key) != NULL;

    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location + section, RegistryKey::ReadOnly);
      PString dummy;
      return registry.QueryValue(key, dummy);
    }

    case NumSources :
      PAssert(!key.IsEmpty() && !section.IsEmpty(), PInvalidParameter);
      static const char dflt[] = "<<<<<====---PConfig::DefaultValueString---====>>>>>";
      return PGetPrivateProfileString(section, key, dflt, location) != dflt;
  }

  return FALSE;
}


PString PConfig::GetString(const PString & section,
                               const PString & key, const PString & dflt) const
{
  PString str;

  switch (source) {
    case Environment : {
      PAssert(!key.IsEmpty() && key.Find('=') == P_MAX_INDEX, PInvalidParameter);
      char * env = getenv(key);
      if (env != NULL)
        str = env;
      else
        str = dflt;
      break;
    }

    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location + section, RegistryKey::ReadOnly);
      if (!registry.QueryValue(key, str))
        str = dflt;
      break;
    }

    case NumSources :
      PAssert(!key.IsEmpty() && !section.IsEmpty(), PInvalidParameter);
      str = PGetPrivateProfileString(section, key, dflt, location);
      str.MakeMinimumSize();
  }

  return str;
}


void PConfig::SetString(const PString & section,
                                    const PString & key, const PString & value)
{
  switch (source) {
    case Environment :
      PAssert(!key.IsEmpty() && key.Find('=') == P_MAX_INDEX, PInvalidParameter);
      _putenv(key + "=" + value);
      break;

    case Application : {
      PAssert(!section.IsEmpty(), PInvalidParameter);
      RegistryKey registry(location + section, RegistryKey::Create);
      registry.SetValue(key, value);
      break;
    }

    case NumSources :
      PAssert(!key.IsEmpty() && !section.IsEmpty(), PInvalidParameter);
      PAssertOS(WritePrivateProfileString(section, key, value, location));
  }
}


BOOL PConfig::GetBoolean(const PString & section,
                                          const PString & key, BOOL dflt) const
{
  if (source != Application) {
    PString str = GetString(section, key, dflt ? "T" : "F").ToUpper();
    int c = toupper(str[0]);
    return c == 'T' || c == 'Y' || str.AsInteger() != 0;
  }

  PAssert(!section.IsEmpty(), PInvalidParameter);
  RegistryKey registry(location + section, RegistryKey::ReadOnly);

  DWORD value;
  if (!registry.QueryValue(key, value, TRUE))
    return dflt;

  return value != 0;
}


void PConfig::SetBoolean(const PString & section, const PString & key, BOOL value)
{
  if (source != Application)
    SetString(section, key, value ? "True" : "False");
  else {
    PAssert(!section.IsEmpty(), PInvalidParameter);
    RegistryKey registry(location + section, RegistryKey::Create);
    registry.SetValue(key, value ? 1 : 0);
  }
}


long PConfig::GetInteger(const PString & section,
                                          const PString & key, long dflt) const
{
  if (source != Application) {
    PString str(PString::Signed, dflt);
    return GetString(section, key, str).AsInteger();
  }

  PAssert(!section.IsEmpty(), PInvalidParameter);
  RegistryKey registry(location + section, RegistryKey::ReadOnly);

  DWORD value;
  if (!registry.QueryValue(key, value, FALSE))
    return dflt;

  return value;
}


void PConfig::SetInteger(const PString & section, const PString & key, long value)
{
  if (source != Application) {
    PString str(PString::Signed, value);
    SetString(section, key, str);
  }
  else {
    PAssert(!section.IsEmpty(), PInvalidParameter);
    RegistryKey registry(location + section, RegistryKey::Create);
    registry.SetValue(key, value);
  }
}


// End Of File ///////////////////////////////////////////////////////////////
