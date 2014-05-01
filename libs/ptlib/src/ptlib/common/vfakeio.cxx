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
 * $Revision: 25134 $
 * $Author: shorne $
 * $Date: 2011-02-20 01:53:16 -0600 (Sun, 20 Feb 2011) $
 */

#include <ptlib.h>

#if P_VIDEO

#include <ptlib/vconvert.h>

#include <ptlib/pluginmgr.h>
#include <ptlib/pprocess.h>
#include <ptlib/videoio.h>
#include <ptclib/delaychan.h>
#include <ptclib/random.h>


// Device names for fake output

enum {
  eMovingBlocks,
  eMovingLine,
  eBouncingBoxes,
  eSolidColour,
  eOriginalMovingBlocks,
  eText,
  eNTSCTest,
  eNumTestPatterns
};

static const char * const FakeDeviceNames[] = {
  "Fake/MovingBlocks",
  "Fake/MovingLine",
  "Fake/BouncingBoxes",
  "Fake/SolidColour",
  "Fake/OriginalMovingBlocks",
  "Fake/Text",
  "Fake/NTSCTest",
  "fake" // Always last
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
      BOOL startImmediate = true    /// Immediately start device
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

    /**Retrieve a list of Device Capabilities
      */
    static bool GetDeviceCapabilities(
      const PString & /*deviceName*/, ///< Name of device
      Capabilities * /*caps*/         ///< List of supported capabilities
    ) { return false; }

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
    void GrabSolidColour(BYTE *resFrame);

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
       false.
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


 protected:
    enum {
      eRGB32,
      eRGB24,
      eYUV420P,
      eYUV422
    } m_internalColourFormat;

   unsigned       m_grabCount;
   PINDEX         m_videoFrameSize;
   PINDEX         m_scanLineWidth;
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

  m_grabCount = 0;
  SetFrameRate(10);
}



BOOL PVideoInputDevice_FakeVideo::Open(const PString & devName, BOOL /*startImmediate*/)
{
  PINDEX i;
  for (i = 0; i < PARRAYSIZE(FakeDeviceNames)-1; ++i) {
    if (devName *= FakeDeviceNames[i]) {
      PVideoDevice::SetChannel(i);
      break;
    }
  }

  deviceName = FakeDeviceNames[i];
  return true;
}


BOOL PVideoInputDevice_FakeVideo::IsOpen() 
{
  return true;
}


BOOL PVideoInputDevice_FakeVideo::Close()
{
  return true;
}


BOOL PVideoInputDevice_FakeVideo::Start()
{
  return true;
}


BOOL PVideoInputDevice_FakeVideo::Stop()
{
  return true;
}


BOOL PVideoInputDevice_FakeVideo::IsCapturing()
{
  return IsOpen();
}


PStringList PVideoInputDevice_FakeVideo::GetInputDeviceNames()
{
  return PStringList(PARRAYSIZE(FakeDeviceNames), FakeDeviceNames);
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
  if (newChannel < 0 || deviceName != FakeDeviceNames[PARRAYSIZE(FakeDeviceNames)-1])
    return true; // No change for -1 or explicit channel in device name

  return PVideoDevice::SetChannel(newChannel);
}


BOOL PVideoInputDevice_FakeVideo::SetColourFormat(const PString & newFormat)
{
  if (newFormat *= "RGB32")
    m_internalColourFormat = eRGB32;
  else if (newFormat *= "RGB24")
    m_internalColourFormat = eRGB24;
  else if (newFormat *= "YUV420P")
    m_internalColourFormat = eYUV420P;
  else if ((newFormat *= "YUV422") || (newFormat *= "YUY2"))
    m_internalColourFormat = eYUV422;
  else
    return false;

  if (!PVideoDevice::SetColourFormat(newFormat))
    return false;

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
  maxWidth  = PVideoFrameInfo::MaxWidth;
  maxHeight = PVideoFrameInfo::MaxHeight;

  return true;
}


BOOL PVideoInputDevice_FakeVideo::SetFrameSize(unsigned width, unsigned height)
{
  if (!PVideoDevice::SetFrameSize(width, height))
    return false;

  m_videoFrameSize = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);
  m_scanLineWidth = (frameHeight == 0) ? 0 : m_videoFrameSize/frameHeight;
  return m_videoFrameSize > 0;
}


PINDEX PVideoInputDevice_FakeVideo::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(m_videoFrameSize);
}


BOOL PVideoInputDevice_FakeVideo::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{    
  m_Pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

 
BOOL PVideoInputDevice_FakeVideo::GetFrameDataNoDelay(BYTE *destFrame, PINDEX * bytesReturned)
{
  m_grabCount++;

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
     case eSolidColour :
       GrabSolidColour(destFrame);
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
       return false;
  }

  if (NULL != converter) {
    if (!converter->Convert(destFrame, destFrame, bytesReturned))
      return false;
  }

  if (bytesReturned != NULL)
    *bytesReturned = m_videoFrameSize;

  return true;
}


void PVideoInputDevice_FakeVideo::GrabBouncingBoxes(BYTE *resFrame)
{
  FillRect(resFrame,
           0, 0, frameWidth, frameHeight, //Fill the whole frame with the colour.
           200,200,200); //a light grey colour.

  double t= ((int)(m_grabCount%50)) -25 ;
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

  t= ((int)(m_grabCount%40)) -20 ;
  h= t*t*frameHeight*0.85/400 ;
  yBox = (int)h;
  yBox= (yBox>>1) * 2;  //yBox is even.

  FillRect(resFrame,
           frameWidth>>1, yBox, boxWidth, boxHeight,
           0, 255, 0); // Green

  t= ((int)(m_grabCount%100)) -50 ;
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

  m_grabCount++;
  colourIndex = PRandom::Number();
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


void PVideoInputDevice_FakeVideo::GrabSolidColour(BYTE *resFrame)
{
  // Change colour every second, cycle is:
  // black, red, green, yellow, blue, magenta, cyan, white
  int mask = m_grabCount/frameRate;
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

  m_grabCount++;
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


static void FillRGBRect(BYTE * frame,
                        int x, int y,
                        int width, int height,
                        int r, int g, int b,
                        PINDEX scanLineWidth,
                        PINDEX bytesPerPixel)
{
  for (int dy = 0; dy < height; dy++) {
    BYTE * ptr = frame + (y+dy)*scanLineWidth + x*bytesPerPixel;
    for (int dx = 0; dx < width; dx++) {
      ptr[0] = (BYTE)r;
      ptr[1] = (BYTE)g;
      ptr[2] = (BYTE)b;
      ptr += bytesPerPixel;
    }
  }
}



static void FillYUV422Rect(BYTE * frame,
                           int x, int y,
                           int width, int height,
                           int r, int g, int b,
                           PINDEX scanLineWidth)
{
  unsigned Y, Cb, Cr;
  PColourConverter::RGBtoYUV(r, g, b, Y, Cb, Cr);

  x &= 0xfffffffe; // Make sure is even

  int halfRectWidth = width >> 1;
  for (int dy = 0; dy < height; dy++) {
    BYTE * ptr = frame + (y+dy)*scanLineWidth + x*2;
    for (int dx = 0; dx < halfRectWidth; dx++) {
      *ptr++ = (BYTE)Y;
      *ptr++ = (BYTE)Cb;
      *ptr++ = (BYTE)Y;
      *ptr++ = (BYTE)Cr;
    }
  }
}


void PVideoInputDevice_FakeVideo::FillRect(BYTE * frame,
                                           int x, int y,
                                           int width, int height,
                                           int r, int g,  int b)
{
  //This routine fills a region of the video image with data. It is used as the central
  //point because one only has to add other image formats here.

  switch (m_internalColourFormat) {
    case eRGB32 :
      FillRGBRect(frame, x, y, width, height, r, g, b, m_scanLineWidth, 4);
      break;

    case eRGB24 :
      FillRGBRect(frame, x, y, width, height, r, g, b, m_scanLineWidth, 3);
      break;

    case eYUV420P :
      PColourConverter::FillYUV420P(x, y, width, height, frameWidth, frameHeight, frame, r, g, b);
      break;

    case eYUV422 :
      FillYUV422Rect(frame, x, y, width, height, r, g, b, m_scanLineWidth);
      break;
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
      BOOL startImmediate = true    /// Immediately start device
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
      BOOL endFrame = true
    );

    /**Indicate frame may be displayed.
      */
    virtual BOOL EndFrame();

	/**Decide whether to disable Decode
	  */
	virtual BOOL DisableDecode();

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
  return true;
}

BOOL PVideoOutputDevice_NULLOutput::Close()
{
  return true;
}

BOOL PVideoOutputDevice_NULLOutput::Start()
{
  return true;
}

BOOL PVideoOutputDevice_NULLOutput::Stop()
{
  return true;
}

BOOL PVideoOutputDevice_NULLOutput::IsOpen()
{
  return true;
}


PStringList PVideoOutputDevice_NULLOutput::GetOutputDeviceNames()
{
  return PString("NULL");
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
  return true;
}


BOOL PVideoOutputDevice_NULLOutput::EndFrame()
{
  return true;
}


BOOL PVideoOutputDevice_NULLOutput::DisableDecode()
{
  return true; 
}


#endif // P_VIDEO

// End Of File ///////////////////////////////////////////////////////////////

