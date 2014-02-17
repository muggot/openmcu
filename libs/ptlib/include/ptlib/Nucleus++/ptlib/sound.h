/*
 * sound.h
 *
 * Sound class.
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
 * $Log: sound.h,v $
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.5  1999/07/19 01:34:22  craigs
 * Rewite to compensate for linux OSS sensitivity to ioctl order.
 *
 * Revision 1.4  1999/06/30 13:50:21  craigs
 * Added code to allow full duplex audio
 *
 * Revision 1.3  1998/11/30 22:07:13  robertj
 * New directory structure.
 *
 * Revision 1.2  1998/09/24 04:11:57  robertj
 * Added open software license.
 *
 */

#ifndef _PSOUND

#ifndef __NUCLEUS_MNT__
#pragma interface
#endif

#include <string>
#include <list>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// declare type for sound handle dictionary

class PSoundHandleEntry : public PObject {
  PCLASSINFO(PSoundHandleEntry, PObject)

  public:
    PSoundHandleEntry();

    int handle;
    int direction;

    unsigned numChannels;
    unsigned sampleRate;
    unsigned bitsPerSample;
    unsigned fragmentValue;
    BOOL isInitialised;
};

PDICTIONARY(PSoundHandleDict, PString, PSoundHandleEntry);

///////////////////////////////////////////////////////////////////////////////
// PSound

#include "../../sound.h"

    virtual PString GetName() const;
	virtual BOOL Read(void * buf, PINDEX len);
	virtual BOOL Write(const void * buf, PINDEX len);
	virtual BOOL Close();

    PString m_device;
    Directions m_Direction;
    unsigned m_numChannels;
    unsigned m_sampleRate;
    unsigned m_bitsPerSample;

    unsigned long m_soundbitereadptr;

public:
};


#endif
