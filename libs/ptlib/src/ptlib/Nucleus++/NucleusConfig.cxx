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
 * $Log: NucleusConfig.cxx,v $
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
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

#include <ptlib.h>


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
    BOOL Flush(const PFilePath & filename, BOOL force);

    void SetDirty()   { dirty = TRUE; }

    BOOL      AddInstance();
    BOOL      RemoveInstance(const PFilePath & filename);

  protected:
    int       instanceCount;
    PMutex    mutex;
    BOOL      dirty;
    BOOL      saveOnExit;
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
    void WriteChangedInstances(BOOL force);

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
  : PThread(10000, AutoDeleteThread),
    stop(s)
{
  Resume();
}

PXConfigWriteThread::~PXConfigWriteThread()
{
  stop.Acknowledge();
}

void PXConfigWriteThread::Main()
{
  while (!stop.Wait(30000))  // if stop.Wait() returns TRUE, we are shutting down
    configDict->WriteChangedInstances(FALSE);   // check dictionary for items that need writing

  configDict->WriteChangedInstances(TRUE);
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
  saveOnExit = TRUE;
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

/*
  this code required if no write thread used


  if (stat && saveOnExit && dirty) {
    WriteToFile(filename);
    dirty = FALSE;
  }
*/

  mutex.Signal();

  return stat;
}

BOOL PXConfig::Flush(const PFilePath & filename, BOOL force)
{
  mutex.Wait();

  BOOL stat = instanceCount == 0;

  if ((force || (instanceCount == 0)) && saveOnExit && dirty) {
    if (instanceCount != 0) 
      PProcess::PXShowSystemWarning(2000, "Flush of config with non-zero instance");
    WriteToFile(filename);
    dirty = FALSE;
  }

  mutex.Signal();

  return stat;
}

BOOL PXConfig::WriteToFile(const PFilePath & filename)
{
#ifdef WOT_NO_FILESYSTEM
  PProcess::PXShowSystemWarning
                       (2000, "No filing system for PXConfig::WriteToFile");
  return TRUE;
#else
  // make sure the directory that the file is to be written into exists
  PDirectory dir = filename.GetDirectory();
  if (!dir.Exists() && !dir.Create( 
                                   PFileInfo::UserExecute |
                                   PFileInfo::UserWrite |
                                   PFileInfo::UserRead)) {
    PProcess::PXShowSystemWarning(2000, "Cannot create PWLIB config dir");
    return FALSE;
  }

  PTextFile file;
  if (!file.Open(filename, PFile::WriteOnly)) {
    PProcess::PXShowSystemWarning(2001, "Cannot create PWLIB config file");
    return FALSE;
  }

  for (PINDEX i = 0; i < GetSize(); i++) {
    PXConfigSectionList & section = (*this)[i].GetList();
    file << "[" << (*this)[i] << "]" << endl;
    for (PINDEX j = 0; j < section.GetSize(); j++) {
      PXConfigValue & value = section[j];
      file << value << "=" << value.GetValue() << endl;
    }
    file << endl;
  }

  file.flush();
  file.SetLength(file.GetPosition());

  return TRUE;
#endif
}


BOOL PXConfig::ReadFromFile (const PFilePath & filename)
{
#ifdef WOT_NO_FILESYSTEM
  PProcess::PXShowSystemWarning
                       (2000, "No filing system for PXConfig::ReadFromFile");
  return TRUE;
#else
  PINDEX len;
  PString line;

  // clear out all information
  RemoveAll();

  // attempt to open file
  PTextFile file;
  if (!file.Open(filename, PFile::ReadOnly))
    return FALSE;

  PXConfigSection * currentSection = NULL;

  // read lines in the file
  while (file.ReadLine(line)) {
    line = line.Trim();
    if ((len = line.GetLength()) > 0) {

      // ignore comments and blank lines 
      char ch = line[(PINDEX)0];
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
          if (equals > 0) {
            PString keyStr = line.Left(equals).Trim();
            PString valStr = line.Right(len - equals - 1).Trim();
            PXConfigValue * value = new PXConfigValue(keyStr, valStr);
            currentSection->GetList().Append(value);
          }
        }
      }
    }
  }
  
  // close the file and return
  file.Close();
  return TRUE;
#endif
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
  saveOnExit = FALSE;
}


static BOOL LocateFile(const PString & baseName,
                       PFilePath & readFilename,
                       PFilePath & filename)
{
#ifndef WOT_NO_FILESYSTEM
  PFilePath userFile;

  // check the user's home directory first
  filename = readFilename = PProcess::Current().PXGetHomeDir() +
             APP_CONFIG_DIR + baseName + EXTENSION;
  if (PFile::Exists(filename))
    return TRUE;

  // otherwise check the system directory for a file to read,
  // and then create 
  readFilename = SYS_CONFIG_DIR + baseName + EXTENSION;
  return PFile::Exists(readFilename);
#else
  return FALSE;
#endif
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
  if (writeThread != NULL)
    stopConfigWriteThread.Signal();
  delete environmentInstance;
}


PXConfig * PXConfigDictionary::GetEnvironmentInstance()
{
  mutex.Wait();
  if (environmentInstance == NULL) {
    environmentInstance = new PXConfig(0);
#ifdef __CWP_HAVE_ENVIRONMENT__
    environmentInstance->ReadFromEnvironment(PProcess::Current().PXGetEnvp());
#else
    PAssertAlways("Environment Instance.  That sounds complificated.");
#endif
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

void PXConfigDictionary::WriteChangedInstances(BOOL force)
{
  mutex.Wait();

  PINDEX i;
  for (i = 0; i < GetSize(); i++) {
    PFilePath key = GetKeyAt(i);
    GetAt(key)->Flush(key, force);
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
#ifndef WOT_NO_FILESYSTEM
  PString name;
#endif

  PFilePath filename, readFilename;

#ifndef WOT_NO_FILESYSTEM
  // handle cnvironment configs differently
  if (src == PConfig::Environment) 
    config = configDict->GetEnvironmentInstance();

  // look up file name to read, and write
  if (src == PConfig::System)
    LocateFile(SYS_CONFIG_NAME, readFilename, filename);
  else {
    if (appname.IsEmpty())
      name = PProcess::Current().GetName();
    else
      name = appname;
    if (!LocateFile(name, readFilename, filename)) {
      name = PProcess::Current().GetFile().GetTitle();
      LocateFile(name, readFilename, filename);
    }
  }
#endif

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
  PFilePath filename;
  config = configDict->GetFileConfigInstance(filename, theFilename);
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

  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) {
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
  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) {
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
  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) {
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
  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) {

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

  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) 
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
  if ((index = config->GetValuesIndex(theSection)) != P_MAX_INDEX) {
    PXConfigSectionList & section = (*config)[index].GetList();
    present = section.GetValuesIndex(theKey) != P_MAX_INDEX;
  }

  config->Signal();
  return present;
}


///////////////////////////////////////////////////////////////////////////////
