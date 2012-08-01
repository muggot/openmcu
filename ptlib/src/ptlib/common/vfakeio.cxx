/*
 * vfakeio.cxx
 *
 * Classes to support streaming video input (grabbing) and output.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2000 Equivalence Pty. Ltd.
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
 * Contributor(s): Derek J Smithies (derek@indranet.co.nz)
 *
 * $Log: vfakeio.cxx,v $
 * Revision 1.41  2007/05/19 09:33:29  rjongbloed
 * Fixed compiler warnings.
 *
 * Revision 1.40  2007/04/20 06:47:48  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.39  2007/04/18 23:49:51  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.38  2007/04/13 07:13:14  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.37  2006/07/06 01:20:48  csoutheren
 * Disable benign warnings on VC 2005
 *
 * Revision 1.36  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.35  2006/03/17 06:56:22  csoutheren
 * Exposed video fonts to external access
 *
 * Revision 1.34  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.33  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.32.6.3  2005/07/25 12:19:23  rjongbloed
 * Fixed correct device name for fake video
 *
 * Revision 1.32.6.2  2005/07/17 12:59:04  rjongbloed
 * Cleaned up pattern identifcation (enum) and max patterns (input channel).
 *
 * Revision 1.32.6.1  2005/07/17 09:27:08  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.32  2005/01/31 08:05:41  csoutheren
 * More patches for MacOSX, thanks to Hannes Friederich
 *
 * Revision 1.31  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.30  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.29  2004/03/17 22:24:29  rjongbloed
 * Fixed MSVC warnings
 *
 * Revision 1.28  2004/03/16 10:10:47  csoutheren
 * Proper fix that does not generate warnings
 *
 * Revision 1.27  2004/03/16 08:27:14  csoutheren
 * Removed illegal characters
 *
 * Revision 1.26  2003/12/14 10:01:02  rjongbloed
 * Resolved issue with name space conflict os static and virtual forms of GetDeviceNames() function.
 *
 * Revision 1.25  2003/11/19 09:29:45  csoutheren
 * Added super hack to avoid problems with multiple plugins in a single file
 *
 * Revision 1.24  2003/11/19 04:29:46  csoutheren
 * Changed to support video output plugins
 *
 * Revision 1.23  2003/08/12 22:04:18  dereksmithies
 * Add fix from Philippe Massicotte to fix segfaults on large images. Thanks!
 *
 * Revision 1.22  2003/06/14 03:28:50  rjongbloed
 * Further MSVC warning fix up
 *
 * Revision 1.21  2003/06/14 02:59:34  rjongbloed
 * Fixed MSVC warning.
 *
 * Revision 1.20  2003/06/11 22:17:54  dereksmithies
 * Add fake video device which display text, on --videoinput 5
 *
 * Revision 1.19  2003/06/10 03:45:11  dereksmithies
 * Change so box on left moves all the way down left side of image.
 *
 * Revision 1.18  2003/06/10 00:36:57  dereksmithies
 * Formatting changes. Remove rounding errors.
 *
 * Revision 1.17  2003/06/03 04:21:49  dereksmithies
 * Add PTRACE statement, and tidy up format of one if statement.
 *
 * Revision 1.16  2003/03/17 07:46:49  robertj
 * Migrated vflip member variable and functions into PVideoDevice class.
 *
 * Revision 1.15  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.14  2002/01/28 21:22:10  dereks
 * Fix the method for returning the device name.
 *
 * Revision 1.13  2002/01/17 03:47:27  dereks
 * Fix latest addition to the fake images gallery.
 *
 * Revision 1.12  2002/01/16 08:02:06  robertj
 * MSVC compatibilty changes
 *
 * Revision 1.11  2002/01/16 03:49:23  dereks
 * Add new test image.
 *
 * Revision 1.10  2002/01/04 04:11:45  dereks
 * Add video flip code from Walter Whitlock, which flips code at the grabber.
 *
 * Revision 1.9  2001/11/28 04:39:25  robertj
 * Fixed MSVC warning
 *
 * Revision 1.8  2001/11/28 00:07:32  dereks
 * Locking added to PVideoChannel, allowing reader/writer to be changed mid call
 * Enabled adjustment of the video frame rate
 * New fictitous image, a blank grey area
 *
 * Revision 1.7  2001/03/12 03:54:11  dereks
 * Make setting frame rate consistent with that for real video device.
 *
 * Revision 1.6  2001/03/09 00:12:40  robertj
 * Fixed incorrect number of channels returned on fake video.
 *
 * Revision 1.5  2001/03/08 22:56:25  robertj
 * Fixed compatibility with new meaning of channelNumber variable, cannot be negative.
 *
 * Revision 1.4  2001/03/03 05:06:31  robertj
 * Major upgrade of video conversion and grabbing classes.
 *
 * Revision 1.3  2001/03/02 06:52:33  yurik
 * Got rid of unknown for WinCE pragma
 *
 * Revision 1.2  2000/12/19 23:58:14  robertj
 * Fixed MSVC compatibility issues.
 *
 * Revision 1.1  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 *
 */


#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>

#if P_VIDEO

#include <ptlib/vconvert.h>

#if defined(_WIN32) && !defined(P_FORCE_STATIC_PLUGIN)
#error "vfakeio.cxx must be compiled without precompiled headers"
#endif
#include <ptlib/pluginmgr.h>
#include <ptlib/pprocess.h>
#include <ptlib/videoio.h>
#include <ptclib/delaychan.h>

#ifdef __MACOSX__
namespace PWLibStupidOSXHacks {
  int loadFakeVideoStuff;
};
#endif

enum {
  eMovingBlocks,
  eMovingLine,
  eBouncingBoxes,
  eBlankImage,
  eOriginalMovingBlocks,
  eText,
  eNTSCTest,
  eNumTestPatterns
};

/****
 * The fonts for these letters were written by  Sverre H. Huseby, and have been included
 * in vfakeio by Derek Smithies.
 */
static PVideoFont::LetterData vFakeLetterData[] = {
    { ' ', 
     { "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    " }},
    { 'a', 
     { "      ",
       "      ",
       "      ",
       " **** ",
       "     *",
       " *****",
       "*    *",
       "*   **",
       " *** *",
       "      ",
       "      " }},
    { 'b', 
     { "*     ",
       "*     ",
       "*     ",
       "* *** ",
       "**   *",
       "*    *",
       "*    *",
       "**   *",
       "* *** ",
       "      ",
       "      " }},
    { 'c', 
     { "      ",
       "      ",
       "      ",
       " **** ",
       "*    *",
       "*     ",
       "*     ",
       "*    *",
       " **** ",
       "      ",
       "      " }},
    { 'd', 
     { "     *",
       "     *",
       "     *",
       " *** *",
       "*   **",
       "*    *",
       "*    *",
       "*   **",
       " *** *",
       "      ",
       "      " }},
    { 'e', 
     { "      ",
       "      ",
       "      ",
       " **** ",
       "*    *",
       "******",
       "*     ",
       "*    *",
       " **** ",
       "      ",
       "      " }},
    { 'f', 
     { "  *** ",
       " *   *",
       " *    ",
       " *    ",
       "****  ",
       " *    ",
       " *    ",
       " *    ",
       " *    ",
       "      ",
       "      " }},
    { 'g', 
     { "      ",
       "      ",
       "      ",
       " *** *",
       "*   * ",
       "*   * ",
       " ***  ",
       "*     ",
       " **** ",
       "*    *",
       " **** " }},
    { 'h', 
     { "*     ",
       "*     ",
       "*     ",
       "* *** ",
       "**   *",
       "*    *",
       "*    *",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { 'i', 
     { "     ",
       "  *  ",
       "     ",
       " **  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "*****",
       "     ",
       "     " }},
    { 'j', 
     { "     ",
       "    *",
       "     ",
       "   **",
       "    *",
       "    *",
       "    *",
       "    *",
       "*   *",
       "*   *",
       " *** " }},
    { 'k', 
     { "*     ",
       "*     ",
       "*     ",
       "*   * ",
       "*  *  ",
       "***   ",
       "*  *  ",
       "*   * ",
       "*    *",
       "      ",
       "      " }},
    { 'l', 
     { " **  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "*****",
       "     ",
       "     " }},
    { 'm', 
     { "       ",
       "       ",
       "       ",
       "* * ** ",
       "** *  *",
       "*  *  *",
       "*  *  *",
       "*  *  *",
       "*  *  *",
       "       ",
       "       " }},
    { 'n', 
     { "      ",
       "      ",
       "      ",
       "* *** ",
       "**   *",
       "*    *",
       "*    *",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { 'o', 
     { "      ",
       "      ",
       "      ",
       " **** ",
       "*    *",
       "*    *",
       "*    *",
       "*    *",
       " **** ",
       "      ",
       "      " }},
    { 'p', 
     { "      ",
       "      ",
       "      ",
       "* *** ",
       "**   *",
       "*    *",
       "**   *",
       "* *** ",
       "*     ",
       "*     ",
       "*     " }},
    { 'q', 
     { "      ",
       "      ",
       "      ",
       " *** *",
       "*   **",
       "*    *",
       "*   **",
       " *** *",
       "     *",
       "     *",
       "     *" }},
    { 'r', 
     { "    ",
       "    ",
       "    ",
       "* **",
       " *  ",
       " *  ",
       " *  ",
       " *  ",
       " *  ",
       "    ",
       "    " }},
    { 's', 
     { "      ",
       "      ",
       "      ",
       " **** ",
       "*    *",
       " **   ",
       "   ** ",
       "*    *",
       " **** ",
       "      ",
       "      " }},
    { 't', 
     { " *   ",
       " *   ",
       " *   ",
       "**** ",
       " *   ",
       " *   ",
       " *   ",
       " *  *",
       "  ** ",
       "     ",
       "     " }},
    { 'u', 
     { "      ",
       "      ",
       "      ",
       "*    *",
       "*    *",
       "*    *",
       "*    *",
       "*   **",
       " *** *",
       "      ",
       "      " }},
    { 'v', 
     { "     ",
       "     ",
       "     ",
       "*   *",
       "*   *",
       "*   *",
       " * * ",
       " * * ",
       "  *  ",
       "     ",
       "     " }},
    { 'w', 
     { "       ",
       "       ",
       "       ",
       "*     *",
       "*     *",
       "*  *  *",
       "*  *  *",
       "*  *  *",
       " ** ** ",
       "       ",
       "       " }},
    { 'x', 
     { "      ",
       "      ",
       "      ",
       "*    *",
       " *  * ",
       "  **  ",
       "  **  ",
       " *  * ",
       "*    *",
       "      ",
       "      " }},
    { 'y', 
     { "      ",
       "      ",
       "      ",
       "*    *",
       "*    *",
       "*    *",
       "*   **",
       " *** *",
       "     *",
       "*    *",
       " **** " }},
    { 'z', 
     { "      ",
       "      ",
       "      ",
       "******",
       "    * ",
       "   *  ",
       "  *   ",
       " *    ",
       "******",
       "      ",
       "      " }},
    { '\xe6', 
     { "      ",
       "      ",
       "      ",
       " ** * ",
       "   * *",
       " *****",
       "*  *  ",
       "*  * *",
       " ** * ",
       "      ",
       "      " }},
    { '\xf8', 
     { "      ",
       "      ",
       "     *",
       " **** ",
       "*   **",
       "*  * *",
       "* *  *",
       "**   *",
       " **** ",
       "*     ",
       "      " }},
    { '\xe5', 
     { "  **  ",
       " *  * ",
       "  **  ",
       " **** ",
       "     *",
       " *****",
       "*    *",
       "*   **",
       " *** *",
       "      ",
       "      " }},
    { 'A', 
     { "  **  ",
       " *  * ",
       "*    *",
       "*    *",
       "*    *",
       "******",
       "*    *",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { 'B', 
     { "***** ",
       " *   *",
       " *   *",
       " *   *",
       " **** ",
       " *   *",
       " *   *",
       " *   *",
       "***** ",
       "      ",
       "      " }},
    { 'C', 
     { " *** ",
       "*   *",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { 'D', 
     { "***** ",
       " *   *",
       " *   *",
       " *   *",
       " *   *",
       " *   *",
       " *   *",
       " *   *",
       "***** ",
       "      ",
       "      " }},
    { 'E', 
     { "*****",
       "*    ",
       "*    ",
       "*    ",
       "**** ",
       "*    ",
       "*    ",
       "*    ",
       "*****",
       "     ",
       "     " }},
    { 'F', 
     { "*****",
       "*    ",
       "*    ",
       "*    ",
       "**** ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "     ",
       "     " }},
    { 'G', 
     { " **** ",
       "*    *",
       "*     ",
       "*     ",
       "*     ",
       "*  ***",
       "*    *",
       "*   **",
       " *** *",
       "      ",
       "      " }},
    { 'H', 
     { "*    *",
       "*    *",
       "*    *",
       "*    *",
       "******",
       "*    *",
       "*    *",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { 'I', 
     { "***",
       " * ",
       " * ",
       " * ",
       " * ",
       " * ",
       " * ",
       " * ",
       "***",
       "   ",
       "   " }},
    { 'J', 
     { "  ***",
       "    *",
       "    *",
       "    *",
       "    *",
       "    *",
       "    *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { 'K', 
     { "*    *",
       "*   * ",
       "*  *  ",
       "* *   ",
       "**    ",
       "* *   ",
       "*  *  ",
       "*   * ",
       "*    *",
       "      ",
       "      " }},
    { 'L', 
     { "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "*****",
       "     ",
       "     " }},
    { 'M', 
     { "*     *",
       "**   **",
       "* * * *",
       "*  *  *",
       "*     *",
       "*     *",
       "*     *",
       "*     *",
       "*     *",
       "       ",
       "       " }},
    { 'N', 
     { "*    *",
       "*    *",
       "**   *",
       "* *  *",
       "*  * *",
       "*   **",
       "*    *",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { 'O', 
     { " *** ",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { 'P', 
     { "**** ",
       "*   *",
       "*   *",
       "*   *",
       "**** ",
       "*    ",
       "*    ",
       "*    ",
       "*    ",
       "     ",
       "     " }},
    { 'Q', 
     { " ***  ",
       "*   * ",
       "*   * ",
       "*   * ",
       "*   * ",
       "*   * ",
       "* * * ",
       "*  ** ",
       " **** ",
       "     *",
       "      " }},
    { 'R', 
     { "**** ",
       "*   *",
       "*   *",
       "*   *",
       "**** ",
       "**   ",
       "* *  ",
       "*  * ",
       "*   *",
       "     ",
       "     " }},
    { 'S', 
     { " *** ",
       "*   *",
       "*    ",
       "*    ",
       " *** ",
       "    *",
       "    *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { 'T', 
     { "*****",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "     ",
       "     " }},
    { 'U', 
     { "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { 'V', 
     { "*   *",
       "*   *",
       "*   *",
       " * * ",
       " * * ",
       " * * ",
       "  *  ",
       "  *  ",
       "  *  ",
       "     ",
       "     " }},
    { 'W', 
     { "*     *",
       "*     *",
       "*     *",
       "*     *",
       "*     *",
       "*  *  *",
       "* * * *",
       "**   **",
       "*     *",
       "       ",
       "       " }},
    { 'X', 
     { "*   *",
       "*   *",
       " * * ",
       " * * ",
       "  *  ",
       " * * ",
       " * * ",
       "*   *",
       "*   *",
       "     ",
       "     " }},
    { 'Y', 
     { "*   *",
       "*   *",
       " * * ",
       " * * ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "     ",
       "     " }},
    { 'Z', 
     { "*****",
       "    *",
       "   * ",
       "   * ",
       "  *  ",
       " *   ",
       " *   ",
       "*    ",
       "*****",
       "     ",
       "     " }},
    { '\xc6', 
     { " ** ***",
       "*  *   ",
       "*  *   ",
       "*  *   ",
       "****** ",
       "*  *   ",
       "*  *   ",
       "*  *   ",
       "*  ****",
       "       ",
       "       " }},
    { '\xd8', 
     { " *** ",
       "*  **",
       "* * *",
       "* * *",
       "* * *",
       "* * *",
       "* * *",
       "**  *",
       " *** ",
       "*    ",
       "     " }},
    { '\xc5', 
     { "  **  ",
       " *  * ",
       "  **  ",
       "  **  ",
       " *  * ",
       "*    *",
       "******",
       "*    *",
       "*    *",
       "      ",
       "      " }},
    { '!', 
     { "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       " ",
       "*",
       " ",
       " " }},
    { '"', 
     { "* *",
       "* *",
       "   ",
       "   ",
       "   ",
       "   ",
       "   ",
       "   ",
       "   ",
       "   ",
       "   " }},
    { '#', 
     { " *  * ",
       " *  * ",
       "******",
       " *  * ",
       " *  * ",
       " *  * ",
       "******",
       " *  * ",
       " *  * ",
       "      ",
       "      " }},
    { '$', 
     { "   *  ",
       " **** ",
       "*  * *",
       "*  *  ",
       "*  *  ",
       " **** ",
       "  *  *",
       "* *  *",
       " **** ",
       "  *   ",
       "      " }},
    { '%', 
     { " *  *",
       "* * *",
       " * * ",
       "   * ",
       "  *  ",
       " *   ",
       " * * ",
       "* * *",
       "*  * ",
       "     ",
       "     " }},
    { '&', 
     { "      ",
       "      ",
       " **   ",
       "*  *  ",
       "*  *  ",
       " **   ",
       "*  * *",
       "*   * ",
       " *** *",
       "      ",
       "      " }},
    { '\'',
     { " ***",
       " ** ",
       "*   ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    " }},
    { '(', 
     { "  *",
       " * ",
       " * ",
       "*  ",
       "*  ",
       "*  ",
       " * ",
       " * ",
       "  *",
       "   ",
       "   " }},
    { ')', 
      { "*  ",
       " * ",
       " * ",
       "  *",
       "  *",
       "  *",
       " * ",
       " * ",
       "*  ",
       "   ",
       "   " }},
    { '*', 
     { "      ",
       "      ",
       "      ",
       " *  * ",
       "  **  ",
       "******",
       "  **  ",
       " *  * ",
       "      ",
       "      ",
       "      " }},
    { '+', 
     { "     ",
       "     ",
       "     ",
       "  *  ",
       "  *  ",
       "*****",
       "  *  ",
       "  *  ",
       "     ",
       "     ",
       "     " }},
    { ',', 
     { "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       " ***",
       " ** ",
       "*   ",
       "    " }},
    { '-', 
     { "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "*****",
       "     ",
       "     ",
       "     ",
       "     ",
       "     " }},
    { '.', 
     { "  ",
       "  ",
       "  ",
       "  ",
       "  ",
       "  ",
       "  ",
       "**",
       "**",
       "  ",
       "  " }},
    { '/', 
     { "     *",
       "    * ",
       "   *  ",
       "   *  ",
       "  *   ",
       " *    ",
       " *    ",
       "*     ",
       "*     ",
       "      ",
       "      " }},
    { '0', 
     { "  *  ",
       " * * ",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       "*   *",
       " * * ",
       "  *  ",
       "     ",
       "     " }},
    { '1', 
     { "  *  ",
       " **  ",
       "* *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "  *  ",
       "*****",
       "     ",
       "     " }},
    { '2', 
     { " *** ",
       "*   *",
       "    *",
       "    *",
       "   * ",
       "  *  ",
       " *   ",
       "*    ",
       "*****",
       "     ",
       "     " }},
    { '3', 
     { "*****",
       "    *",
       "   * ",
       "  *  ",
       " *** ",
       "    *",
       "    *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { '4', 
     { "    * ",
       "   ** ",
       "  * * ",
       " *  * ",
       "*   * ",
       "******",
       "    * ",
       "    * ",
       "    * ",
       "      ",
       "      " }},
    { '5', 
     { "*****",
       "*    ",
       "*    ",
       "* ** ",
       "**  *",
       "    *",
       "    *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { '6', 
     { " *** ",
       "*   *",
       "*    ",
       "*    ",
       "* ** ",
       "**  *",
       "*   *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { '7', 
     { "*****",
       "    *",
       "   * ",
       "  *  ",
       "  *  ",
       " *   ",
       " *   ",
       "*    ",
       "*    ",
       "     ",
       "     " }},
    { '8', 
     { " *** ",
       "*   *",
       "*   *",
       "*   *",
       " *** ",
       "*   *",
       "*   *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { '9', 
     { " *** ",
       "*   *",
       "*   *",
       "*  **",
       " ** *",
       "    *",
       "    *",
       "*   *",
       " *** ",
       "     ",
       "     " }},
    { ':', 
     { "  ",
       "  ",
       "**",
       "**",
       "  ",
       "  ",
       "  ",
       "**",
       "**",
       "  ",
       "  " }},
    { ';', 
     { "   ",
       "   ",
       "   ",
       " **",
       " **",
       "   ",
       "   ",
       " **",
       " **",
       "*  ",
       "   " }},
    { '<', 
     { "    *",
       "   * ",
       "  *  ",
       " *   ",
       "*    ",
       " *   ",
       "  *  ",
       "   * ",
       "    *",
       "     ",
       "     " }},
    { '=', 
     { "     ",
       "     ",
       "     ",
       "*****",
       "     ",
       "*****",
       "     ",
       "     ",
       "     ",
       "     ",
       "     " }},
    { '>', 
     { "*    ",
       " *   ",
       "  *  ",
       "   * ",
       "    *",
       "   * ",
       "  *  ",
       " *   ",
       "*    ",
       "     ",
       "     " }},
    { '?', 
     { " *** ",
       "*   *",
       "    *",
       "    *",
       "   * ",
       "  *  ",
       "  *  ",
       "     ",
       "  *  ",
       "     ",
       "     " }},
    { '@', 
     { " **** ",
       "*    *",
       "*    *",
       "*  ***",
       "* *  *",
       "* * **",
       "*  * *",
       "*     ",
       " **** ",
       "      ",
       "      " }},
    { '[', 
     { "***",
       "*  ",
       "*  ",
       "*  ",
       "*  ",
       "*  ",
       "*  ",
       "*  ",
       "***",
       "   ",
       "   " }},
    { '\\',
     { "*    ",
       "*    ",
       " *   ",
       " *   ",
       "  *  ",
       "   * ",
       "   * ",
       "    *",
       "    *",
       "     ",
       "     " }},
    { ']', 
     { "***",
       "  *",
       "  *",
       "  *",
       "  *",
       "  *",
       "  *",
       "  *",
       "***",
       "   ",
       "   " }},
    { '^', 
     { "  *  ",
       " * * ",
       "*   *",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     " }},
    { '_', 
     { "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "*****",
       "     ",
       "     " }},
    { '`', 
     { "*** ",
       " ** ",
       "   *",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    ",
       "    " }},
    { '{', 
     { "  *",
       " * ",
       " * ",
       " * ",
       "*  ",
       " * ",
       " * ",
       " * ",
       "  *",
       "   ",
       "   " }},
    { '|', 
     { "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       "*",
       " ",
       " " }},
    { '}', 
     { "*  ",
       " * ",
       " * ",
       " * ",
       "  *",
       " * ",
       " * ",
       " * ",
       "*  ",
       "   ",
       "   " }},
    { '~', 
     { " *   ",
       "* * *",
       "   * ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     ",
       "     " }},
};


const PVideoFont::LetterData * PVideoFont::GetLetterData(char ascii)
{
  int q;
  int fontNumLetters = sizeof(vFakeLetterData) / sizeof(vFakeLetterData[0]);
  if (ascii == '\t')
    ascii = ' ';
  for (q = 0; q < fontNumLetters; q++)
    if (vFakeLetterData[q].ascii == ascii)
      return vFakeLetterData + q;

  return NULL;
}

/** This class defines a video input device that
    generates fictitous image data.
*/
class PVideoInputDevice_FakeVideo : public PVideoInputDevice
{
 PCLASSINFO(PVideoInputDevice_FakeVideo, PVideoInputDevice);
 public:
  /** Create a new (fake) video input device.
   */
    PVideoInputDevice_FakeVideo();


    /**Open the device given the device name.
      */
    BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /**Determine of the device is currently open.
      */
    BOOL IsOpen() ;

    /**Close the device.
      */
    BOOL Close();

    /**Start the video device I/O.
      */
    BOOL Start();

    /**Stop the video device I/O capture.
      */
    BOOL Stop();

    /**Determine if the video device I/O capture is in progress.
      */
    BOOL IsCapturing();

    /**Get a list of all of the drivers available.
      */
    static PStringList GetInputDeviceNames();

    virtual PStringList GetDeviceNames() const
      { return GetInputDeviceNames(); }

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Grab a frame. 

       There will be a delay in returning, as specified by frame rate.
      */
    virtual BOOL GetFrameData(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// Optional bytes returned.
    );

    /**Grab a frame.

       Do not delay according to the current frame rate.
      */
    virtual BOOL GetFrameDataNoDelay(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );


    /**A test image that contains area of low and high resolution.
       The picture changes every second*/
    void GrabMovingBlocksTestFrame(BYTE *resFrame);
    
    /**a test image consisting of a horizontal line moving down the image, 
       with a constantly varying background. */
    void GrabMovingLineTestFrame(BYTE *resFrame);

    /**Generate a constant image, which contains the colours for
       a NTSC test frame.*/
    void GrabNTSCTestFrame(BYTE *resFrame);
        
    /**Generate three bouncing boxes, which bounce from a different height
      */
    void GrabBouncingBoxes(BYTE *resFrame);
    
    /**Generate a static image, containing a constant field of grey.
     */
    void GrabBlankImage(BYTE *resFrame);

    /**Generate the original form of the moving blocks test frame.
     */
    void GrabOriginalMovingBlocksFrame(BYTE *resFrame);

    /**Generate a textual output on the fake video image
     */
    void GrabTextVideoFrame(BYTE *resFrame);
    
    /** Fills a region of the image with a constant colour.
     */
    void FillRect(BYTE * frame,
      int x,         int y,
                  int rectWidth, int rectHeight,
                  int r,         int g,          int b);

    /**Set the video format to be used.

       Default behaviour sets the value of the videoFormat variable and then
       returns the IsOpen() status.
    */
    virtual BOOL SetVideoFormat(
      VideoFormat videoFormat   /// New video format
    );

    /**Get the number of video channels available on the device.

       Default behaviour returns 1.
    */
    virtual int GetNumChannels() ;

    /**Set the video channel to be used on the device.

       Default behaviour sets the value of the channelNumber variable and then
       returns the IsOpen() status.
    */
    virtual BOOL SetChannel(
         int channelNumber  /// New channel number for device.
    );
    
    /**Set the colour format to be used.

       Default behaviour sets the value of the colourFormat variable and then
       returns the IsOpen() status.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat   // New colour format for device.
    );
    
    /**Set the video frame rate to be used on the device.

       Default behaviour sets the value of the frameRate variable and then
       return the IsOpen() status.
    */
    virtual BOOL SetFrameRate(
      unsigned rate  /// Frames per second
    );
         
    /**Get the minimum & maximum size of a frame on the device.

       Default behaviour returns the value 1 to UINT_MAX for both and returns
       FALSE.
    */
    virtual BOOL GetFrameSizeLimits(
      unsigned & minWidth,   /// Variable to receive minimum width
      unsigned & minHeight,  /// Variable to receive minimum height
      unsigned & maxWidth,   /// Variable to receive maximum width
      unsigned & maxHeight   /// Variable to receive maximum height
    ) ;

    /**Set the frame size to be used.

       Default behaviour sets the frameWidth and frameHeight variables and
       returns the IsOpen() status.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );
         
    void ClearMapping() { return ; }

    /**Try all known video formats & see which ones are accepted by the video driver
     */
    virtual BOOL TestAllFormats()
      { return TRUE; }
   
 protected:
   unsigned grabCount;
   PINDEX   videoFrameSize;
   PINDEX   scanLineWidth;
   PINDEX   bytesPerPixel; // 2==YUV420P, 3=RGB24, 4=RGB32
   PAdaptiveDelay m_Pacing;

   PString textLine[PVideoFont::MAX_L_HEIGHT];
};

PCREATE_VIDINPUT_PLUGIN(FakeVideo);

///////////////////////////////////////////////////////////////////////////////
// PVideoInputDevice_FakeVideo


PVideoInputDevice_FakeVideo::PVideoInputDevice_FakeVideo()
{
  SetColourFormat("RGB24");
  channelNumber = 3; // Blank screen

  grabCount = 0;
  SetFrameRate(10);
}



BOOL PVideoInputDevice_FakeVideo::Open(const PString & /*devName*/, BOOL /*startImmediate*/)
{
  deviceName = "fake";
  return TRUE;    
}


BOOL PVideoInputDevice_FakeVideo::IsOpen() 
{
  return TRUE;
}


BOOL PVideoInputDevice_FakeVideo::Close()
{
  return TRUE;
}


BOOL PVideoInputDevice_FakeVideo::Start()
{
  return TRUE;
}


BOOL PVideoInputDevice_FakeVideo::Stop()
{
  return TRUE;
}


BOOL PVideoInputDevice_FakeVideo::IsCapturing()
{
  return IsOpen();
}


PStringList PVideoInputDevice_FakeVideo::GetInputDeviceNames()
{
  PStringList list;

  list.AppendString("fake");

  return list;
}


BOOL PVideoInputDevice_FakeVideo::SetVideoFormat(VideoFormat newFormat)
{
  return PVideoDevice::SetVideoFormat(newFormat);
}


int PVideoInputDevice_FakeVideo::GetNumChannels() 
{
  return eNumTestPatterns;  
}


BOOL PVideoInputDevice_FakeVideo::SetChannel(int newChannel)
{
  return PVideoDevice::SetChannel(newChannel);
}


BOOL PVideoInputDevice_FakeVideo::SetColourFormat(const PString & newFormat)
{
  if (newFormat *= "RGB32")
    bytesPerPixel = 4;
  else if (newFormat *= "RGB24")
    bytesPerPixel = 3;
  else if (newFormat *= "YUV420P")
    bytesPerPixel = 2;
  else
    return FALSE;

  if (!PVideoDevice::SetColourFormat(newFormat))
    return FALSE;

  return SetFrameSize(frameWidth, frameHeight);
}


BOOL PVideoInputDevice_FakeVideo::SetFrameRate(unsigned rate)
{
  if (rate < 1)
    rate = 1;
  else if (rate > 50)
    rate = 50;

  return PVideoDevice::SetFrameRate(rate);
}


BOOL PVideoInputDevice_FakeVideo::GetFrameSizeLimits(unsigned & minWidth,
                                           unsigned & minHeight,
                                           unsigned & maxWidth,
                                           unsigned & maxHeight) 
{
  minWidth  = 16;
  minHeight = 12;
  maxWidth  = 1024;
  maxHeight =  768;

  return TRUE;
}


BOOL PVideoInputDevice_FakeVideo::SetFrameSize(unsigned width, unsigned height)
{
  if (!PVideoDevice::SetFrameSize(width, height))
    return FALSE;

  videoFrameSize = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);
  scanLineWidth = videoFrameSize/frameHeight;
  return videoFrameSize > 0;
}


PINDEX PVideoInputDevice_FakeVideo::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(videoFrameSize);
}


BOOL PVideoInputDevice_FakeVideo::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{    
  m_Pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

 
BOOL PVideoInputDevice_FakeVideo::GetFrameDataNoDelay(BYTE *destFrame, PINDEX * bytesReturned)
{
  grabCount++;

  // Make sure are NUM_PATTERNS cases here.
  switch(channelNumber){       
     case eMovingBlocks : 
       GrabMovingBlocksTestFrame(destFrame);
       break;
     case eMovingLine : 
       GrabMovingLineTestFrame(destFrame);
       break;
     case eBouncingBoxes :
       GrabBouncingBoxes(destFrame);
       break;
     case eBlankImage :
       GrabBlankImage(destFrame);
       break;
     case eOriginalMovingBlocks :
       GrabOriginalMovingBlocksFrame(destFrame);
       break;
     case eText :
       GrabTextVideoFrame(destFrame);
       break;
     case eNTSCTest :
       GrabNTSCTestFrame(destFrame);
       break;
     default :
       return FALSE;
  }

  if (NULL != converter) {
    if (!converter->Convert(destFrame, destFrame, bytesReturned))
      return FALSE;
  }

  if (bytesReturned != NULL)
    *bytesReturned = videoFrameSize;

  return TRUE;
}


void PVideoInputDevice_FakeVideo::FillRect(BYTE * frame,
                   int xPos, int initialYPos,
                   int rectWidth, int rectHeight,
                   int r, int g,  int b)
{
// PTRACE(0,"x,y is"<<xPos<<" "<<yPos<<" and size is "<<rectWidth<<" "<<rectHeight);

  //This routine fills a region of the video image with data. It is used as the central
  //point because one only has to add other image formats here.

  if (bytesPerPixel > 2) {
    for (int y = 0; y < rectHeight; y++) {
      BYTE * ptr = frame + (initialYPos+y)*scanLineWidth + xPos*bytesPerPixel;
      for (int x = 0; x < rectWidth; x++) {
        *ptr++ = (BYTE)r;
        *ptr++ = (BYTE)g;
        *ptr++ = (BYTE)b;
        if (bytesPerPixel > 3)
          *ptr++ = 0;
      }
    }
    return;
  }

  int yPos = initialYPos;

  int offset       = ( yPos * frameWidth ) + xPos;
  int colourOffset = ( (yPos * frameWidth) >> 2) + (xPos >> 1);

  int Y  =  ( 257 * r + 504 * g +  98 * b)/1000 + 16;
  int Cb =  (-148 * r - 291 * g + 439 * b)/1000 + 128;
  int Cr =  ( 439 * r - 368 * g -  71 * b)/1000 + 128;

  unsigned char * Yptr  = frame + offset;
  unsigned char * CbPtr = frame + (frameWidth * frameHeight) + colourOffset;
  unsigned char * CrPtr = frame + (frameWidth * frameHeight) + (frameWidth * frameHeight/4)  + colourOffset;

  int rr ;
  int halfRectWidth = rectWidth >> 1;
  int halfWidth     = frameWidth >> 1;
  
  for (rr = 0; rr < rectHeight;rr+=2) {
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;

    memset(CbPtr, Cb, halfRectWidth);
    memset(CrPtr, Cr, halfRectWidth);

    CbPtr += halfWidth;
    CrPtr += halfWidth;
  }
}

void PVideoInputDevice_FakeVideo::GrabBouncingBoxes(BYTE *resFrame)
{
  FillRect(resFrame,
     0, 0, frameWidth, frameHeight, //Fill the whole frame with the colour.
     200,200,200); //a light grey colour.

  double t= (grabCount%50) -25 ;
  double h=  t*t*frameHeight*0.85/625;
  int    yBox = (int)h;
  yBox= (yBox>>1) * 2;  //yBox is even.
  
  int boxHeight= (int)(frameHeight*0.1);
  boxHeight= (boxHeight >>1) * 2;
  int boxWidth = (int)(frameWidth*0.1);
  boxWidth = (boxWidth >>1) * 2;

  FillRect(resFrame,
     frameWidth >> 2, yBox,  boxWidth, boxHeight,
           255, 0, 0); // Red Box.

  t= (grabCount%40) -20 ;
  h= t*t*frameHeight*0.85/400 ;
  yBox = (int)h;
  yBox= (yBox>>1) * 2;  //yBox is even.

  FillRect(resFrame,
           frameWidth>>1, yBox, boxWidth, boxHeight,
           0, 255, 0); // Green

  t= (grabCount%100) -50 ;
  h= t*t*frameHeight*0.85/2500;      
  yBox = (int)h;
  yBox= (yBox>>1) * 2;  //yBox is even.

  FillRect(resFrame,
           (frameWidth>>1) + (frameWidth>>2), yBox,  boxWidth, boxHeight,
           0, 0, 255); // Blue
}

void PVideoInputDevice_FakeVideo::GrabNTSCTestFrame(BYTE *resFrame)
{
  //  Test image # 1
  //  A static image is generated, consisting of a series of coloured block.
  //  Sample NTSC test frame is found at http://www.displaymate.com/patterns.html
  //
  static int row1[7][3] = {
    { 204, 204, 204 },   // 80% grey
    { 255, 255,   0 },   // yellow
    {   0, 255, 255 },   // cyan
    {   0, 255,   0 },   // green
    { 255,   0, 255 },   // magenta
    { 255,   0,   0 },   // red
    {   0,   0, 255 },   // blue
  };

  static int row2[7][3] = {
    {   0,   0, 255 },   // blue
    {  19,  19,  19 },   // black
    { 255,   0, 255 },   // magenta
    {  19,  19,  19 },   // black
    {   0, 255, 255 },   // cyan
    {  19,  19,  19 },   // black
    { 204, 204, 204 },   // grey
  };

  static int row3a[4][3] = {
    {   8,  62,  89 },   // I
    { 255, 255, 255 },   // white
    {  58,   0, 126 },   // +Q
    {  19,  19,  19 },   // black
  };

  static int row3b[3][3] = {
    {   0,   0,   0 },   // 3.5
    {  19,  19,  19 },   // 7.5
    {  38,  38,  38 },   // 11.5
  };

  static int row3c[3] = { 19,  19,  19 };

  int row1Height = (int)(0.66 * frameHeight);
  int row2Height = (int)((0.75 * frameHeight) - row1Height);
  row1Height = (row1Height>>1)*2;     //Require that height is even.
  row2Height = (row2Height>>1)*2;     
  int row3Height = frameHeight - row1Height - row2Height;

  int columns[8];
  PINDEX i;

  for(i=0;i<7;i++) {    
    columns[i]= i*frameWidth/7;
    columns[i]= (columns[i]>>1)*2;  // require that columns[i] is even.
  }
  columns[7] = frameWidth;


  // first row
  for (i = 0; i < 6; i++) 
    FillRect(resFrame,
             columns[i], 0, columns[i+1]-columns[i], row1Height, //x,y,w,h
             row1[i][0], row1[i][1], row1[i][2]); // rgb



  // second row
  for (i = 0; i < 7; i++) 
    FillRect(resFrame,
             columns[i], row1Height,  columns[i+1]-columns[i], row2Height, 
             row2[i][0], row2[i][1], row2[i][2]);

  // third row
  int columnBot[5];

  for (i=0; i<4; i++) {    
    columnBot[i]= i*columns[5]/4;
    columnBot[i] = 2 * (columnBot[i]>>1);
  }
  columnBot[4]= columns[5];

  for (i = 0; i < 4; i++) 
    FillRect(resFrame,
             columnBot[i],row1Height + row2Height, columnBot[i+1]-columnBot[i], row3Height, 
             row3a[i][0], row3a[i][1], row3a[i][2]);

  for (i=0; i<3; i++) {
    columnBot[i] = columns[4]+(i*frameWidth)/(7*3);
    columnBot[i] = 2 * (columnBot[i]>>1);       //Force even.
  }
  columnBot[3]= columns[5];

  for (i = 0; i < 3; i++) 
    FillRect(resFrame,
             columnBot[i], row1Height + row2Height, columnBot[i+1] - columnBot[i], row3Height,
             row3b[i][0], row3b[i][1], row3b[i][2]);

  FillRect(resFrame,
           columns[6], row1Height + row2Height, columns[7] - columns[6], row3Height, 
           row3c[0], row3c[1], row3c[2]);
}


void PVideoInputDevice_FakeVideo::GrabMovingBlocksTestFrame(BYTE * resFrame)
{
  // Test image # 2
  /*Brightness is set to alter, left to right.
  Colour component alters top to bottom.

  Image contains lots of high and low resolution areas.
  */
  unsigned wi,hi, colourIndex,colNo, boxSize;   

#define COL(b,x,y) ((b+x+y)%7)

  static int background[7][3] = {
    { 254, 254, 254 },   // white
    { 255, 255,   0 },   // yellow
    {   0, 255, 255 },   // cyan
    {   0, 255,   0 },   // green
    { 255,   0, 255 },   // magenta
    { 255,   0,   0 },   // red
    {   0,   0, 255 },   // blue
  };

  int columns[9];
  int heights[9];
  int offset;
  offset = (frameWidth >> 3) & 0xffe;

  for(wi = 0; wi < 8; wi++) 
    columns[wi] = wi * offset;
  columns[8] = frameWidth;

  offset = (frameHeight >> 3) & 0xffe;
  for(hi = 0; hi < 9; hi++) 
    heights[hi] = hi * offset;
  heights[8] = frameHeight;

  grabCount++;
  colourIndex = (unsigned int)time(NULL);//time in seconds since last epoch.
  // Provides a difference if run on two ohphone sessions.
  colNo = (colourIndex / 10) % 7;   //Every 10 seconds, coloured background blocks move.

  for(hi = 0; hi < 8; hi++) //Fill the background in.
    for(wi = 0 ; wi < 8; wi++) {
      FillRect(resFrame,
               columns[wi], heights[hi], columns[wi + 1] - columns[wi], heights[hi + 1] - heights[hi],
               background[COL(colNo, wi, hi)][0], background[COL(colNo, wi, hi)][1], background[COL(colNo, wi, hi)][2]);
    }

    //Draw a black box rapidly moving down the left of the window.
    boxSize= frameHeight / 10;
    hi = ((3 * colourIndex) % (frameHeight-boxSize)) & 0xffe; //Make certain hi is even.
    FillRect(resFrame, 10, hi, boxSize, boxSize, 0, 0, 0); //Black Box.

    //Draw four parallel black lines, which move up the middle of the window.
    colourIndex = colourIndex / 3;     //Every three seconds, lines move.

    for(wi = 0; wi < 2; wi++) 
      columns[wi]= (((wi + 1)  * frameWidth) / 3) & 0xffe;// Force columns to be even.

    hi = colourIndex % ((frameHeight - 16) / 2);
    hi = (frameHeight - 16) - (hi * 2);     //hi is even, Lines move in opp. direction to box.

    unsigned yi;    
    for(yi = 0; yi < 4; yi++) 
      FillRect(resFrame,
               columns[0], hi+(yi * 4), columns[1] - columns[0], 2,
               0, 0, 0);
}


void PVideoInputDevice_FakeVideo::GrabMovingLineTestFrame(BYTE *resFrame)
{
  //  Test image # 3
  //  Faster image generation. Same every times system runs.
  //  Colours cycle through. Have a vertical lines style of pattern.
  //  There is a horizontal bar which moves down the screen .
  static int v=0;
  int r,g,b;

  v++;
  r = (200+v) & 255;
  g = (100+v) & 255;
  b = (000+v) & 255;

  FillRect(resFrame, 0, 0,frameWidth, frameHeight, r, g, b);

  int hi = (v % (frameHeight-2) >> 1) *2;
  FillRect(resFrame, 0, hi, frameWidth, 2, 0, 0, 0);
}

void PVideoInputDevice_FakeVideo::GrabBlankImage(BYTE *resFrame)
{
  // Change colour every second, cycle is:
  // black, red, green, yellow, blue, magenta, cyan, white
  int mask = grabCount/frameRate;
  FillRect(resFrame,
           0, 0, frameWidth, frameHeight, //Fill the whole frame with the colour.
           (mask&1) ? 255 : 0, // red
           (mask&2) ? 255 : 0, // green
           (mask&4) ? 255 : 0);//blue
}

void PVideoInputDevice_FakeVideo::GrabOriginalMovingBlocksFrame(BYTE *frame)
{
  unsigned wi,hi,colourIndex,colourNumber;
  int framesize = frameWidth*frameHeight;

  static int gCount=0;
  gCount++;

  colourIndex = gCount/10;
  colourNumber= (colourIndex/10)%7;   //Every 10 seconds, coloured background blocks move.

  for(hi=0; hi<frameHeight; hi++)               //slow moving group of lines going upwards.
    for(wi=0; wi<frameWidth; wi++) 
      if ( (wi>frameWidth/3)&&(wi<frameWidth*2/3)&&
        ( ((gCount+hi)%frameHeight)<16)&&
        ( (hi%4)<2)                     )
        frame[(hi*frameWidth)+wi] = 16;
      else
        frame[(hi*frameWidth)+wi] = (BYTE)(((colourNumber+((wi*7)/frameWidth))%7)*35+26);

  for(hi=1; hi<=frameHeight; hi++)                 //fast moving block going downwards.
    for(wi=frameWidth/9; wi<(2*frameWidth/9); wi++) 
      if(  (( (gCount*4)+hi)%frameHeight)<20)
        frame[((frameHeight-hi)*frameWidth)+wi] = 16;

  unsigned halfWidth  = frameWidth/2;
  unsigned halfHeight = frameHeight/2;
  for(hi=1; hi<halfHeight; hi++)  
    for(wi=0; wi<halfWidth; wi++)
      frame[framesize+(hi*halfWidth)+wi] = (BYTE)(((((hi*7)/halfHeight)+colourNumber)%7)*35+26);
}


void PVideoInputDevice_FakeVideo::GrabTextVideoFrame(BYTE *resFrame)
{
  PINDEX i, j;
  static PTime startTime;

  grabCount++;
  FillRect(resFrame,
           0, 0, frameWidth, frameHeight, //Fill the whole frame with the colour.
           200, 200, 200); //a light grey colour.


  if (textLine[0].GetLength() < 2) {
    PStringStream message;
    message << PProcess::Current().GetUserName() <<  " on " <<
               PProcess::Current().GetOSName() << ":" <<
               PProcess::Current().GetOSHardware();
    PINDEX nChars = message.GetLength();
    const PVideoFont::LetterData *ld;

    for (j = 0; j < PVideoFont::MAX_L_HEIGHT; j++)
      textLine[j] = "";

    for (i = 0; i < (nChars + 2); i++){
      if (i >= nChars)
        ld = PVideoFont::GetLetterData(' ');
      else
        ld = PVideoFont::GetLetterData(message[i]);
      if (ld == NULL)
        continue;
      for (j = 0; j < PVideoFont::MAX_L_HEIGHT; j++)
        textLine[j] += ld->line[j] + PString(" ");
    }
  }

  PINDEX boxSize = (frameHeight / (PVideoFont::MAX_L_HEIGHT * 2) ) & 0xffe;
  int index = (int)((PTime() - startTime).GetMilliSeconds() / 300);

  PINDEX maxI = (frameWidth / boxSize) - 2;
  for (i = 0; i < maxI; i++)
    for (j = 0; j < PVideoFont::MAX_L_HEIGHT; j++) {
      PINDEX ii = (index + i) % textLine[0].GetLength();
      if (textLine[j][ii] != ' ')
        FillRect(resFrame,
                 (i + 1) * boxSize, (frameHeight / 3) + ((j + 1) * boxSize), //x,y start pos
                 boxSize, boxSize,                    //x,y dimension
                 250, 00, 00); //red box.
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


/**This class defines a NULL video output device.
   This will do precisely nothing with the output.
 */
class PVideoOutputDevice_NULLOutput : public PVideoOutputDevice
{
  PCLASSINFO(PVideoOutputDevice_NULLOutput, PVideoOutputDevice);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDevice_NULLOutput();

    /**Get a list of all of the drivers available.
      */
    static PStringList GetOutputDeviceNames();

    virtual PStringList GetDeviceNames() const
      { return GetOutputDeviceNames(); }

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /**Start the video device I/O.
      */
    BOOL Start();

    /**Stop the video device I/O capture.
      */
    BOOL Stop();

    /**Close the device.
      */
    virtual BOOL Close();

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen();

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Set a section of the output frame buffer.
      */
    virtual BOOL SetFrameData(
      unsigned x,
      unsigned y,
      unsigned width,
      unsigned height,
      const BYTE * data,
      BOOL endFrame = TRUE
    );

    /**Indicate frame may be displayed.
      */
    virtual BOOL EndFrame();
};

PCREATE_VIDOUTPUT_PLUGIN(NULLOutput);

///////////////////////////////////////////////////////////////////////////////
// PVideoOutputDevice_NULLOutput

PVideoOutputDevice_NULLOutput::PVideoOutputDevice_NULLOutput()
{
  deviceName = "NULL";
}


BOOL PVideoOutputDevice_NULLOutput::Open(const PString & /*deviceName*/,
                                  BOOL /*startImmediate*/)
{
  return TRUE;
}

BOOL PVideoOutputDevice_NULLOutput::Close()
{
  return TRUE;
}

BOOL PVideoOutputDevice_NULLOutput::Start()
{
  return TRUE;
}

BOOL PVideoOutputDevice_NULLOutput::Stop()
{
  return TRUE;
}

BOOL PVideoOutputDevice_NULLOutput::IsOpen()
{
  return TRUE;
}


PStringList PVideoOutputDevice_NULLOutput::GetOutputDeviceNames()
{
  PStringList list;
  list += "NULL";
  return list;
}


PINDEX PVideoOutputDevice_NULLOutput::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(CalculateFrameBytes(frameWidth, frameHeight, colourFormat));
}


BOOL PVideoOutputDevice_NULLOutput::SetFrameData(unsigned /*x*/, unsigned /*y*/,
                                          unsigned /*width*/, unsigned /*height*/,
                                          const BYTE * /*data*/,
                                          BOOL /*endFrame*/)
{
  return TRUE;
}


BOOL PVideoOutputDevice_NULLOutput::EndFrame()
{
  return TRUE;
}

#endif // P_VIDEO

// End Of File ///////////////////////////////////////////////////////////////

