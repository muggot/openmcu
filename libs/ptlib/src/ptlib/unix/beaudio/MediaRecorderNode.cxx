//	MediaRecorderNode.cpp
//	---------------------
//	Copyright 1999, Be Incorporated.   All Rights Reserved.
//	This file may be used under the terms of the Be Sample Code License.

#include "MediaRecorderNode.h"
#include "MediaRecorder.h"
#include <TimedEventQueue.h>
#include <TimeSource.h>
#include <Buffer.h>
#include <stdio.h>


BMediaRecorderNode::BMediaRecorderNode(const char *name, BMediaRecorder * rec, int32 priority) :
	BMediaNode(name),
	BMediaEventLooper(),
	BBufferConsumer(B_MEDIA_UNKNOWN_TYPE),
	_mRecorder(rec)
{
	SetPriority(priority);
	_mInput.destination.id = 1;
	_mInput.destination.port = ControlPort();
	sprintf(_mInput.name, "%.20s Input", name);
	_mName.SetTo(name);
}

BMediaAddOn *
BMediaRecorderNode::AddOn(int32 *internal_id) const
{
	if (internal_id) *internal_id = -1;
	return 0;
}

void 
BMediaRecorderNode::SetOKFormat(const media_format &format)
{
	_mOKFormat = format;
}

status_t
BMediaRecorderNode::GetInput(media_input * out_input)
{
	_mInput.node = Node();
	*out_input = _mInput;
	return B_OK;
}

void 
BMediaRecorderNode::SetDataEnabled(bool enabled)
{
	int32 tag;
	(void)SetOutputEnabled(_mInput.source, _mInput.destination, enabled, 0, &tag);
}

void 
BMediaRecorderNode::HandleEvent(const media_timed_event * /* event */, bigtime_t /* lateness */, bool /* realTimeEvent */)
{
	//	we ignore them all!
}

status_t 
BMediaRecorderNode::HandleMessage(int32 /* message */, const void * /* data */, size_t /* size */)
{
	//	if the message is some secret message, do it here
	return B_ERROR;
}

status_t 
BMediaRecorderNode::AcceptFormat(const media_destination & /* dest */, media_format *format)
{
	if (format_is_compatible(*format, _mOKFormat)) return B_OK;
	*format = _mOKFormat;
	return B_MEDIA_BAD_FORMAT;
}

status_t 
BMediaRecorderNode::GetNextInput(int32 *cookie, media_input *out_input)
{
	if (*cookie == 0) {
		*cookie = -1;
		*out_input = _mInput;
		return B_OK;
	}
	return B_BAD_INDEX;
}

void 
BMediaRecorderNode::DisposeInputCookie(int32 /* cookie */)
{
}

void 
BMediaRecorderNode::BufferReceived(BBuffer *buffer)
{
	_mRecorder->BufferReceived(buffer->Data(), buffer->SizeUsed(), *buffer->Header());
	buffer->Recycle();
}

void 
BMediaRecorderNode::ProducerDataStatus(const media_destination & /* for_whom */, int32 /* status */, bigtime_t /* at_performance_time */)
{
}

status_t 
BMediaRecorderNode::GetLatencyFor(const media_destination & /* for_whom */, bigtime_t *out_latency, media_node_id *out_timesource)
{
	*out_latency = 0;
	*out_timesource = TimeSource()->ID();
	return B_OK;
}

status_t 
BMediaRecorderNode::Connected(const media_source &producer, const media_destination &where, const media_format &with_format, media_input *out_input)
{
	_mInput.source = producer;
	_mInput.format = with_format;
	*out_input = _mInput;
	return B_OK;
}

void 
BMediaRecorderNode::Disconnected(const media_source &producer, const media_destination &where)
{
	_mInput.source = media_source::null;
	sprintf(_mInput.name, "%.20s Input", _mName.String());
}

status_t 
BMediaRecorderNode::FormatChanged(const media_source &producer, const media_destination &consumer, int32 change_tag, const media_format &format)
{
	if (!format_is_compatible(format, _mOKFormat)) {
		return B_MEDIA_BAD_FORMAT;
	}
	_mInput.format = format;
	return B_OK;
}


BMediaRecorderNode::~BMediaRecorderNode()
{
}

