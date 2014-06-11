/*
 * Common Plugin code for OpenH323/OPAL
 *
 * This code is based on the following files from the OPAL project which
 * have been removed from the current build and distributions but are still
 * available in the CVS "attic"
 * 
 *    src/codecs/h263codec.cxx 
 *    include/codecs/h263codec.h 

 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2006 Post Increment
 * Copyright (C) 2005 Salyens
 * Copyright (C) 2001 March Networks Corporation
 * Copyright (C) 1999-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *                 Matthias Schneider (ma30002000@yahoo.de)
 */

#include "dyna.h"


bool DynaLink::Open(const char *name)
{
  // At first we try without a path
  if (InternalOpen("", name))
    return true;

  // Try the current directory
  char ptlibPath[1024] = ".";
  // try directories specified in PTLIBPLUGINDIR
  char * env = ::getenv("PTLIBPLUGINDIR");
  if (env != NULL && *env != '\0') {
    strcat(ptlibPath, DIR_TOKENISER);
    strcat(ptlibPath, env);
  }
  else {
#ifdef OPAL_PLUGIN_DIR
    strcat(ptlibPath, DIR_TOKENISER OPAL_PLUGIN_DIR);
#elif _WIN32
    strcat(ptlibPath, DIR_TOKENISER "C:\\PTLib_Plugins");
#endif
  }
  strcat(ptlibPath, DIR_TOKENISER "/usr/local/lib:/opt/local/lib");

  char * p = ::strtok(ptlibPath, DIR_TOKENISER);
  while (p != NULL) {
    if (InternalOpen(p, name))
      return true;
    p = ::strtok(NULL, DIR_TOKENISER);
  }

  return false;
}

bool DynaLink::InternalOpen(const char * dir, const char *name)
{
  char path[1024];
  memset(path, 0, sizeof(path));

  // Copy the directory to "path" and add a separator if necessary
  if (strlen(dir) > 0) {
    strcpy(path, dir);
    if (path[strlen(path)-1] != DIR_SEPARATOR[0]) 
      strcat(path, DIR_SEPARATOR);
  }
  strcat(path, name);

  if (strlen(path) == 0) {
    PTRACE(1, "DynaLink", "dir '" << (dir != NULL ? dir : "(NULL)")
           << "', name '" << (name != NULL ? name : "(NULL)") << "' resulted in empty path");
    return false;
  }

  // Load the Libary
#ifdef _WIN32
# ifdef UNICODE
  // must be called before using avcodec lib
  USES_CONVERSION;
  m_hDLL = LoadLibraryEx(A2T(path), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
# else
  // must be called before using avcodec lib
  m_hDLL = LoadLibraryEx(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
# endif /* UNICODE */
#else
  // must be called before using avcodec lib
  m_hDLL = dlopen((const char *)path, RTLD_NOW);
#endif /* _WIN32 */

  // Check for errors
  if (m_hDLL == NULL) {
#ifndef _WIN32
    const char * err = dlerror();
    if (err != NULL)
      PTRACE(3, "DynaLink", "dlopen error " << err);
    else
      PTRACE(3, "DynaLink", "dlopen error loading " << path);
#else /* _WIN32 */
    PTRACE(3, "DynaLink", "Error loading " << path);
#endif /* _WIN32 */
    return false;
  } 

#ifdef _WIN32
  GetModuleFileName(m_hDLL, path, sizeof(path));
#endif

  PTRACE(1, "DynaLink", "Successfully loaded '" << path << "'");
  return true;
}

void DynaLink::Close()
{
  if (m_hDLL != NULL) {
#ifdef _WIN32
    FreeLibrary(m_hDLL);
#else
    dlclose(m_hDLL);
#endif /* _WIN32 */
    m_hDLL = NULL;
  }
}

bool DynaLink::GetFunction(const char * name, Function & func)
{
  if (m_hDLL == NULL)
    return false;

#ifdef _WIN32

  #ifdef UNICODE
    USES_CONVERSION;
    FARPROC p = GetProcAddress(m_hDLL, A2T(name));
  #else
    FARPROC p = GetProcAddress(m_hDLL, name);
  #endif /* UNICODE */
  if (p == NULL) {
    PTRACE(1, "DynaLink", "Error linking function " << name << ", error=" << GetLastError());
    return false;
  }

  func = (Function)p;

#else // _WIN32

  void * p = dlsym(m_hDLL, (const char *)name);
  if (p == NULL) {
    PTRACE(1, "DynaLink", "Error linking function " << name << ", error=" << dlerror());
    return false;
  }
  func = (Function &)p;

#endif // _WIN32

  return true;
}
