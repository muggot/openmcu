/*
 * main.h
 *
 * PWLib application header file for vidtest
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * $Log: main.h,v $
 * Revision 1.4  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.3  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.2.12.1  2005/07/17 09:25:31  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.2  2003/04/29 00:57:21  dereks
 * Add user interface, option setting for format/input/fake. Works on Linux.
 *
 * Revision 1.1  2003/04/28 08:18:42  craigs
 * Initial version
 *
 */

#ifndef _Vidtest_MAIN_H
#define _Vidtest_MAIN_H

#include <ptlib/pprocess.h>
#include <ptlib/videoio.h>

class VidTest : public PProcess
{
  PCLASSINFO(VidTest, PProcess)

  public:
    VidTest();
    virtual void Main();

 protected:
   PDECLARE_NOTIFIER(PThread, VidTest, GrabAndDisplay);

  PVideoInputDevice  * grabber;
  PVideoOutputDevice * display;
  PSyncPointAck        exitGrabAndDisplay;
  PSyncPointAck        pauseGrabAndDisplay;
  PSyncPoint           resumeGrabAndDisplay;
};


#endif  // _Vidtest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
