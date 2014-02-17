/*
 * ptts.cxx
 *
 * Text To Speech classes
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: ptts.cxx,v $
 * Revision 1.22  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.21  2007/04/04 01:51:38  rjongbloed
 * Reviewed and adjusted PTRACE log levels
 *   Now follows 1=error,2=warn,3=info,4+=debug
 *
 * Revision 1.20  2006/07/27 09:48:24  rjongbloed
 * Fixed DevStudio 2005 compiler compatibility
 *
 * Revision 1.19  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.18  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.17  2005/01/04 08:09:42  csoutheren
 * Fixed Linux configure problems
 *
 * Revision 1.16  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.15  2004/10/23 10:56:15  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.14  2004/07/12 09:17:20  csoutheren
 * Fixed warnings and errors under Linux
 *
 * Revision 1.13  2004/07/06 10:12:54  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.12  2004/06/30 12:17:05  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.11  2004/06/19 09:02:32  csoutheren
 * Added rate and volume to keep Festival happy
 *
 * Revision 1.10  2004/06/19 07:18:59  csoutheren
 * Change TTS engine registration to use abstract factory code
 *
 * Revision 1.9  2004/04/09 06:52:17  rjongbloed
 * Removed #pargma linker command for /delayload of DLL as documentations sais that
 *   you cannot do this.
 *
 * Revision 1.8  2004/02/23 23:52:20  csoutheren
 * Added pragmas to avoid every Windows application needing to include libs explicitly
 *
 * Revision 1.7  2003/10/30 11:28:25  rjongbloed
 * Added dircetory name for Speech API under Windows.
 *
 * Revision 1.6  2003/09/17 06:02:13  csoutheren
 * Fixed windows header file problem caused by removing recursive headers
 *
 * Revision 1.5  2003/04/17 01:21:33  craigs
 * Added import of pybuildopts to correctly detect if P_SAPI is set
 *
 * Revision 1.4  2003/04/16 08:00:19  robertj
 * Windoes psuedo autoconf support
 *
 * Revision 1.3  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.2  2002/08/14 15:18:25  craigs
 * Fixed Festval implementation
 *
 * Revision 1.1  2002/08/06 04:45:58  craigs
 * Initial version
 *
 */

#ifdef __GNUC__
#pragma implementation "ptts.h"
#endif

#include "ptbuildopts.h"

////////////////////////////////////////////////////////////
#include <ptlib/pfactory.h>
#include <ptclib/ptts.h>

PINSTANTIATE_FACTORY(PTextToSpeech, PString)

// WIN32 COM stuff must be first in file to compile properly

#if P_SAPI

#if defined(P_SAPI_LIBRARY)
#pragma comment(lib, P_SAPI_LIBRARY)
#endif

#ifndef _WIN32_DCOM
#define _WIN32_DCOM 1
#endif

#include <objbase.h>
#include <atlbase.h>
#include <objbase.h>
#include <windows.h>
#include <windowsx.h>
#include <sphelper.h>

#endif

////////////////////////////////////////////////////////////

// this disables the winsock2 stuff in the Windows contain.h, to avoid header file problems
#define P_KNOCKOUT_WINSOCK2

#include <ptlib.h>
#include <ptlib/pipechan.h>
#include <ptclib/ptts.h>


////////////////////////////////////////////////////////////
//
// Text to speech using Microsoft's Speech API (SAPI)
// Can be downloaded from http://www.microsoft.com/speech/download/sdk51
//

#if P_SAPI

#define MAX_FN_SIZE 1024

class PTextToSpeech_SAPI : public PTextToSpeech
{
  PCLASSINFO(PTextToSpeech_SAPI, PTextToSpeech);
  public:
    PTextToSpeech_SAPI();
    ~PTextToSpeech_SAPI();

    // overrides
    PStringArray GetVoiceList();
    BOOL SetVoice(const PString & voice);

    BOOL SetRate(unsigned rate);
    unsigned GetRate();

    BOOL SetVolume(unsigned volume);
    unsigned GetVolume();

    BOOL OpenFile   (const PFilePath & fn);
    BOOL OpenChannel(PChannel * channel);
    BOOL IsOpen()     { return opened; }

    BOOL Close      ();
    BOOL Speak      (const PString & str, TextType hint);

  protected:
    BOOL OpenVoice();

    static PMutex refMutex;
    static int * refCount;

    PMutex mutex;
    CComPtr<ISpVoice> m_cpVoice;
    CComPtr<ISpStream> cpWavStream;
    BOOL opened;
    BOOL usingFile;
    unsigned rate, volume;
    PString voice;
};

PFactory<PTextToSpeech>::Worker<PTextToSpeech_SAPI> sapiTTSFactory("Microsoft SAPI", false);

int * PTextToSpeech_SAPI::refCount;
PMutex PTextToSpeech_SAPI::refMutex;


#define new PNEW


PTextToSpeech_SAPI::PTextToSpeech_SAPI()
{
  PWaitAndSignal m(refMutex);

  if (refCount == NULL) {
    refCount = new int;
    *refCount = 1;
    ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  } else {
    (*refCount)++;
  }

  usingFile = opened = FALSE;
}


PTextToSpeech_SAPI::~PTextToSpeech_SAPI()
{
  PWaitAndSignal m(refMutex);

  if ((--(*refCount)) == 0) {
    ::CoUninitialize();
    delete refCount;
    refCount = NULL;
  }
}

BOOL PTextToSpeech_SAPI::OpenVoice()
{
  PWaitAndSignal m(mutex);

  HRESULT hr = m_cpVoice.CoCreateInstance(CLSID_SpVoice);
  return (opened = SUCCEEDED(hr));
}

BOOL PTextToSpeech_SAPI::OpenChannel(PChannel *)
{
  PWaitAndSignal m(mutex);

  Close();
  usingFile = FALSE;
  return (opened = FALSE);
}


BOOL PTextToSpeech_SAPI::OpenFile(const PFilePath & fn)
{
  PWaitAndSignal m(mutex);

  Close();
  usingFile = TRUE;

  if (!OpenVoice())
    return FALSE;

  CSpStreamFormat wavFormat;
  wavFormat.AssignFormat(SPSF_8kHz16BitMono);

  WCHAR szwWavFileName[MAX_FN_SIZE] = L"";;

  USES_CONVERSION;
  wcscpy(szwWavFileName, T2W((const char *)fn));
  HRESULT hr = SPBindToFile(szwWavFileName, SPFM_CREATE_ALWAYS, &cpWavStream, &wavFormat.FormatId(), wavFormat.WaveFormatExPtr()); 

  if (!SUCCEEDED(hr)) {
    cpWavStream.Release();
    return FALSE;
  }

  hr = m_cpVoice->SetOutput(cpWavStream, TRUE);

  return (opened = SUCCEEDED(hr));
}

BOOL PTextToSpeech_SAPI::Close()
{
  PWaitAndSignal m(mutex);

  if (!opened)
    return TRUE;

  if (usingFile) {
    if (opened)
      m_cpVoice->WaitUntilDone(INFINITE);
    cpWavStream.Release();
  }

  if (opened)
    m_cpVoice.Release();

  opened = FALSE;

  return TRUE;
}


BOOL PTextToSpeech_SAPI::Speak(const PString & otext, TextType hint)
{
  PWaitAndSignal m(mutex);

  if (!IsOpen())
    return FALSE;

  PString text = otext;

  // do various things to the string, depending upon the hint
  switch (hint) {
    case Digits:
      {
      }
      break;

    default:
    ;
  };

  // quick hack to calculate length of Unicode string
  WCHAR * uStr = new WCHAR[text.GetLength()+1];

  USES_CONVERSION;
  wcscpy(uStr, T2W((const char *)text));

  HRESULT hr = m_cpVoice->Speak(uStr, SPF_DEFAULT, NULL);

  delete[] uStr;

  return SUCCEEDED(hr);
}

PStringArray PTextToSpeech_SAPI::GetVoiceList()
{
  PWaitAndSignal m(mutex);

  PStringArray voiceList;

  CComPtr<ISpObjectToken> cpVoiceToken;
  CComPtr<IEnumSpObjectTokens> cpEnum;
  ULONG ulCount = 0;

  //Enumerate the available voices 
  HRESULT hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

  // Get the number of voices
  if (SUCCEEDED(hr))
    hr = cpEnum->GetCount(&ulCount);

  // Obtain a list of available voice tokens, set the voice to the token, and call Speak
  while (SUCCEEDED(hr) && ulCount--) {

    cpVoiceToken.Release();

    if (SUCCEEDED(hr))
      hr = cpEnum->Next(1, &cpVoiceToken, NULL );

    if (SUCCEEDED(hr)) {
      voiceList.AppendString("voice");
    }
  } 

  return voiceList;
}

BOOL PTextToSpeech_SAPI::SetVoice(const PString & v)
{
  PWaitAndSignal m(mutex);
  voice = v;
  return TRUE;
}

BOOL PTextToSpeech_SAPI::SetRate(unsigned v)
{
  rate = v;
  return TRUE;
}

unsigned PTextToSpeech_SAPI::GetRate()
{
  return rate;
}

BOOL PTextToSpeech_SAPI::SetVolume(unsigned v)
{
  volume = v;
  return TRUE;
}

unsigned PTextToSpeech_SAPI::GetVolume()
{
  return volume;
}

#endif
// P_SAPI

////////////////////////////////////////////////////////////
//
//  Generic text to speech using Festival
//

#undef new

class PTextToSpeech_Festival : public PTextToSpeech
{
  PCLASSINFO(PTextToSpeech_Festival, PTextToSpeech);
  public:
    PTextToSpeech_Festival();
    ~PTextToSpeech_Festival();

    // overrides
    PStringArray GetVoiceList();
    BOOL SetVoice(const PString & voice);

    BOOL SetRate(unsigned rate);
    unsigned GetRate();

    BOOL SetVolume(unsigned volume);
    unsigned GetVolume();

    BOOL OpenFile   (const PFilePath & fn);
    BOOL OpenChannel(PChannel * channel);
    BOOL IsOpen()    { return opened; }

    BOOL Close      ();
    BOOL Speak      (const PString & str, TextType hint);

  protected:
    BOOL Invoke(const PString & str, const PFilePath & fn);

    PMutex mutex;
    BOOL opened;
    BOOL usingFile;
    PString text;
    PFilePath path;
    unsigned volume, rate;
    PString voice;
};

#define new PNEW

PFactory<PTextToSpeech>::Worker<PTextToSpeech_Festival> festivalTTSFactory("Festival", false);

PTextToSpeech_Festival::PTextToSpeech_Festival()
{
  PWaitAndSignal m(mutex);
  usingFile = opened = FALSE;
  rate = 8000;
  volume = 100;
}


PTextToSpeech_Festival::~PTextToSpeech_Festival()
{
  PWaitAndSignal m(mutex);
}

BOOL PTextToSpeech_Festival::OpenChannel(PChannel *)
{
  PWaitAndSignal m(mutex);

  Close();
  usingFile = FALSE;
  opened = FALSE;

  return TRUE;
}


BOOL PTextToSpeech_Festival::OpenFile(const PFilePath & fn)
{
  PWaitAndSignal m(mutex);

  Close();
  usingFile = TRUE;
  path = fn;
  opened = TRUE;

  PTRACE(3, "TTS\tWriting speech to " << fn);

  return TRUE;
}

BOOL PTextToSpeech_Festival::Close()
{
  PWaitAndSignal m(mutex);

  if (!opened)
    return TRUE;

  BOOL stat = FALSE;

  if (usingFile)
    stat = Invoke(text, path);

  text = PString();

  opened = FALSE;

  return stat;
}


BOOL PTextToSpeech_Festival::Speak(const PString & ostr, TextType hint)
{
  PWaitAndSignal m(mutex);

  if (!IsOpen()) {
    PTRACE(2, "TTS\tAttempt to speak whilst engine not open");
    return FALSE;
  }

  PString str = ostr;

  // do various things to the string, depending upon the hint
  switch (hint) {
    case Digits:
    default:
    ;
  };

  if (usingFile) {
    PTRACE(3, "TTS\tSpeaking " << ostr);
    text = text & str;
    return TRUE;
  }

  PTRACE(1, "TTS\tStream mode not supported for Festival");

  return FALSE;
}

PStringArray PTextToSpeech_Festival::GetVoiceList()
{
  PWaitAndSignal m(mutex);

  PStringArray voiceList;

  voiceList.AppendString("default");

  return voiceList;
}

BOOL PTextToSpeech_Festival::SetVoice(const PString & v)
{
  PWaitAndSignal m(mutex);
  voice = v;
  return TRUE;
}

BOOL PTextToSpeech_Festival::SetRate(unsigned v)
{
  rate = v;
  return TRUE;
}

unsigned PTextToSpeech_Festival::GetRate()
{
  return rate;
}

BOOL PTextToSpeech_Festival::SetVolume(unsigned v)
{
  volume = v;
  return TRUE;
}

unsigned PTextToSpeech_Festival::GetVolume()
{
  return volume;
}

BOOL PTextToSpeech_Festival::Invoke(const PString & otext, const PFilePath & fname)
{
  PString text = otext;
  text.Replace('\n', ' ', TRUE);
  text.Replace('\"', '\'', TRUE);
  text.Replace('\\', ' ', TRUE);
  text = "\"" + text + "\"";

  PString cmdLine = "echo " + text + " | ./text2wave -F " + PString(PString::Unsigned, rate) + " -otype riff > " + fname;

#if 1

#ifndef _WIN32_WCE
  system(cmdLine);
#endif

  return TRUE;

#else

  PPipeChannel cmd;
  int code = -1;
  if (!cmd.Open(cmdLine, PPipeChannel::ReadWriteStd)) {
    PTRACE(1, "TTS\tCannot execute command " << cmd);
  } else {
    PTRACE(3, "TTS\tCreating " << fname << " using " << cmdLine);
    cmd.Execute();
    code = cmd.WaitForTermination();
    if (code >= 0) {
      PTRACE(4, "TTS\tdata generated");
    } else {
      PTRACE(1, "TTS\tgeneration failed");
    }
  }

  return code == 0;

#endif
}

// End Of File ///////////////////////////////////////////////////////////////
