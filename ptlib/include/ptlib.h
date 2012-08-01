/*
 * ptlib.h
 *
 * Umbrella include for all non-GUI classes.
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
 * $Log: ptlib.h,v $
 * Revision 1.34  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.33  2004/10/01 07:17:16  csoutheren
 * Added PSharedptr class
 *
 * Revision 1.32  2004/07/06 10:12:52  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.31  2004/05/13 14:53:34  csoutheren
 * Add "abstract factory" template classes
 *
 * Revision 1.30  2004/04/11 13:26:25  csoutheren
 * Removed namespace problems and removed warnings for Windows <string>
 *
 * Revision 1.29  2003/09/17 05:40:25  csoutheren
 * Removed recursive includes
 *
 * Revision 1.28  2003/09/17 01:18:01  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.27  2003/04/16 07:16:55  craigs
 * Modified for new autoconf based configuration
 *
 * Revision 1.26  2002/04/09 02:30:18  robertj
 * Removed GCC3 variable as __GNUC__ can be used instead, thanks jason Spence
 *
 * Revision 1.25  2002/01/22 03:54:41  craigs
 * Removed pwavfile.h, as this has moved to PTCLib
 *
 * Revision 1.24  2001/07/19 09:53:29  rogerh
 * Add the PWAVFile class to read and write .wav files
 *
 * Revision 1.23  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 * Revision 1.22  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.21  1999/08/17 03:46:40  robertj
 * Fixed usage of inlines in optimised version.
 *
 * Revision 1.20  1999/06/17 13:38:11  robertj
 * Fixed race condition on indirect channel close, mutex needed in PIndirectChannel.
 *
 * Revision 1.19  1999/06/13 13:54:07  robertj
 * Added PConsoleChannel class for access to stdin/stdout/stderr.
 *
 * Revision 1.18  1998/11/30 22:08:56  robertj
 * Fixed backslash in #include
 *
 * Revision 1.17  1998/11/30 02:50:43  robertj
 * New directory structure
 *
 * Revision 1.16  1998/10/31 12:46:57  robertj
 * Renamed file for having general thread synchronisation objects.
 *
 * Revision 1.15  1998/09/23 06:19:52  robertj
 * Added open source copyright license.
 *
 * Revision 1.14  1998/05/30 13:25:00  robertj
 * Added PSyncPointAck class.
 *
 * Revision 1.13  1998/03/20 03:16:10  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.12  1996/09/14 13:09:16  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.11  1996/08/08 10:08:40  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.10  1996/05/23 09:57:24  robertj
 * Changed process.h to pprocess.h to avoid name conflict.
 *
 * Revision 1.9  1995/07/31 12:06:21  robertj
 * Added semaphore class.
 *
 * Revision 1.8  1995/03/12 04:44:56  robertj
 * Added dynamic link libraries.
 *
 * Revision 1.7  1994/09/25  10:43:57  robertj
 * Added pipe channel.
 *
 * Revision 1.6  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.5  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.4  1994/07/25  03:36:03  robertj
 * Added sockets to common, normalising to same comment standard.
 *
 * Revision 1.3  1994/07/21  12:17:41  robertj
 * Sockets.
 *
 * Revision 1.2  1994/06/25  12:27:39  robertj
 * *** empty log message ***
 *
 * Revision 1.1  1994/04/01  14:38:42  robertj
 * Initial revision
 *
 */

#ifndef _PTLIB_H
#define _PTLIB_H

#ifdef __GNUC__

#pragma interface

#if !defined(__USE_STD__) && __GNUC__ >= 3
#define __USE_STD__
#endif

#endif

#ifdef __NUCLEUS_PLUS__
#include "nucpp.h"
#endif

#ifdef __USE_STD__
//using namespace std;
#endif

#include "ptbuildopts.h"
#include <ptlib/contain.h>

///////////////////////////////////////////////////////////////////////////////
// PTime

#include <ptlib/ptime.h>


///////////////////////////////////////////////////////////////////////////////
// PTimeInterval

#include <ptlib/timeint.h>


///////////////////////////////////////////////////////////////////////////////
// PTimer

#include <ptlib/timer.h>


///////////////////////////////////////////////////////////////////////////////
// PDirectory

#include <ptlib/pdirect.h>


///////////////////////////////////////////////////////////////////////////////
// PFilePath

#include <ptlib/filepath.h>


///////////////////////////////////////////////////////////////////////////////
// PConfig

#include <ptlib/config.h>


///////////////////////////////////////////////////////////////////////////////
// PArgList

#include <ptlib/args.h>


///////////////////////////////////////////////////////////////////////////////
// PThread

#include <ptlib/thread.h>


///////////////////////////////////////////////////////////////////////////////
// PProcess

//#include <ptlib/pprocess.h>


///////////////////////////////////////////////////////////////////////////////
// PSemaphore

#include <ptlib/semaphor.h>


///////////////////////////////////////////////////////////////////////////////
// PMutex

#include <ptlib/mutex.h>


///////////////////////////////////////////////////////////////////////////////
// PSyncPoint

#include <ptlib/syncpoint.h>


///////////////////////////////////////////////////////////////////////////////
// PSyncPointAck, PCondMutex etc

#include <ptlib/syncthrd.h>


///////////////////////////////////////////////////////////////////////////////
// PFactory

//#include <ptlib/pfactory.h>


///////////////////////////////////////////////////////////////////////////////
// PSharedPtr

#include <ptlib/psharedptr.h>

///////////////////////////////////////////////////////////////////////////////
// PDynaLink

#include <ptlib/dynalink.h>


///////////////////////////////////////////////////////////////////////////////
// PChannel

//#include <ptlib/channel.h>


///////////////////////////////////////////////////////////////////////////////
// PIndirectChannel

#include <ptlib/indchan.h>


///////////////////////////////////////////////////////////////////////////////
// PFile

#include <ptlib/file.h>


///////////////////////////////////////////////////////////////////////////////
// PTextFile

#include <ptlib/textfile.h>


///////////////////////////////////////////////////////////////////////////////
// PStructuredFile

#include <ptlib/sfile.h>


///////////////////////////////////////////////////////////////////////////////
// PConsoleChannel

#include <ptlib/conchan.h>


///////////////////////////////////////////////////////////////////////////////
// PluginManager

//#include <ptlib/pluginmgr.h>

///////////////////////////////////////////////////////////////////////////////
// PSound

//#include <ptlib/sound.h>


///////////////////////////////////////////////////////////////////////////////
// PVideoChannel

//#include <ptlib/video.h>


///////////////////////////////////////////////////////////////////////////////


#if P_USE_INLINES

#ifdef _WIN32
#include <ptlib/msos/ptlib/ptlib.inl>
#else
#include <ptlib/unix/ptlib/ptlib.inl>
#endif
#include <ptlib/osutil.inl>

#endif

#endif // _PTLIB_H


// End Of File ///////////////////////////////////////////////////////////////
