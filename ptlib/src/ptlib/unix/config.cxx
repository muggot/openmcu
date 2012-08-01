/*
 * config.cxx
 *
 * System/application configuration class implementation
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
 * $Log: config.cxx,v $
 * Revision 1.37  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.36  2004/04/03 23:55:59  csoutheren
 * Added fix for PConfig environment variables under Linux
 *   Thanks to Michal Zygmuntowicz
 *
 * Revision 1.35  2003/03/17 08:10:59  robertj
 * Fixed bug with parsing lines with no equal sign
 *
 * Revision 1.34  2003/01/30 23:46:05  dereks
 * Fix compile error on gcc 3.2
 *
 * Revision 1.33  2003/01/26 03:57:12  robertj
 * Fixed problem with last change so can still operate if do not have write
 *   access to the directory config file is in.
 * Improved error reporting.
 *
 * Revision 1.32  2003/01/24 12:12:20  robertj
 * Changed so that a crash in some other thread can no longer cause the
 *   config file to be truncated to zero bytes.
 *
 * Revision 1.31  2001/09/14 07:36:27  robertj
 * Fixed bug where fails to read last line of config file if not ended with '\n'.
 *
 * Revision 1.30  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.29  2001/05/24 00:56:38  robertj
 * Fixed problem with config file being written every time on exit. Now is
 *   only written if it the config was modified by the application.
 *
 * Revision 1.28  2001/03/10 04:15:29  robertj
 * Incorrect case for .ini extension
 *
 * Revision 1.27  2001/03/09 06:31:22  robertj
 * Added ability to set default PConfig file or path to find it.
 *
 * Revision 1.26  2000/10/19 04:17:04  craigs
 * Changed to allow writing of config files whilst config file is open
 *
 * Revision 1.25  2000/10/02 20:58:06  robertj
 * Fixed bug where subsequent config file opening uses first opened filename.
 *
 * Revision 1.24  2000/08/30 04:45:02  craigs
 * Added ability to have multiple lines with the same key
 *
 * Revision 1.23  2000/08/16 04:21:27  robertj
 * Fixed subtle difference between UNix and Win32 section names (ignore trailing backslash)
 *
 * Revision 1.22  2000/05/25 12:10:06  robertj
 * Added PConfig::HasKey() function to determine if value actually set.
 *
 * Revision 1.21  2000/05/02 08:30:26  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.20  1998/12/16 12:40:41  robertj
 * Fixed bug where .ini file is not written when service run as a daemon.
 *
 * Revision 1.19  1998/12/16 09:57:37  robertj
 * Fixed bug in writing .ini file, not truncating file when shrinking.
 *
 * Revision 1.18  1998/11/30 21:51:41  robertj
 * New directory structure.
 *
 * Revision 1.17  1998/11/03 02:30:38  robertj
 * Fixed emeory leak of environment.
 *
 * Revision 1.16  1998/09/24 04:12:11  robertj
 * Added open software license.
 *
 */

#define _CONFIG_CXX

#pragma implementation "config.h"

#include <ptlib.h>
#include <ptlib/pprocess.h>

#include "../common/pconfig.cxx"


#define SYS_CONFIG_NAME		"pwlib"

#define	APP_CONFIG_DIR		".pwlib_config/"
#define	SYS_CONFIG_DIR		"/usr/local/pwlib/"

#define	EXTENSION		".ini"
#define	ENVIRONMENT_CONFIG_STR	"/\~~environment~~\/"

//
//  a single key/value pair
//
PDECLARE_CLASS (PXConfigValue, PCaselessString)
  public:
    PXConfigValue(const PString & theKey, const PString & theValue = "") 
      : PCaselessString(theKey), value(theValue) { }
    PString GetValue() const { return value; }
    void    SetValue(const PString & theValue) { value = theValue; }

  protected:
    PString  value;
};

//
//  a list of key/value pairs
//
PLIST (PXConfigSectionList, PXConfigValue);

//
//  a list of key value pairs, with a section name
//
PDECLARE_CLASS(PXConfigSection, PCaselessString)
  public:
    PXConfigSection(const PCaselessString & theName) 
      : PCaselessString(theName) { list.AllowDeleteObjects(); }

    PXConfigSectionList & GetList() { return list; }

  protected:
    PXConfigSectionList list;
};

//
// a list of sections
//

PDECLARE_LIST(PXConfig, PXConfigSection)
  public:
    PXConfig(int i = 0);

    void Wait()   { mutex.Wait(); }
    void Signal() { mutex.Signal(); }

    BOOL ReadFromFile (const PFilePath & filename);
    void ReadFromEnvironment (char **envp);

    BOOL WriteToFile(const PFilePath & filename);
    BOOL Flush(const PFilePath & filename);

    void SetDirty()   { dirty = TRUE; }

    BOOL      AddInstance();
    BOOL      RemoveInstance(const PFilePath & filename);

    PINDEX    GetSectionsIndex(const PString & theSection) const;

  protected:
    int       instanceCount;
    PMutex    mutex;
    BOOL      dirty;
    BOOL      canSave;
};

//
// a dictionary of configurations, keyed by filename
//
PDECLARE_DICTIONARY(PXConfigDictionary, PFilePath, PXConfig)
  public:
    PXConfigDictionary(int dummy);
    ~PXConfigDictionary();
    PXConfig * GetFileConfigInstance(const PFilePath & key, const PFilePath & readKey);
    PXConfig * GetEnvironmentInstance();
    void RemoveInstance(PXConfig * instance);
    void WriteChangedInstances();

  protected:
    PMutex        mutex;
    PXConfig    * environmentInstance;
    PThread     * writeThread;
    PSyncPointAck stopConfigWriteThread;
};


PDECLARE_CLASS(PXConfigWriteThread, PThread)
  public:
    PXConfigWriteThread(PSyncPointAck & stop);
    ~PXConfigWriteThread();
    void Main();
  private:
    PSyncPointAck & stop;
};


PXConfigDictionary * configDict;

#define	new PNEW

//////////////////////////////////////////////////////

void PProcess::CreateConfigFilesDictionary()
{
  configFiles = new PXConfigDictionary(0);
}


PXConfigWriteThread::PXConfigWriteThread(PSyncPointAck & s)
  : PThread(10000, NoAutoDeleteThread, NormalPriority, "PXConfigWriteThread"),
    stop(s)
{
  Resume();
}

PXConfigWriteThread::~PXConfigWriteThread()
{
}

void PXConfigWriteThread::Main()
{
  while (!stop.Wait(30000))  // if stop.Wait() returns TRUE, we are shutting down
    configDict->WriteChangedInstances();   // check dictionary for items that need writing

  configDict->WriteChangedInstances();

  stop.Acknowledge();
}



PXConfig::PXConfig(int)
{
  // make sure content gets removed
  AllowDeleteObjects();

  // no instances, initially
  instanceCount = 0;

  // we start off clean
  dirty = FALSE;

  // normally save on exit (except for environment configs)
  canSave = TRUE;
}

BOOL PXConfig::AddInstance()
{
  mutex.Wait();
  BOOL stat = instanceCount++ == 0;
  mutex.Signal();

  return stat;
}

BOOL PXConfig::RemoveInstance(const PFilePath & /*filename*/)
{
  mutex.Wait();

  PAssert(instanceCount != 0, "PConfig instance count dec past zero");

  BOOL stat = --instanceCount == 0;

  mutex.Signal();

  return stat;
}

BOOL PXConfig::Flush(const PFilePath & filename)
{
  mutex.Wait();

  BOOL stat = instanceCount == 0;

  if (canSave && dirty) {
    WriteToFile(filename);
    dirty = FALSE;
  }

  mutex.Signal();

  return stat;
}

BOOL PXConfig::WriteToFile(const PFilePath & filename)
{
  // make sure the directory that the file is to be written into exists
  PDirectory dir = filename.GetDirectory();
  if (!dir.Exists() && !dir.Create( 
                                   PFileInfo::UserExecute |
                                   PFileInfo::UserWrite |
                                   PFileInfo::UserRead)) {
    PProcess::PXShowSystemWarning(2000, "Cannot create PWLIB config directory");
    return FALSE;
  }

  PTextFile file;
  if (!file.Open(filename + ".new", PFile::WriteOnly))
    file.Open(filename, PFile::WriteOnly);

  if (!file.IsOpen()) {
    PProcess::PXShowSystemWarning(2001, "Cannot create PWLIB config file: " + file.GetErrorText());
    return FALSE;
  }

  for (PINDEX i = 0; i < GetSize(); i++) {
    PXConfigSectionList & section = (*this)[i].GetList();
    file << "[" << (*this)[i] << "]" << endl;
    for (PINDEX j = 0; j < section.GetSize(); j++) {
      PXConfigValue & value = section[j];
      PStringArray lines = value.GetValue().Tokenise('\n', TRUE);
      PINDEX k;
      for (k = 0; k < lines.GetSize(); k++) 
        file << value << "=" << lines[k] << endl;
    }
    file << endl;
  }

  file.flush();
  file.SetLength(file.GetPosition());
  file.Close();

  if (file.GetFilePath() != filename) {
    if (!file.Rename(file.GetFilePath(), filename.GetFileName(), TRUE)) {
      PProcess::PXShowSystemWarning(2001, "Cannot rename config file: " + file.GetErrorText());
      return FALSE;
    }
  }

  PTRACE(4, "PWLib\tSaved config file: " << filename);
  return TRUE;
}


BOOL PXConfig::ReadFromFile(const PFilePath & filename)
{
  PINDEX len;

  // clear out all information
  RemoveAll();

  // attempt to open file
  PTextFile file;
  if (!file.Open(filename, PFile::ReadOnly))
    return FALSE;

  PXConfigSection * currentSection = NULL;

  // read lines in the file
  while (file.good()) {
    PString line;
    file >> line;
    line = line.Trim();
    if ((len = line.GetLength()) > 0) {

      // ignore comments and blank lines 
      char ch = line[0];
      if ((len > 0) && (ch != ';') && (ch != '#')) {
        if (ch == '[') {
          PCaselessString sectionName = (line.Mid(1,len-(line[len-1]==']'?2:1))).Trim();
          PINDEX  index;
          if ((index = GetValuesIndex(sectionName)) != P_MAX_INDEX)
            currentSection = &(*this )[index];
          else {
            currentSection = new PXConfigSection(sectionName);
            Append(currentSection);
          }
        } else if (currentSection != NULL) {
          PINDEX equals = line.Find('=');
          if (equals > 0 && equals != P_MAX_INDEX) {
            PString keyStr = line.Left(equals).Trim();
            PString valStr = line.Right(len - equals - 1).Trim();

            PINDEX index;
            if ((index = currentSection->GetList().GetValuesIndex(keyStr)) != P_MAX_INDEX)  {
              PXConfigValue & value = currentSection->GetList()[index];
              value.SetValue(value.GetValue() + '\n' + valStr);
            } else {
              PXConfigValue * value = new PXConfigValue(keyStr, valStr);
              currentSection->GetList().Append(value);
            }
          }
        }
      }
    }
  }
  
  // close the file and return
  file.Close();
  return TRUE;
}

void PXConfig::ReadFromEnvironment (char **envp)
{
  // clear out all information
  RemoveAll();

  PXConfigSection * currentSection = new PXConfigSection("Options");
  Append(currentSection);

  while (*envp != NULL && **envp != '\0') {
    PString line(*envp);
    PINDEX equals = line.Find('=');
    if (equals > 0) {
      PXConfigValue * value = new PXConfigValue(line.Left(equals), line.Right(line.GetLength() - equals - 1));
      currentSection->GetList().Append(value);
    }
    envp++;
  }

  // can't save environment configs
  canSave = FALSE;
}

PINDEX PXConfig::GetSectionsIndex(const PString & theSection) const
{
  PINDEX len = theSection.GetLength()-1;
  if (theSection[len] != '\\')
    return GetValuesIndex(theSection);
  else
    return GetValuesIndex(theSection.Left(len));
}


static BOOL LocateFile(const PString & baseName,
                       PFilePath & readFilename,
                       PFilePath & filename)
{
  // check the user's home directory first
  filename = readFilename = PProcess::Current().GetConfigurationFile();
  if (PFile::Exists(filename))
    return TRUE;

  // otherwise check the system directory for a file to read,
  // and then create 
  readFilename = SYS_CONFIG_DIR + baseName + EXTENSION;
  return PFile::Exists(readFilename);
}

///////////////////////////////////////////////////////////////////////////////

PString PProcess::GetConfigurationFile()
{
  if (configurationPaths.IsEmpty()) {
    configurationPaths.AppendString(PXGetHomeDir() + APP_CONFIG_DIR);
    configurationPaths.AppendString(SYS_CONFIG_DIR);
  }

  // See if explicit filename
  if (configurationPaths.GetSize() == 1 && !PDirectory::Exists(configurationPaths[0]))
    return configurationPaths[0];

  PString iniFilename = executableFile.GetTitle() + ".ini";

  for (PINDEX i = 0; i < configurationPaths.GetSize(); i++) {
    PFilePath cfgFile = PDirectory(configurationPaths[i]) + iniFilename;
    if (PFile::Exists(cfgFile))
      return cfgFile;
  }

  return PDirectory(configurationPaths[0]) + iniFilename;
}


////////////////////////////////////////////////////////////
//
// PXConfigDictionary
//

PXConfigDictionary::PXConfigDictionary(int)
{
  environmentInstance = NULL;
  writeThread = NULL;
  configDict = this;
}


PXConfigDictionary::~PXConfigDictionary()
{
  if (writeThread != NULL) {
    stopConfigWriteThread.Signal();
    writeThread->WaitForTermination();
    delete writeThread;
  }
  delete environmentInstance;
}


PXConfig * PXConfigDictionary::GetEnvironmentInstance()
{
  mutex.Wait();
  if (environmentInstance == NULL) {
    environmentInstance = new PXConfig(0);
    environmentInstance->ReadFromEnvironment(PProcess::Current().PXGetEnvp());
  }
  mutex.Signal();
  return environmentInstance;
}


PXConfig * PXConfigDictionary::GetFileConfigInstance(const PFilePath & key, const PFilePath & readKey)
{
  mutex.Wait();

  // start write thread, if not already started
  if (writeThread == NULL)
    writeThread = new PXConfigWriteThread(stopConfigWriteThread);

  PXConfig * config = GetAt(key);
  if (config != NULL) 
    config->AddInstance();
  else {
    config = new PXConfig(0);
    config->ReadFromFile(readKey);
    config->AddInstance();
    SetAt(key, config);
  }

  mutex.Signal();
  return config;
}

void PXConfigDictionary::RemoveInstance(PXConfig * instance)
{
  mutex.Wait();

  if (instance != environmentInstance) {
    PINDEX index = GetObjectsIndex(instance);
    PAssert(index != P_MAX_INDEX, "Cannot find PXConfig instance to remove");

    // decrement the instance count, but don't remove it yet
    PFilePath key = GetKeyAt(index);
    instance->RemoveInstance(key);
  }

  mutex.Signal();
}

void PXConfigDictionary::WriteChangedInstances()
{
  mutex.Wait();

  PINDEX i;
  for (i = 0; i < GetSize(); i++) {
    PFilePath key = GetKeyAt(i);
    GetAt(key)->Flush(key);
  }

  mutex.Signal();
}

////////////////////////////////////////////////////////////
//
// PConfig::
//
// Create a new configuration object
//
////////////////////////////////////////////////////////////

void PConfig::Construct(Source src,
                        const PString & appname,
                        const PString & /*manuf*/)
{
  // handle cnvironment configs differently
  if (src == PConfig::Environment)  {
    config = configDict->GetEnvironmentInstance();
    return;
  }
  
  PString name;
  PFilePath filename, readFilename;
  
  // look up file name to read, and write
  if (src == PConfig::System)
    LocateFile(SYS_CONFIG_NAME, readFilename, filename);
  else
    filename = readFilename = PProcess::Current().GetConfigurationFile();

  // get, or create, the configuration
  config = configDict->GetFileConfigInstance(filename, readFilename);
}

PConfig::PConfig(int, const PString & name)
  : defaultSection("Options")
{
  PFilePath readFilename, filename;
  LocateFile(name, readFilename, filename);
  config = configDict->GetFileConfigInstance(filename, readFilename);
}

void PConfig::Construct(const PFilePath & theFilename)

{
  config = configDict->GetFileConfigInstance(theFilename, theFilename);
}

PConfig::~PConfig()

{
  configDict->RemoveInstance(config);
}

////////////////////////////////////////////////////////////
//
// PConfig::
//
// Return a list of all the section names in the file.
//
////////////////////////////////////////////////////////////

PStringList PConfig::GetSections() const
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PStringList list;

  for (PINDEX i = 0; i < (*config).GetSize(); i++)
    list.AppendString((*config)[i]);

  config->Signal();

  return list;
}


////////////////////////////////////////////////////////////
//
// PConfig::
//
// Return a list of all the keys in the section. If the section name is
// not specified then use the default section.
//
////////////////////////////////////////////////////////////

PStringList PConfig::GetKeys(const PString & theSection) const
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PINDEX index;
  PStringList list;

  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) {
    PXConfigSectionList & section = (*config)[index].GetList();
    for (PINDEX i = 0; i < section.GetSize(); i++)
      list.AppendString(section[i]);
  }

  config->Signal();
  return list;
}



////////////////////////////////////////////////////////////
//
// PConfig::
//
// Delete all variables in the specified section. If the section name is
// not specified then use the default section.
//
////////////////////////////////////////////////////////////

void PConfig::DeleteSection(const PString & theSection)

{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PStringList list;

  PINDEX index;
  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) {
    config->RemoveAt(index);
    config->SetDirty();
  }

  config->Signal();
}


////////////////////////////////////////////////////////////
//
// PConfig::
//
// Delete the particular variable in the specified section.
//
////////////////////////////////////////////////////////////

void PConfig::DeleteKey(const PString & theSection, const PString & theKey)
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PINDEX index;
  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) {
    PXConfigSectionList & section = (*config)[index].GetList();
    PINDEX index_2;
    if ((index_2 = section.GetValuesIndex(theKey)) != P_MAX_INDEX) {
      section.RemoveAt(index_2);
      config->SetDirty();
    }
  }

  config->Signal();
}



////////////////////////////////////////////////////////////
//
// PConfig::
//
// Test if there is a value for the key.
//
////////////////////////////////////////////////////////////

BOOL PConfig::HasKey(const PString & theSection, const PString & theKey) const
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  BOOL present = FALSE;
  PINDEX index;
  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) {
    PXConfigSectionList & section = (*config)[index].GetList();
    present = section.GetValuesIndex(theKey) != P_MAX_INDEX;
  }

  config->Signal();
  return present;
}



////////////////////////////////////////////////////////////
//
// PConfig::
//
// Get a string variable determined by the key in the section.
//
////////////////////////////////////////////////////////////

PString PConfig::GetString(const PString & theSection,
                                    const PString & theKey, const PString & dflt) const
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PString value = dflt;
  PINDEX index;
  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) {

    PXConfigSectionList & section = (*config)[index].GetList();
    if ((index = section.GetValuesIndex(theKey)) != P_MAX_INDEX) 
      value = section[index].GetValue();
  }

  config->Signal();
  return value;
}


////////////////////////////////////////////////////////////
//
// PConfig::
//
// Set a string variable determined by the key in the section.
//
////////////////////////////////////////////////////////////

void PConfig::SetString(const PString & theSection,
                        const PString & theKey,
                        const PString & theValue)
{
  PAssert(config != NULL, "config instance not set");
  config->Wait();

  PINDEX index;
  PXConfigSection * section;
  PXConfigValue   * value;

  if ((index = config->GetSectionsIndex(theSection)) != P_MAX_INDEX) 
    section = &(*config)[index];
  else {
    section = new PXConfigSection(theSection);
    config->Append(section);
    config->SetDirty();
  } 

  if ((index = section->GetList().GetValuesIndex(theKey)) != P_MAX_INDEX) 
    value = &(section->GetList()[index]);
  else {
    value = new PXConfigValue(theKey);
    section->GetList().Append(value);
    config->SetDirty();
  }

  if (theValue != value->GetValue()) {
    value->SetValue(theValue);
    config->SetDirty();
  }

  config->Signal();
}


///////////////////////////////////////////////////////////////////////////////
