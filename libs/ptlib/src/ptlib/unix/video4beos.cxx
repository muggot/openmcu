/*
 * video4beos.cxx
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
 * Contributor(s): Yuri Kiryanov, ykiryanov at users.sourceforge.net
 * Jac Goudsmit <jac@be.com>.
 *
 * $Log: video4beos.cxx,v $
 * Revision 1.6  2005/08/09 09:08:13  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.5.6.1  2005/07/17 09:27:08  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.5  2004/06/16 01:55:47  ykiryanov
 * Made video capture working
 *
 * Revision 1.4  2004/01/02 23:30:18  rjongbloed
 * Removed extraneous static function for getting input device names that has been deprecated during the plug ins addition.
 *
 * Revision 1.3  2002/04/10 08:40:36  rogerh
 * Simplify the SetVideoChannelFormat() code. Use the implementation in the
 * ancestor class.
 *
 * Revision 1.2  2002/04/05 21:54:58  rogerh
 * Add SetVideoChannelFormat - Reminded by Yuri
 *
 * Revision 1.1  2001/07/09 06:16:15  yurik
 * Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.
 *
 *
 */

#include <ptlib.h>
#include <ptlib/unix/ptlib/bevideo.h>

#include <stdio.h>
#include <fcntl.h>
#include <Buffer.h>
#include <unistd.h>
#include <string.h>
#include <NodeInfo.h>
#include <scheduler.h>
#include <TimeSource.h>
#include <MediaRoster.h>
#include <Application.h>
#include <BufferGroup.h>

#include <Bitmap.h>
#include <MediaNode.h>
#include <BufferConsumer.h>
#include <TimedEventQueue.h>
#include <MediaEventLooper.h>
	
// Define default trace level
#define TL 7   

///////////////////////////////////////////////////////////////////////////////
// VideoConsumer

class VideoConsumer : 
	public BMediaEventLooper,
	public BBufferConsumer
{
public:
  VideoConsumer(
    const char * name,
    BMediaAddOn *addon,
    const uint32 internal_id,
    int XSize,
    int YSize);
  ~VideoConsumer();
	
//  BMediaNode 
public:
  virtual BMediaAddOn *AddOn(long *cookie) const;
	
protected:
  virtual void Start(bigtime_t performance_time);
  virtual void Stop(bigtime_t performance_time, bool immediate);
  virtual void Seek(bigtime_t media_time, bigtime_t performance_time);
  virtual void TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time);

  virtual void NodeRegistered();
  virtual status_t RequestCompleted(
    const media_request_info & info);
							
  virtual status_t DeleteHook(BMediaNode * node);

// BMediaEventLooper 
protected:
  virtual void HandleEvent(
    const media_timed_event *event,
    bigtime_t lateness,
    bool realTimeEvent);

// BBufferConsumer 
public:
  virtual status_t AcceptFormat(
    const media_destination &dest,
    media_format * format);
  virtual status_t GetNextInput(
    int32 * cookie,
    media_input * out_input);
							
  virtual void DisposeInputCookie(
    int32 cookie);
	
protected:
  virtual void BufferReceived(
    BBuffer * buffer);
	
private:

  virtual void ProducerDataStatus(
    const media_destination &for_whom,
    int32 status,
    bigtime_t at_media_time);									

  virtual  status_t GetLatencyFor(
    const media_destination &for_whom,
    bigtime_t * out_latency,
    media_node_id * out_id);	

  virtual status_t Connected(
    const media_source &producer,
    const media_destination &where,
    const media_format & with_format,
    media_input * out_input);							

  virtual void Disconnected(
    const media_source &producer,
    const media_destination &where);							
   virtual status_t FormatChanged(
    const media_source & producer,
    const media_destination & consumer, 
    int32 from_change_count,
    const media_format & format);
							
// implementation 
public:
  status_t CreateBuffers(
    const media_format & with_format);
  void DeleteBuffers();

  PMutex mFrameMutex;
  void GetFrame(BYTE* buffer, PINDEX* bytesReturned, 
    PColourConverter* converter);
							
private:
  uint32 mInternalID;
  BMediaAddOn* mAddOn;

  media_input mIn;
  media_destination mDestination;
  bigtime_t mMyLatency;

  BBitmap* mFrames[3];
  bool mOurBuffers;
  BBufferGroup* mBuffers;
  uint32 mBufferMap[3];	

  int mXSize;
  int mYSize;
};

const media_raw_video_format vid_format = 
{ 29.97,1,0,175, // YK
  B_VIDEO_TOP_LEFT_RIGHT,1,1,
  {B_RGB32,
   176,144,176*4, // YK
   0,0}};

VideoConsumer::VideoConsumer(
  const char * name,
  BMediaAddOn *addon,
  const uint32 internal_id,
  int XSize,
  int YSize
  ) :
	
  BMediaNode(name),
  BMediaEventLooper(),
  BBufferConsumer(B_MEDIA_RAW_VIDEO),
  mInternalID(internal_id),
  mAddOn(addon),
  mMyLatency(20000),
  mOurBuffers(false),
  mBuffers(NULL),
  
  mXSize(XSize),
  mYSize(YSize)
{ 
  PTRACE(TL, "VideoConsumer::VideoConsumer");

  AddNodeKind(B_PHYSICAL_OUTPUT);
  SetEventLatency(0);
	
  for (uint32 j = 0; j < 3; j++)
  {
    mFrames[j] = NULL;
    mBufferMap[j] = 0;
  }
	
  SetPriority(B_LOW_PRIORITY);
}

VideoConsumer::~VideoConsumer()
{
  PTRACE(TL, "VideoConsumer::~VideoConsumer");
  Quit();

  DeleteBuffers();
}

// From BMediaNode
BMediaAddOn *
VideoConsumer::AddOn(long *cookie) const
{
  PTRACE(3, "VideoConsumer::AddOn");
  
  // do the right thing if we're ever used with an add-on
  *cookie = mInternalID;
  return mAddOn;
}

void 
VideoConsumer::Start(bigtime_t performance_time)
{
  BMediaEventLooper::Start(performance_time);
}

void 
VideoConsumer::Stop(bigtime_t performance_time, bool immediate)
{
  BMediaEventLooper::Stop(performance_time, immediate);
}

void 
VideoConsumer::Seek(bigtime_t media_time, bigtime_t performance_time)
{
  BMediaEventLooper::Seek(media_time, performance_time);
}

void 
VideoConsumer::TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time)
{
  BMediaEventLooper::TimeWarp(at_real_time, to_performance_time);
}

status_t 
VideoConsumer::DeleteHook(BMediaNode *node)
{
  return BMediaEventLooper::DeleteHook(node);
}

void
VideoConsumer::NodeRegistered()
{
  PTRACE(TL, "VideoConsumer::NodeRegistered");
  mIn.destination.port = ControlPort();
  mIn.destination.id = 0;
  mIn.source = media_source::null;
  mIn.format.type = B_MEDIA_RAW_VIDEO;

  mIn.format.u.raw_video = vid_format;

  mIn.format.u.raw_video.display.line_width = mXSize; 
  mIn.format.u.raw_video.display.line_count = mYSize; 
  mIn.format.u.raw_video.display.bytes_per_row = mXSize * 4; // RGB32 only

  Run();
}

status_t
VideoConsumer::RequestCompleted(const media_request_info & info)
{
  PTRACE(TL, "VideoConsumer::RequestCompleted");
  switch(info.what)
  {
    case media_request_info::B_SET_OUTPUT_BUFFERS_FOR:
    {
      if (info.status != B_OK)
        PTRACE(1, "VideoConsumer::RequestCompleted: Not using our buffers!");
    }
    break;
    
    case media_request_info::B_SET_VIDEO_CLIPPING_FOR:
    case media_request_info::B_REQUEST_FORMAT_CHANGE:
    case media_request_info::B_SET_OUTPUT_ENABLED:
    case media_request_info::B_FORMAT_CHANGED:
    default:

    break;
  }
  
  return B_OK;
}

void
VideoConsumer::BufferReceived(BBuffer * buffer)
{
  PTRACE(TL, "VideoConsumer::Buffer " << buffer->ID() << " received");

  if (RunState() == B_STOPPED)
  {
    buffer->Recycle();
    return;
  }

  media_timed_event event(buffer->Header()->start_time, 
    BTimedEventQueue::B_HANDLE_BUFFER,
    buffer,
    BTimedEventQueue::B_RECYCLE_BUFFER);
	
  EventQueue()->AddEvent(event);
}

void
VideoConsumer::ProducerDataStatus(
  const media_destination &for_whom,
  int32 status,
  bigtime_t at_media_time)
{
  PTRACE(TL, "VideoConsumer::ProducerDataStatus");

  if (for_whom != mIn.destination)	
    return;
}

status_t
VideoConsumer::CreateBuffers(
  const media_format & with_format)
{
  PTRACE(TL, "VideoConsumer::CreateBuffers");
	
  // delete any old buffers
  DeleteBuffers();	

  status_t status = B_OK;

  // create a buffer group

  mBuffers = new BBufferGroup();
  status = mBuffers->InitCheck();
  if (B_OK != status)
  {
    PTRACE(TL, "VideoConsumer::CreateBuffers - error while creating buffer group");
    return status;
  }

  uint32 XSize = with_format.u.raw_video.display.line_width;
  uint32 YSize = with_format.u.raw_video.display.line_count;	
  color_space mColorspace = with_format.u.raw_video.display.format;
  PTRACE(TL, "VideoConsumer::CreateBuffers - Colorspace = " << (unsigned) mColorspace);

  // and attach the  bitmaps to the buffer group
  for (uint32 j=0; j < 3; j++)
  {
    mFrames[j] = new BBitmap(BRect(0, 0, (XSize-1), (YSize - 1)), mColorspace, false, true);
    if (mFrames[j]->IsValid())
    {						
      buffer_clone_info info;
      if ((info.area = area_for(mFrames[j]->Bits())) == B_ERROR)
        PTRACE(1, "VideoConsumer::CreateBuffers - error in area");
      info.offset = 0;
      info.size = (size_t) mFrames[j]->BitsLength();
      info.flags = j;
      info.buffer = 0;

      if ((status = mBuffers->AddBuffer(info)) != B_OK)
      {
        PTRACE(1, "VideoConsumer::CreateBuffers - error adding buffer to group");
        return status;
      } // if status
      else
      { 
        PTRACE(1, "VideoConsumer::CreateBuffers - successfully added buffer to group");
      }
    } // if bitmap is valid
    else 
    {
      PTRACE(1, 
      "VideoConsumer::CreateBuffers - error creating ring buffer, status " << status);
      return B_ERROR;
    } // bitmap is not valid	
  } // for loop

  BBuffer ** buffList = new BBuffer * [3];
  for (int j = 0; j < 3; j++) buffList[j] = 0;
	
  if ((status = mBuffers->GetBufferList(3, buffList)) == B_OK)	
  {
    for (int j = 0; j < 3; j++)
    {
      if (buffList[j] != NULL)
      {
        mBufferMap[j] = (uint32) buffList[j];
        PTRACE(TL, " j = " << j << " buffer = " << mBufferMap[j]);
      } 
      else
      {
        PTRACE(1, "VideoConsumer::CreateBuffers - error mapping ring buffer");
        return B_ERROR;
      } // else buflist
    } // end for
  } // if status
  else
    PTRACE(1, "VideoConsumer::CreateBuffers - error in get buffer list");
		
  PTRACE(TL, "VideoConsumer::CreateBuffers - exit");
  return status;
}

void
VideoConsumer::DeleteBuffers()
{
  PTRACE(TL, "VideoConsumer::DeleteBuffers");
	
  if (mBuffers)
  {
    delete mBuffers;
    mBuffers = NULL;
		
    for (uint32 j = 0; j < 3; j++)
    {
      if (mFrames[j]->IsValid())
      {
        delete mFrames[j];
        mFrames[j] = NULL;
      } // if bitmap
    } // for
  } // if buffers

  PTRACE(TL, "VideoConsumer::DeleteBuffers - exit");
}

status_t
VideoConsumer::Connected(
  const media_source & producer,
  const media_destination & where,
  const media_format & with_format,
  media_input * out_input)
{
  PTRACE(TL, "VideoConsumer::Connected");
	
  mIn.source = producer;
  mIn.format = with_format;
  mIn.node = Node();
  sprintf(mIn.name, "PWLV");
  *out_input = mIn;

  uint32 user_data = 0;
  int32 change_tag = 1;	
  if (CreateBuffers(with_format) == B_OK)
  {
     BBufferConsumer::SetOutputBuffersFor(producer, mDestination, 
	mBuffers, (void *)&user_data, &change_tag, true);
  }
  else
  {
    PTRACE(1, "VideoConsumer::Connected - can't create buffers");
    return B_ERROR;
  }

  PTRACE(TL, "VideoConsumer::Connected - exit");
  return B_OK;
}

void
VideoConsumer::Disconnected(
	const media_source & producer,
	const media_destination & where)
{
  PTRACE(TL, "VideoConsumer::Disconnected");

  if (where == mIn.destination && producer == mIn.source)
  {
    // disconnect the connection
    mIn.source = media_source::null;
  }
}

//---------------------------------------------------------------

status_t
VideoConsumer::AcceptFormat(
  const media_destination & dest,
  media_format * format)
{
  PTRACE(TL, "VideoConsumer::AcceptFormat") ;
	
  if (dest != mIn.destination)
  {
    PTRACE(1, "VideoConsumer::AcceptFormat - bad destination");
    return B_MEDIA_BAD_DESTINATION;	
  }
	
  if (format->type == B_MEDIA_NO_TYPE)
    format->type = B_MEDIA_RAW_VIDEO;
	
  if (format->type != B_MEDIA_RAW_VIDEO)
  {
    PTRACE(1, "VideoConsumer::AcceptFormat - bad format");
    return B_MEDIA_BAD_FORMAT;
  }

  if (format->u.raw_video.display.format != B_RGB32 &&
    format->u.raw_video.display.format != media_raw_video_format::wildcard.display.format)
  {
    PTRACE(1, "AcceptFormat - not a RGB32 format!");
    return B_MEDIA_BAD_FORMAT;
  }
		
  if (format->u.raw_video.display.format == media_raw_video_format::wildcard.display.format)
  {
    format->u.raw_video.display.format = B_RGB32;
  }

  char format_string[256];		
  string_for_format(*format, format_string, 256);
  PTRACE(TL, "VideoConsumer::AcceptFormat: " << format_string);

  return B_OK;
}


status_t
VideoConsumer::GetNextInput(
	int32 * cookie,
	media_input * out_input)
{
  PTRACE(TL, "VideoConsumer::GetNextInput");

  // custom build a destination for this connection
  // put connection number in id

  if (*cookie < 1)
  {
    mIn.node = Node();
    mIn.destination.id = *cookie;
    sprintf(mIn.name, "PWLV");
    *out_input = mIn;
    (*cookie)++;
    return B_OK;
  }
  else
  {
    PTRACE(1, "VideoConsumer::GetNextInput - bad index");
    return B_MEDIA_BAD_DESTINATION;
  }
}

void
VideoConsumer::DisposeInputCookie(int32 /*cookie*/)
{
}

status_t
VideoConsumer::GetLatencyFor(
  const media_destination &for_whom,
  bigtime_t * out_latency,
  media_node_id * out_timesource)
{
  PTRACE(TL, "VideoConsumer::GetLatencyFor");
	
  if (for_whom != mIn.destination)
    return B_MEDIA_BAD_DESTINATION;
	
  *out_latency = mMyLatency;
  *out_timesource = TimeSource()->ID();
  return B_OK;
}


status_t
VideoConsumer::FormatChanged(
  const media_source & producer,
  const media_destination & consumer, 
  int32 from_change_count,
  const media_format &format)
{
  PTRACE(TL, "VideoConsumer::FormatChanged");
	
  if (consumer != mIn.destination)
    return B_MEDIA_BAD_DESTINATION;

  if (producer != mIn.source)
    return B_MEDIA_BAD_SOURCE;

  mIn.format = format;
	
  return CreateBuffers(format);
}

void
VideoConsumer::HandleEvent(
  const media_timed_event *event,
  bigtime_t lateness,
  bool realTimeEvent)
{
  PTRACE(TL, "VideoConsumer::HandleEvent");
	
  BBuffer *buffer;
	
  switch (event->type)
  {
    case BTimedEventQueue::B_START:
    PTRACE(TL, "VideoConsumer::HandleEvent - start");
	break;

    case BTimedEventQueue::B_STOP:
    PTRACE(TL, "VideoConsumer::HandleEvent - stop");
    EventQueue()->FlushEvents(event->event_time, 
      BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER);
	break;

    case BTimedEventQueue::B_HANDLE_BUFFER:
    PTRACE(TL, "VideoConsumer::HandleEvent - handle buffer");
    buffer = (BBuffer *) event->pointer;
    if (RunState() == B_STARTED)
    {
      // see if this is one of our buffers
      uint32 index = 0;
      mOurBuffers = true;
      while(index < 3)
      if ((uint32)buffer == mBufferMap[index])
        break;
      else
        index++;
						
      if (index == 3)
      {
        // no, buffers belong to consumer
	    mOurBuffers = false;
	    index = 0;
      }

      PWaitAndSignal ws(mFrameMutex);
	  memcpy(mFrames[index]->Bits(), buffer->Data(), mFrames[index]->BitsLength());
  }
  else
   buffer->Recycle();
   break;

 default:
   PTRACE(1, "VideoConsumer::HandleEvent - bad event");
   break;
  }			
}

void VideoConsumer::GetFrame(BYTE* buffer, 
   PINDEX* bytesReturned, 
  PColourConverter* converter) 
{ 
    PAssertNULL(bytesReturned);
    
    PWaitAndSignal ws(mFrameMutex);
    {
      const BYTE* frame = (const BYTE*) mFrames[0]->Bits();
    
      if (converter != NULL) 
      {
        converter->Convert(frame, buffer, bytesReturned);
      } 
      else 
      {
        memcpy(buffer, frame, *bytesReturned);
      }
    }
}

///////////////////////////////////////////////////////////////////////////////
// PVideoDevice

PVideoInputDevice_BeOSVideo::PVideoInputDevice_BeOSVideo()
  : captureThread(NULL)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo");
  
  captureThread = NULL;
  isCapturingNow = FALSE;
}

BOOL PVideoInputDevice_BeOSVideo::Open(const PString & devName, BOOL startImmediate)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::Open");

  Close();

  frameWidth = CIFWidth;
  frameHeight = CIFHeight;
  colourFormat = "RGB32";

  deviceName = devName;

  if (startImmediate)
    return Start();
    
  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::IsOpen() 
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::IsOpen");
  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::Close()
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::Close");

  if (!IsOpen())
    return FALSE;
 
  Stop();

  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::Start()
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::Start");

  if (IsCapturing())
    return TRUE;
   
  StartNodes();    

  isCapturingNow = TRUE;

  return isCapturingNow;
}


BOOL PVideoInputDevice_BeOSVideo::Stop()
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::Stop");

  if (!IsCapturing())
    return FALSE;

  StopNodes();
  ::snooze(100000);

  isCapturingNow = FALSE;

  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::IsCapturing()
{
  return isCapturingNow;
}


BOOL PVideoInputDevice_BeOSVideo::TestAllFormats() 
{
  BOOL running = IsCapturing();
  if (running)
    Stop();

  if (running)
    return Start();

  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::SetFrameRate(unsigned rate)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::SetFrameRate");

  if (!PVideoDevice::SetFrameRate(rate))
    return FALSE;

  BOOL running = IsCapturing();
  if (running)
    Stop();
  
  if (running)
    return Start();

  return TRUE;
}


BOOL PVideoInputDevice_BeOSVideo::SetFrameSize(unsigned width, unsigned height)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::SetFrameSize, width " << width << " height " << height);

  BOOL running = IsCapturing();
  if (running)
    Stop();

  if (!PVideoDevice::SetFrameSize(width, height))
    return FALSE;

  if (running)
    return Start();

  return TRUE;
}

BOOL PVideoInputDevice_BeOSVideo::SetColourFormat(const PString & colourFmt)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::SetColourFormat, format" << colourFmt);
 
  BOOL running = IsCapturing();
  if (running)
    Stop();

  if (!PVideoDevice::SetColourFormat(colourFmt)) {
    return FALSE;
  }

  converter = PColourConverter::Create("RGB32", colourFormat, frameWidth, frameHeight);
  if (converter == NULL) {
    PTRACE(1, "Failed to make a converter.");
    return FALSE;
  }

//  if (converter->SetSrcFrameSize(width,height) == FALSE) {
//    PTRACE(1, "Failed to set source frame size of a converter.");
//    return FALSE;
//  }
//  if (converter->SetDstFrameSize(desiredFrameWidth, desiredFrameHeight, FALSE) == FALSE) {
//    PTRACE(1, "Failed to set destination frame size (+scaling) of a converter.");
//    return FALSE;
//  }
    
  if (running)
    return Start();

  return TRUE;
}


PStringList PVideoInputDevice_BeOSVideo::GetInputDeviceNames()
{
  PStringList list;

  list.AppendString("MediaKit"); 
  return list;
}


PINDEX PVideoInputDevice_BeOSVideo::GetMaxFrameBytes()
{
  if (!IsOpen())
    return 0;
  
  return GetMaxFrameBytesConverted(frameWidth * frameHeight *4); // RGB32 only 
}


BOOL PVideoInputDevice_BeOSVideo::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

BOOL PVideoInputDevice_BeOSVideo::GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned)
{
  PTRACE(TL, "PVideoInputDevice_BeOSVideo::GetFrameDataNoDelay");
  if (!IsCapturing()) 
    return FALSE;
  
  *bytesReturned = GetMaxFrameBytes();
  fVideoConsumer->GetFrame(buffer, bytesReturned, converter);
   
  return TRUE;
}

status_t PVideoInputDevice_BeOSVideo::StartNodes()
{
  PTRACE(TL, "StartNodes!");
  status_t status = B_OK;

  // find the media roster 
  fMediaRoster = BMediaRoster::Roster(&status);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't find the media roster, status " << status);
    return status;
  }	

  // find the time source 
  status = fMediaRoster->GetTimeSource(&fTimeSourceNode);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't get a time source, status " << status);
    return status;
  }

  // find a video producer node 
  PTRACE(3, "PVideoInputDevice_BeOSVideo acquiring video input node");
  status = fMediaRoster->GetVideoInput(&fProducerNode);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't find a video input!, status " << status);
    return status;
  } 

  // create the video consumer node 
  fVideoConsumer = new VideoConsumer("PWLV", NULL, 0, frameWidth, frameHeight);

  if (!fVideoConsumer) 
  {
    PTRACE(3, "Can't create a video consumer");
    return B_ERROR;
  }
	
  // register the node 
  status = fMediaRoster->RegisterNode(fVideoConsumer);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't register the video consumer, status " << status);
    return status;
  }
  
  fPort = fVideoConsumer->ControlPort();
	
  // find free producer output 
  int32 cnt = 0;
  status = fMediaRoster->GetFreeOutputsFor(fProducerNode, 
    &fProducerOut, 1,  &cnt, B_MEDIA_RAW_VIDEO);

  if (status != B_OK || cnt < 1) 
  {
    status = B_RESOURCE_UNAVAILABLE;
    PTRACE(3, "Can't find an available video stream, status " << status);
    return status;
  }

  // find free consumer input 
  cnt = 0;
  status = fMediaRoster->GetFreeInputsFor(fVideoConsumer->Node(), 
    &fConsumerIn, 1, &cnt, B_MEDIA_RAW_VIDEO);

  if (status != B_OK || cnt < 1) 
  {
    status = B_RESOURCE_UNAVAILABLE;
    PTRACE(3, 
     "Can't find an available connection to the video consumer, status " << status);
    return status;
  }

  // Connect The Nodes!!! 
  media_format format;
  format.type = B_MEDIA_RAW_VIDEO;
  media_raw_video_format vid_format = 
    { 0, 1, 0, 175, // YK
       B_VIDEO_TOP_LEFT_RIGHT, 1, 1, 
      {B_RGB32, 0, 0, 0, 0, 0}};

  vid_format.display.line_width = frameWidth;
  vid_format.display.line_count = frameHeight; 
  vid_format.display.bytes_per_row = frameWidth * 4; // RGB32 only

  format.u.raw_video = vid_format; 
	
  // connect producer to consumer 
  status = fMediaRoster->Connect(fProducerOut.source, fConsumerIn.destination,
				&format, &fProducerOut, &fConsumerIn);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't connect the video source to the video consumer, status " << status);
    return status;
  }
	
  // set time sources 
  status = fMediaRoster->SetTimeSourceFor(fProducerNode.node, fTimeSourceNode.node);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't set the timesource for the video source, status " << status);
    return status;
  }
	
  status = fMediaRoster->SetTimeSourceFor(fVideoConsumer->ID(), fTimeSourceNode.node);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't set the timesource for the video consumer, status " << status);
    return status;
  }
	
  // figure out what recording delay to use 
  bigtime_t latency = 0;
  status = fMediaRoster->GetLatencyFor(fProducerNode, &latency);
  status = fMediaRoster->SetProducerRunModeDelay(fProducerNode, latency);

  // start the nodes 
  bigtime_t initLatency = 0;
  status = fMediaRoster->GetInitialLatencyFor(fProducerNode, &initLatency);
  if (status < B_OK) 
  {
    PTRACE(3, "error getting initial latency for fCaptureNode, status " << status);	
  }
  initLatency += estimate_max_scheduling_latency();
	
  BTimeSource *timeSource = fMediaRoster->MakeTimeSourceFor(fProducerNode);
  bool running = timeSource->IsRunning();
	
  // workaround for people without sound cards 
  // because the system time source won't be running 
  bigtime_t real = BTimeSource::RealTime();
  if (!running)
  {
    status = fMediaRoster->StartTimeSource(fTimeSourceNode, real);
    if (status != B_OK) 
    {
      timeSource->Release();
      PTRACE(3, "Cannot start time source!, status" << status);
      return status;
    }
   
    status = fMediaRoster->SeekTimeSource(fTimeSourceNode, 0, real);
    if (status != B_OK) 
    {
       timeSource->Release();
       PTRACE(3, "Cannot seek time source!, status " << status);
       return status;
    } 
  }

  bigtime_t perf = timeSource->PerformanceTimeFor(real + latency + initLatency);
  timeSource->Release();
	
  // start the nodes 
  status = fMediaRoster->StartNode(fProducerNode, perf);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't start the video source, status " << status);
    return status;
  }

  status = fMediaRoster->StartNode(fVideoConsumer->Node(), perf);
  if (status != B_OK) 
  {
    PTRACE(3, "Can't start the video consumer, status " << status);
    return status;
  }
	
  return status;
}

void PVideoInputDevice_BeOSVideo::StopNodes()
{
  PTRACE(TL, "StopNodes");
  if (!fMediaRoster)
    return;
	
  if (fVideoConsumer)
  {
    // stop 	
    PTRACE(TL, "Stopping nodes!");
    fMediaRoster->StopNode(fProducerNode, 0, true);
    fMediaRoster->StopNode(fVideoConsumer->Node(), 0, true);
	
    // disconnect 
    fMediaRoster->Disconnect(
        fProducerOut.node.node, fProducerOut.source,
	fConsumerIn.node.node, fConsumerIn.destination);
								
    if(fProducerNode != media_node::null) 
    {
      PTRACE(TL, "Releasing fProducerNode");
      fMediaRoster->ReleaseNode(fProducerNode);
      fProducerNode = media_node::null;
    }

     fMediaRoster->ReleaseNode(fVideoConsumer->Node());		
     fVideoConsumer = NULL;
     
  } // if video consumer
}

// End Of File ///////////////////////////////////////////////////////////////
