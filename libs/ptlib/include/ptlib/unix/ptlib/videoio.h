/*
 * videoio.h
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
 * Contributor(s): ______________________________________.
 *
 * $Log: videoio.h,v $
 * Revision 1.20  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.19  2003/01/06 18:41:08  rogerh
 * Add NetBSD patches, taken from the NetBSD pkg patches.
 * Submitted by Andreas Wrede
 *
 * Revision 1.18  2002/04/10 08:40:36  rogerh
 * Simplify the SetVideoChannelFormat() code. Use the implementation in the
 * ancestor class.
 *
 * Revision 1.17  2002/04/05 06:41:54  rogerh
 * Apply video changes from Damien Sandras <dsandras@seconix.com>.
 * The Video Channel and Format are no longer set in Open(). Instead
 * call the new SetVideoChannelFormat() method. This makes video capture
 * and GnomeMeeting more stable with certain Linux video capture devices.
 *
 * Revision 1.16  2002/01/16 03:36:51  dereks
 * Add variable to match each VIDIOCMCAPTURE with a VIDIOCSYNC
 *
 * Revision 1.15  2001/11/28 00:07:32  dereks
 * Locking added to PVideoChannel, allowing reader/writer to be changed mid call
 * Enabled adjustment of the video frame rate
 * New fictitous image, a blank grey area
 *
 * Revision 1.14  2001/11/25 23:47:05  robertj
 * Changed sense of HAS_VIDEO_CAPTURE to NO_VIDEO_CAPTURE to reduce cmd line.
 *
 * Revision 1.13  2001/11/22 16:08:32  rogerh
 * Allow compiles on Linux without V4L installed (eg 2.0.36 / RedHat 5.2)
 *
 * Revision 1.12  2001/08/08 06:46:44  rogerh
 * Only implement the Whiteness and Colour methods on Linux.
 *
 * Revision 1.11  2001/08/06 19:35:27  rogerh
 * Include the relevent header file based on the version of OpenBSD.
 * Submitted by Marius Aamodt Eriksen <marius@umich.edu>
 *
 * Revision 1.10  2001/08/06 07:27:21  rogerh
 * Make a note of the new location of a header file, but do not use it yet.
 *
 * Revision 1.9  2001/08/03 04:21:51  dereks
 * Add colour/size conversion for YUV422->YUV411P
 * Add Get/Set Brightness,Contrast,Hue,Colour for PVideoDevice,  and
 * Linux PVideoInputDevice.
 * Add lots of PTRACE statement for debugging colour conversion.
 * Add support for Sony Vaio laptop under linux. Requires 2.4.7 kernel.
 *
 * Revision 1.8  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.7  2001/03/20 02:21:57  robertj
 * More enhancements from Mark Cooke
 *
 * Revision 1.6  2001/03/03 06:13:01  robertj
 * Major upgrade of video conversion and grabbing classes.
 *
 * Revision 1.5  2001/01/05 10:50:04  rogerh
 * More BSD Unix support for PVideoInputDevice
 *
 * Revision 1.4  2001/01/03 10:34:18  rogerh
 * Put Linux specific parts in P_LINUX sections and start adding some FreeBSD
 * and OpenBSD code.
 *
 * Revision 1.3  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 * Revision 1.2  2000/07/30 03:54:28  robertj
 * Added more colour formats to video device enum.
 *
 * Revision 1.1  2000/07/26 02:40:29  robertj
 * Added video I/O devices.
 *
 */

#if defined(P_LINUX) && !defined(NO_VIDEO_CAPTURE)
#include <linux/videodev.h>     /* change this to "videodev2.h" for v4l2 */
#endif

#if defined(P_FREEBSD)
#include <machine/ioctl_meteor.h>
#endif

#if defined(P_OPENBSD) || defined(P_NETBSD)
#if P_OPENBSD >= 200105
#include <dev/ic/bt8xx.h> 
#elif P_NETBSD >= 105000000
#include <dev/ic/bt8xx.h> 
#else 
#include <i386/ioctl_meteor.h>
#endif
#endif

  public:
    virtual BOOL SetVideoFormat(VideoFormat videoFormat);
    virtual int  GetNumChannels();
    virtual BOOL SetChannel(int channelNumber);
    virtual BOOL SetColourFormat(const PString & colourFormat);
    virtual BOOL SetFrameRate(unsigned rate);
    virtual BOOL GetFrameSizeLimits(unsigned & minWidth, unsigned & minHeight, unsigned & maxWidth, unsigned & maxHeight) ;
    virtual BOOL SetFrameSize(unsigned width, unsigned height);
    virtual int GetBrightness();
    virtual BOOL SetBrightness(unsigned newBrightness) ;
    virtual int GetContrast();
    virtual BOOL SetContrast(unsigned newContrast); 
    virtual int GetHue();
    virtual BOOL SetHue(unsigned newHue); 


#if defined(P_LINUX) && !defined(NO_VIDEO_CAPTURE)
    // only override these methods in Linux. Other platforms will use the
    // default methods in PVideoDevice
    virtual int GetWhiteness();
    virtual BOOL SetWhiteness(unsigned newWhiteness); 
    virtual int GetColour();
    virtual BOOL SetColour(unsigned newColour); 
    virtual BOOL SetVideoChannelFormat(int channelNumber,
				       VideoFormat videoFormat);
#endif

    /** from one ioctl call, get whiteness, brightness, colour, contrast and hue.
     */
    virtual BOOL GetParameters (int *whiteness, int *brightness, 
				int *colour, int *contrast, int *hue);

  protected:
    void ClearMapping();

    /** Do not use memory mapping, access the data with a call to ::read();
     */
    BOOL NormalReadProcess(BYTE *resultBuffer, PINDEX *bytesReturned);


#if defined(P_LINUX) && !defined(NO_VIDEO_CAPTURE)
    int    videoFd;
    struct video_capability videoCapability;
    int    canMap;  // -1 = don't know, 0 = no, 1 = yes
    int    colourFormatCode;
    PINDEX hint_index;
    BYTE * videoBuffer;
    PINDEX frameBytes;

   /** Ensure each ::ioctl(VIDIOMCAPTURE) is matched by a ::ioctl(VIDIOCSYNC).
    */
    BOOL   pendingSync[2];

    int    currentFrame;
    struct video_mbuf frame;
    struct video_mmap frameBuffer[2];
#endif

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD)
    struct video_capability
    {
        int channels;   /* Num channels */
        int maxwidth;   /* Supported width */
        int maxheight;  /* And height */
        int minwidth;   /* Supported width */
        int minheight;  /* And height */
    };

    int    videoFd;
    struct video_capability videoCapability;
    int    canMap;  // -1 = don't know, 0 = no, 1 = yes
    BYTE * videoBuffer;
    PINDEX frameBytes;
    int    mmap_size;
#endif

// End Of File ////////////////////////////////////////////////////////////////
