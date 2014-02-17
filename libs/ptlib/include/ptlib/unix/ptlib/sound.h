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
 * Revision 1.20  2004/04/02 03:49:49  ykiryanov
 * Removed BeOS dependent code in major effort to clean the code
 *
 * Revision 1.19  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.18  2003/05/16 17:49:18  shawn
 * Audio code for CoreAudio of Mac OS X now uses multiple playback buffers.
 *
 * Revision 1.17  2003/03/21 11:05:34  rogerh
 * Audio changes for Mac OS X from Shawn.
 *
 * Revision 1.16  2003/03/01 17:05:05  rogerh
 * Mac OS X updates from Shawn Pai-Hsiang Hsiao
 *
 * Revision 1.15  2003/02/24 17:49:02  rogerh
 * Add Mac Core Audio changes from Shawn.
 *
 * Revision 1.14  2003/02/19 10:22:22  rogerh
 * Add ESD fix from Shawn Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>
 *
 * Revision 1.13  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.12  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.11  2001/07/09 06:16:15  yurik
 * Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.
 *
 * Revision 1.10  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.9  2001/02/07 03:33:43  craigs
 * Added functions to get sound channel parameters
 *
 * Revision 1.8  2000/07/02 14:15:55  craigs
 * Fixed minor formatting issues
 *
 * Revision 1.7  2000/05/02 08:28:34  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.6  2000/04/19 00:13:52  robertj
 * BeOS port changes.
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

#ifdef USE_ESD
#include <ptclib/delaychan.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// declare type for sound handle dictionary

#if defined(P_MAC_MPTHREADS)
class JRingBuffer;
#endif

///////////////////////////////////////////////////////////////////////////////
// PSound

  public:
    BOOL Close();
    BOOL Write(const void * buf, PINDEX len);
    BOOL Read(void * buf, PINDEX len);
  
  protected:
    BOOL  Setup();

    static PMutex dictMutex;

    Directions direction;
    PString device;
    BOOL isInitialised;

#if defined(P_MAC_MPTHREADS)
    JRingBuffer *mpInput;
#endif

    unsigned mNumChannels;
    unsigned mSampleRate;
    unsigned mBitsPerSample;
    unsigned actualSampleRate;

#ifdef USE_ESD
    PAdaptiveDelay writeDelay;
#endif

#ifdef P_MACOSX
    int caDevID;               // the CoreAdudio Device ID
    unsigned caNumChannels;    // number of channels the device has

    unsigned int chunkSamples; // number of samples each chunk has
    void *caCBData;            // pointer to various data for CA callbacks
                               // including caBufLen, caBuf, and so on

    int caBufLen;
    char *caBuf;
    char *consumerOffset, *producerOffset;

    void *caConverterRef;      // sample rate converter reference
    pthread_mutex_t caMutex;
    pthread_cond_t caCond;
#endif

// End Of File ////////////////////////////////////////////////////////////////
