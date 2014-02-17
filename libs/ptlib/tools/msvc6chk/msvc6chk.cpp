// msvc6chk.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <windows.h>

#include "iostream.h"
#include "io.h"
#include "fcntl.h"

#include <string>

#define VERSION   "1.0"


static char * dinkumwareCopyright = 
"The files presented here are copyright © 1995-2000 by P.J. Plauger.\n"
"All rights reserved. They are for use only in conjunction with a valid\n"
"license for Microsoft Visual C++ V5.0 or V6.0. Microsoft Corporation is in\n"
"no way involved with the production or release of these files. The files are\n"
"offered on an `as is' basis. DINKUMWARE, LTD. AND P.J. PLAUGER MAKE NO\n"
"REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THESE FILES, EITHER\n"
"EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF\n"
"MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.\n"
"DINKUMWARE, LTD. AND P.J. PLAUGER SHALL NOT BE LIABLE FOR ANY DAMAGES\n"
"SUFFERED BY LICENSEE AS A RESULT OF USING THESE FILES.";



static struct FileUpgradeInfo {
  HKEY key;
  const char * registryBase;
  const char * registryKey;
  const char * filename;
  unsigned int oldcrc;

  const char * newFilename;
  unsigned int newcrc;

  char * copyright;
} fileInfo[] = {
  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\DEQUE",      0x4229bfd5,
    "UPGRADES/DEQUE",      0x11539af7,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\FSTREAM",      0x28f76291,
    "UPGRADES/FSTREAM",      0xb9a507b4,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\LIST",      0x64487361,
    "UPGRADES/LIST",      0xa97cc61b,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\VECTOR",      0x26ba0df5,
    "UPGRADES/VECTOR",      0x35a94c52,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\XMEMORY",      0xa6ac2fb8,
    "UPGRADES/XMEMORY",      0x52936ff7,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\XSTRING",      0x705ce397,
    "UPGRADES/XSTRING",      0x6215f086,
    dinkumwareCopyright
  },

  { HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\VisualStudio\\6.0\\Setup\\Microsoft Visual C++",
    "ProductDir",
    "INCLUDE\\XTREE",      0xe64c6f6e,
    "UPGRADES/XTREE",      0x07972076,
    dinkumwareCopyright
  },

  { NULL }
};

static unsigned long crc_table[256];

void gen_crc32_table(void)                /* build the crc table */
{
  unsigned long crc, poly;
  int	i, j;

  poly = 0xEDB88320L;
  for (i = 0; i < 256; i++) {
    crc = i;
    for (j = 8; j > 0; j--) {
      if (crc & 1)
        crc = (crc >> 1) ^ poly;
      else
        crc >>= 1;
    }
    crc_table[i] = crc;
  }
}

void init_crc32(unsigned long & crc)
{
  crc = 0xFFFFFFFF;
}


void add_to_crc32(unsigned long & crc, unsigned char value)
{
  crc = (crc >> 8) ^ crc_table[(crc^value) & 0xFF];
  crc = crc ^ 0xFFFFFFFF;
}

bool CalculateCRCOfFile(const std::string & fn, unsigned long & crc32)
{
  int file = ::open(fn.c_str(), O_RDONLY | O_BINARY);
  if (file < 0)
    return false;

  init_crc32(crc32);
  char ch;
  while (read(file, &ch, 1)) 
    add_to_crc32(crc32, ch);
  ::close(file);
  return true;
}


class RegKey
{
public:
  RegKey()
  {
    key = NULL;
  }
  ~RegKey()
  {
    if (key != NULL)
      RegCloseKey(key);
  }

  HKEY key;
};


int main(int argc, char * argv[])
{
  cout << "PWLIB File Upgrader v" << VERSION << "\n"
       << "Copyright (C) 2004 by Post Increment" << endl;

  gen_crc32_table();

  bool testFiles = false;
  bool doUpgrade = false;
  bool force     = false;

  if (argc > 1) {
    if (strcmpi(argv[1], "test") == 0)
      testFiles = true;
    else if (strcmpi(argv[1], "upgrade") == 0) 
      doUpgrade = true;
    else if (strcmpi(argv[1], "force") == 0)  {
      doUpgrade = true;
      force     = true;
    }
  }

  int status = 0;

  unsigned i;
  for (i = 0; fileInfo[i].filename != NULL; ++i) {
    FileUpgradeInfo & info = fileInfo[i];

    char * infoKeyName = "(unknown)";
    if (info.key == HKEY_CLASSES_ROOT)
      infoKeyName = "HKEY_CLASSES_ROOT";

    else if (info.key == HKEY_CURRENT_USER)
      infoKeyName = "HKEY_CURRENT_USER";

    else if (info.key == HKEY_LOCAL_MACHINE)
      infoKeyName = "HKEY_LOCAL_MACHINE";

    else if (info.key == HKEY_USERS)
      infoKeyName = "HKEY_USERS";

    // open registry key
    RegKey key;
    LONG stat = RegOpenKeyEx(info.key,
                             info.registryBase,
                             0, KEY_READ,
                             &key.key);
    if (stat != ERROR_SUCCESS) {
      cout << "Registry entry " << infoKeyName << "\\" << info.registryBase << " not found - error = " << stat << endl;
      continue;
    }

    DWORD keyType;
    char keyData[4096];
    DWORD keyLen = sizeof(keyData);
    if (RegQueryValueEx(key.key, 
                        info.registryKey,
                        NULL,
                        &keyType,
                        (BYTE *)keyData, &keyLen) != ERROR_SUCCESS) {
      cout << "Registry key " << infoKeyName << "\\" << info.registryBase << "\\" << info.registryKey << " not found" << endl;
      continue;
    }

    std::string fn(keyData);
    fn += '\\';
    fn += info.filename;

    unsigned long crc32;
    if (!CalculateCRCOfFile(fn, crc32)) {
      cout << fn.c_str() << "does not exist" << endl;
      continue;
    }

    if (testFiles) {
      unsigned long newcrc;
      if (!CalculateCRCOfFile(info.newFilename, newcrc))
        cout << info.newFilename << "does not exist" << endl;
      else {
        cout << hex << crc32  << dec << " = " << fn.c_str() << "\n"
             << hex << newcrc << dec << " = " << info.newFilename << "\n"
             << endl;
      }
    } 
    
    else if (crc32 == info.newcrc) {
      cout << fn.c_str() << " already upgraded" << endl;
      continue;
    }

    if (crc32 != info.oldcrc) {
      cout << "WARNING: " << fn.c_str() << " has unknown CRC " << hex << crc32 << dec
           << ", assuming it does NOT need upgrade." << endl;
      continue;
    }

    // create name of backup file
    std::string backupFn = fn + ".backup";
    char buffer[4];
    int count = 1;
    for (;;) {
      if (::access(backupFn.c_str(), 0) != 0)
        break;
      sprintf(buffer, "_%i.backup", count++);
      backupFn = fn + std::string(buffer);
    }

    if (!doUpgrade) {
      cout << "\nWARNING: the following file requires upgrading:\n"
           << fn.c_str() << endl;
      std::string cmd = argv[0];
      int pos = cmd.rfind('\\');
      if (pos != std::string::npos)
        cmd.erase(0, pos+1);
      pos = cmd.rfind('.');
      if (pos != std::string::npos)
        cmd.erase(pos);
      cout << "Please run \"" << cmd.c_str() << " upgrade\" to upgrade this file" << endl;
      status = 1;
      continue;
    } 
    
    cout << "\nWARNING: the following file requires upgrading:\n"
         << fn.c_str() << endl;

    if (info.copyright != NULL)
      cout << "\n" << info.copyright << "\n" << endl;

    if (!force) {
      cout << "Upgrade file (y/n) ? " << flush;
      char ch;
      cin >> ch;
      ch = toupper(ch);
      if (ch != 'Y') {
        cout << "Aborting upgrade." << endl;
        continue;
      }
    }

    // check that replacement file exists
    if (::access(info.newFilename, 0) != 0) {
      cout << "ERROR: new file " << info.newFilename << "not found - aborting upgrade" << endl;
      status = 1;
      continue;
    }

    // rename existing file
    if (::rename(fn.c_str(), backupFn.c_str()) != 0) {
      cout << "ERROR: rename of " << fn.c_str() << " to " << backupFn.c_str() << " failed" << endl;
      status = 1;
      continue;
    }

    // copy new file
    if (!CopyFile(info.newFilename, fn.c_str(), true)) {
      cout << "ERROR: copy of " << info.newFilename << " to " << fn.c_str() << " failed\n"
           << "       reverting " << backupFn.c_str() << " to " << fn.c_str() << endl;
      ::unlink(fn.c_str());
      ::rename(backupFn.c_str(), fn.c_str());
      status = 1;
      continue;
    }

    // upgrade done
    cout << "The following files have been upgraded\n"
         << "  " << fn.c_str() << "\n"
         << "The previous version of the file has been renamed to\n"
         << "  " << backupFn.c_str() << endl;
  }

  return status;
}

