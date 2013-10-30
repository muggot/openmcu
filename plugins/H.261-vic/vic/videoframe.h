#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H

/************ Change log
 *
 * $Log: videoframe.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.8  2003/02/10 00:32:34  robertj
 * Removed code for redundent class and constructor.
 *
 * Revision 1.7  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.6  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.5  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


class VideoFrame {
 public:
     VideoFrame(u_char *cr, int w, int h);
     VideoFrame(int w, int h) ;
     ~VideoFrame();
       
     void SetSize(int newwidth, int newheight);

     u_char *frameptr;
     u_char *crvec;

     u_int ts;

     int    width;
     int    height;
};

#endif //ifndef VIDEO_FRAME_H


