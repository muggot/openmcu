/*
 * ptts.h
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
 * $Log: ptts.h,v $
 * Revision 1.9  2007/09/18 06:21:49  csoutheren
 * Add new texttospeech types
 *
 * Revision 1.8  2007/04/19 04:33:53  csoutheren
 * Fixed problems with pre-compiled headers
 *
 * Revision 1.7  2005/01/04 07:44:02  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.6  2004/07/07 07:18:43  csoutheren
 * Removed warnings on Linux from Windows static global hacks
 *
 * Revision 1.5  2004/07/06 10:12:51  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.4  2004/06/19 07:18:58  csoutheren
 * Change TTS engine registration to use abstract factory code
 *
 * Revision 1.3  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.2  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.1  2002/08/06 04:45:38  craigs
 * Initial version
 *
 */

#ifndef _PTEXTTOSPEECH
#define _PTEXTTOSPEECH

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <ptclib/ptts.h>

class PTextToSpeech : public PObject
{
  PCLASSINFO(PTextToSpeech, PObject);
  public:
    enum TextType {
      Default,
      Literal,
      Digits,
      Number,
      Currency,
      Time,
      Date,
      DateAndTime,
      Phone,
      IPAddress,
      Duration,
      Spell
    };

    virtual PStringArray GetVoiceList() = 0;
    virtual BOOL SetVoice(const PString & voice) = 0;

    virtual BOOL SetRate(unsigned rate) = 0;
    virtual unsigned GetRate() = 0;

    virtual BOOL SetVolume(unsigned volume) = 0;
    virtual unsigned GetVolume() = 0;

    virtual BOOL OpenFile   (const PFilePath & fn) = 0;
    virtual BOOL OpenChannel(PChannel * chanel) = 0;
    virtual BOOL IsOpen() = 0;

    virtual BOOL Close      () = 0;
    virtual BOOL Speak      (const PString & text, TextType hint = Default) = 0;
};

#endif
