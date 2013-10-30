/*p64encoder.h copyright (c)Indranet Technologies ltd (lara@indranet.co.nz)
 *                        Author Derek J Smithies (derek@indranet.co.nz)
 *
 *
 * This file defines the p64encoder class, which is the combined total of
 * the grabber, pre encoder, and encoder classes.
 */

/************ Change log
 *
 * $Log: p64encoder.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
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
 *
 * Revision 1.17  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.16  2002/01/04 02:48:05  dereks
 * Fix previous commit so it compiles OK.
 *
 * Revision 1.15  2002/01/03 23:05:50  dereks
 * Add methods to count number of H261 packets waiting to be sent.
 *
 * Revision 1.14  2001/12/04 04:26:06  robertj
 * Added code to allow change of video quality in H.261, thanks Damian Sandras
 *
 * Revision 1.13  2001/09/25 03:14:48  dereks
 * Add constant bitrate control for the h261 video codec.
 * Thanks Tiziano Morganti for the code to set bit rate. Good work!
 *
 * Revision 1.12  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.11  2001/03/19 01:41:33  robertj
 * Removed last vestiges of old grabber code.
 *
 * Revision 1.10  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.9  2000/10/13 01:47:27  dereks
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


#ifndef lib_p64encoder_h
#define lib_p64encoder_h

#include "config.h"
#include "p64.h"
#include "vid_coder.h"
#include "videoframe.h"
#include "encoder-h261.h"
#include "transmitter.h"

#define WIDTH 352
#define HEIGHT 288



class P64Encoder{
  public:
  /** Constructor for using a grabber. The grabber may elect to read data from a 
      hardware device, or from data in memory, Data in memory is placed there by 
      programs such as openmcu.
	*/
  P64Encoder(int quant_level,int idle);

  ~P64Encoder();
      
  /** Set the quantization level (or quality level) in the h261 encoder class.
      Has a value of 1 (good quality) to 31 (poor quality).
      Quality is improved at the expense of bit rate.
  */
  void SetQualityLevel(int qLevel);

  /** Set the number of blocks in the background that need
      to be transmitted with each frame
  */
  void SetBackgroundFill(int idle);

  /** Set size of the grabbing window, which sets the
      size of the vid_frame class. When the vid_frame class
      is passed to grabber and encoder, the grabber and
      encoder automatically resize
  */
  void SetSize(int width,int height);

  /** Called by the display routine, so we can display
      local video using the grabbed 411 format stuff.  
      Returns the address of the grabbed 411 format frame.
  */
  u_char* GetFramePtr();

  //  /** Grabs one video frame,
  //      or generates test pattern if grabber is not operational
  //  */
  //  void GrabOneFrame();


  /** On a previously grabbed frame,
      1)do some motion and block changed checking,
      2)h261 format conversion and
      3)store result in a series of packets in a list 
        in the transmitter class.
  */
	void ProcessOneFrame();

  /** Retrieves the first packet in the list recorded in
      in the transmitter class
  */
  void ReadOnePacket(
    u_char * buffer,  // Buffer of encoded data
    unsigned & length // Actual length of encoded data buffer
  );

  /** Pre-process one video frame for later incremental encoding
      same as ProcessOneFrame() except that no encoding is done
  */
  void PreProcessOneFrame();

  /** Incrementally encodes and retrieves one packet 
      Must call PreProcesOneFrame() first, then call IncEncodeAndGetPacket()
      the same way as ReadOnePacket() is called
  */
  void IncEncodeAndGetPacket(
    u_char * buffer,  // Buffer of encoded data
    unsigned & length // Actual length of encoded data buffer
  );
 
	/** Return true if there are packets waiting to be transmitted. These
      packets were created at the last invocation of the video codec.
	*/ 
  int PacketsOutStanding()
    { return  trans->PacketsOutStanding(); }

	/** When incrementally encoding, return true if there is still 
      more encoding waiting to be done for the current video frame.
	*/ 
  int MoreToIncEncode()
    { return  h261_edr->MoreToIncEncode(); }

	/** Return the count of packets waiting to be transmitted. These
      packets were created at the last invocation of the video codec.
	*/ 
  int GetCountPacketsOutStanding()
	  { return trans->GetCountPacketsOutStanding();}

  void FastUpdatePicture();

protected:
  //variables used in grabbing/processing the image. 
  Transmitter      *trans;
  H261PixelEncoder *h261_edr;
  VideoFrame       *vid_frame;
  Pre_Vid_Coder    *pre_vid;

  VideoFrame       *vid_frame2; // for testing
  Pre_Vid_Coder    *pre_vid2; // for testing

};


#endif   //#ifndef lib_p64encoder_h
