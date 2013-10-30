/*p64encoder.h copyright (c)Indranet Technologies ltd (lara@indranet.co.nz)
 *                       Author Derek J Smithies (derek@indranet.co.nz)
 *
 *This file contains code which is the top level of
 *      a)video grabbing
 *      b)transformation into h261 packets.
 *
 * Questions may be sent to Derek J Smithies.
 *
 ****************************************************************/

/************ Change log
 *
 * $Log: p64encoder.cxx,v $
 * Revision 1.4  2012/09/13 09:22:10  willamowius
 * avoid uninitialized compare
 *
 * Revision 1.3  2011/01/10 21:41:36  willamowius
 * revert: call PreIncEncodeSetup() again
 *
 * Revision 1.2  2011/01/09 10:06:56  shorne
 * Fix for polycom interop.. Alessandro Angeli
 *
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.3  2006/08/01 13:02:40  rjongbloed
 * Merged changes from OpenH323 on removing need to winsock (ntohl function reference)
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.2  2005/08/31 13:16:02  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 * Revision 1.16  2006/07/22 13:15:31  rjongbloed
 * Eliminate need for linking winsock just for ntohl() function.
 *
 * Revision 1.15  2003/04/03 23:54:15  robertj
 * Added fast update to H.261 codec, thanks Gustavo García Bernardo
 *
 * Revision 1.14  2003/02/10 00:32:15  robertj
 * Changed delete of array pointer (use of []) to avoid complaints.
 * Removed code for redundent class and constructor.
 *
 * Revision 1.13  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.12  2001/12/04 04:26:06  robertj
 * Added code to allow change of video quality in H.261, thanks Damian Sandras
 *
 * Revision 1.11  2001/09/25 03:14:48  dereks
 * Add constant bitrate control for the h261 video codec.
 * Thanks Tiziano Morganti for the code to set bit rate. Good work!
 *
 * Revision 1.10  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.9  2000/10/13 01:47:26  dereks
 * Include command option for setting the number of transmitted video
 * frames per second.   use --videotxfps n
 *
 * Revision 1.8  2000/09/08 06:41:38  craigs
 * Added ability to set video device
 * Added ability to select test input frames
 *
 * Revision 1.7  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#include "p64encoder.h"

P64Encoder::P64Encoder(int quant_level,int fillLevel)
{
  trans       = new Transmitter();
  h261_edr    = new H261PixelEncoder(trans);
  h261_edr->setq(quant_level);

  vid_frame   = new VideoFrame(WIDTH,HEIGHT);
  pre_vid     = new Pre_Vid_Coder();
  pre_vid->SetBackgroundFill(fillLevel);
  //vid_frame2  = new VideoFrame(WIDTH,HEIGHT); //testing
  //pre_vid2    = new Pre_Vid_Coder(); //testing
  //pre_vid2->SetBackgroundFill(fillLevel); //testing
}



P64Encoder::~P64Encoder(){
  delete pre_vid;
  delete vid_frame;
  delete h261_edr; 
  delete trans;
  //delete pre_vid2; //testing
  //delete vid_frame2; //testing
}

void P64Encoder::SetQualityLevel(int qLevel)
{
  h261_edr->setq(qLevel);
}

void P64Encoder::SetBackgroundFill(int idle)
{
  pre_vid->SetBackgroundFill(idle);
  //pre_vid2->SetBackgroundFill(idle); //testing
}

void P64Encoder::SetSize(int width,int height) {
  vid_frame->SetSize(width,height);
  //vid_frame2->SetSize(width,height); //testing
}

void P64Encoder::ProcessOneFrame() {
  pre_vid->ProcessFrame(vid_frame);
  h261_edr->consume(vid_frame);
} 

void P64Encoder::PreProcessOneFrame() {
  pre_vid->ProcessFrame(vid_frame);
  h261_edr->PreIncEncodeSetup(vid_frame);
} 

void P64Encoder::IncEncodeAndGetPacket(
  u_char * buffer,    // returns buffer of encoded data
  unsigned & length ) // returns actual length of encoded data buffer
{
  h261_edr->IncEncodeAndGetPacket(buffer, length);
}

void P64Encoder::ReadOnePacket(
      u_char * buffer,    /// Buffer of encoded data
      unsigned & length /// Actual length of encoded data buffer
      )
{
 u_char * b_ptr;
 u_char * h_ptr;

 unsigned len_head,len_buff;

 trans->GetNextPacket(&h_ptr, &b_ptr, len_head, len_buff);
 length=len_head+len_buff;
 if(length!=0) {                          //Check to see if a packet was read.
   SWAP32(buffer, h_ptr);
    memcpy(buffer+len_head,b_ptr,len_buff);
 }
}

u_char* P64Encoder::GetFramePtr()
{
  if (vid_frame)
    return vid_frame->frameptr;
  return NULL; 
}

void P64Encoder::FastUpdatePicture()
{
  pre_vid->FastUpdatePicture();
}


/////////////////////////////////////////////////////////////////////////////
//VideoFrame

VideoFrame::VideoFrame(u_char *cr, int newWidth, int newHeight)
{
  crvec = cr;
  frameptr = NULL;
  width = height = 0; // avoid uninitialized compare
  ts = 0; // avoid uninitialized compare
  SetSize(newWidth,newHeight);
}

VideoFrame::VideoFrame(int newWidth, int newHeight)
{
  crvec = NULL;
  frameptr = NULL;
  width = height = 0; // avoid uninitialized compare
  ts = 0; // avoid uninitialized compare
  SetSize(newWidth,newHeight);
}


void VideoFrame::SetSize(int newWidth, int newHeight)
{
  if ((newWidth!=width)||(newHeight!=height)) {
    width = newWidth;
    height = newHeight;
    if (frameptr)
      delete [] frameptr;
    frameptr= new BYTE[(width*height*3)>>1];
  }
}

VideoFrame::~VideoFrame()
{
  if (frameptr) 
    delete [] frameptr;
}


/////////////////////////////////////////////////////////////////////////////
