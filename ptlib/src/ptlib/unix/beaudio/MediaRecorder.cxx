//	MediaRecorder.cpp
//	-----------------
//	Copyright 1999-2001, Be Incorporated.   All Rights Reserved.
//	This file may be used under the terms of the Be Sample Code License.

#include "MediaRecorder.h"
#include "MediaRecorderNode.h"
#include "NodeRelease.h"
#include <MediaRoster.h>
#include <TimeSource.h>
#include <MediaAddOn.h>


BMediaRecorder::BMediaRecorder(
	const char *name,
	int32 priority) :
	_mInitErr(0),
	_mNode(0),
	_mBufferHook(0),
	_mBufferCookie(0),
	_mConnected(false),
	_mRunning(false),
	_mTimeSource(0)
{
	(void)BMediaRoster::Roster(&_mInitErr);
	if (_mInitErr == B_OK) {
		if (priority < 5) priority = 115;
		_mNode = new BMediaRecorderNode(name, this, priority);
		_mInitErr = BMediaRoster::CurrentRoster()->RegisterNode(_mNode);
		if (_mInitErr == B_OK) {
			_mOutputNode = _mNode->Node();
		}
	}
}

BMediaRecorder::~BMediaRecorder()
{
	if (_mNode != 0) {
		(void)Stop();
		(void)Disconnect();
		_mNode->Release();
	}
	if (_mTimeSource != 0) {
		_mTimeSource->Release();
	}
}

status_t
BMediaRecorder::InitCheck()
{
	return _mInitErr;
}

status_t
BMediaRecorder::SetBufferHook(void( *buffer_hook)( void * cookie, const void *data, size_t size, const media_header &header), void *cookie)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (_mRunning) {
		return EPERM;
	}
	_mBufferHook = buffer_hook;
	_mBufferCookie = cookie;
	return B_OK;
}

void
BMediaRecorder::SetCookie(void *cookie)
{
	_mBufferCookie = cookie;
}

void 
BMediaRecorder::BufferReceived(void *data, size_t size, const media_header &header)
{
	if (_mBufferHook && _mRunning) {
		(*_mBufferHook)(_mBufferCookie, data, size, header);
	}
}

status_t 
BMediaRecorder::Connect(const media_format &format, uint32 flags)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (_mConnected) {
		return B_MEDIA_ALREADY_CONNECTED;
	}
	return giga_connect(&format, flags, 0, 0, 0);
}

status_t 
BMediaRecorder::Connect(const dormant_node_info &info, const media_format *format, uint32 flags)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (_mConnected) {
		return B_MEDIA_ALREADY_CONNECTED;
	}
	return giga_connect(format, flags, &info, 0, 0);
}

status_t 
BMediaRecorder::Connect(const media_node &node, const media_output *use_output, const media_format *format, uint32 flags)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (_mConnected) {
		return B_MEDIA_ALREADY_CONNECTED;
	}
	return giga_connect(format, flags, 0, &node, use_output);
}

status_t 
BMediaRecorder::Disconnect()
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (!_mConnected) {
		return B_MEDIA_NOT_CONNECTED;
	}
	if (!_mNode) {
		return B_ERROR;
	}

	//	do the disconnect
	status_t err = BMediaRoster::CurrentRoster()->Disconnect(
			_mInputNode.node, _mInput.source, _mOutputNode.node, _mOutput.destination);

	if (_mTimeSource != 0) {
		_mTimeSource->Release();
		_mTimeSource = 0;
	}

	_mConnected = false;
	_mRunning = false;

	return err;
}

status_t 
BMediaRecorder::Start(bool force)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (!_mConnected) {
		return B_MEDIA_NOT_CONNECTED;
	}
	if (_mRunning && !force) {
		return EALREADY;
	}
	if (!_mNode) {
		return B_ERROR;
	}
	//	start node here
	status_t err = B_OK;
	if (_mInputNode.kind & B_TIME_SOURCE) {
		err = BMediaRoster::CurrentRoster()->StartTimeSource(_mInputNode, BTimeSource::RealTime());
	}
	else {
		err = BMediaRoster::CurrentRoster()->StartNode(_mInputNode, _mTimeSource->Now());
	}
	//	then un-mute it
	if (err == B_OK) {
		_mNode->SetDataEnabled(true);
	}
	_mRunning = (err == B_OK);
	return err;
}

status_t 
BMediaRecorder::Stop(bool force)
{
	if (_mInitErr < B_OK) {
		return _mInitErr;
	}
	if (!_mRunning && !force) {
		return EALREADY;
	}
	if (!_mNode) {
		return B_ERROR;
	}
	//	should have the Node mute the output here
	_mNode->SetDataEnabled(false);

	_mRunning = false;
	return B_OK;
}

bool
BMediaRecorder::IsRunning()
{
	return _mRunning;
}

bool
BMediaRecorder::IsConnected()
{
	return _mConnected;
}

const media_node &
BMediaRecorder::Node()
{
	return _mOutputNode;
}

const media_output &
BMediaRecorder::Input()
{
	return _mInput;
}

const media_input &
BMediaRecorder::Output()
{
	return _mOutput;
}

const media_format & 
BMediaRecorder::Format()
{
	return _mInput.format;
}


status_t 
BMediaRecorder::giga_connect( const media_format *in_format, uint32 in_flags, const dormant_node_info *in_dormant, const media_node *in_node, const media_output *in_output)
{
	media_format fmt;
	media_node node;
	StNodeRelease away(node, false);
	status_t err = B_OK;
	media_output out;

	//	argument checking and set-up
	if (in_format != 0) fmt = *in_format;
	if (_mNode == 0) return B_ERROR;
	if ((in_node == 0) && (in_output != 0)) return B_MISMATCHED_VALUES;
	if ((in_dormant != 0) && ((in_node != 0) || (in_output != 0))) return B_MISMATCHED_VALUES;
	//if ((in_format == 0) && (in_output != 0)) fmt = in_output->format;

	_mNode->SetOKFormat(fmt);

	//	figure out the node
	//	instantiate?
	if (in_dormant != 0) {
		err = BMediaRoster::Roster()->InstantiateDormantNode(*in_dormant, &node, B_FLAVOR_IS_GLOBAL);
		away.SetRelease(true);
	}
	//	provided?
	else if (in_node != 0) {
		node = *in_node;
	}
	//	switch on format for default?
	else switch (fmt.type) {
	case B_MEDIA_RAW_AUDIO:
		err = BMediaRoster::Roster()->GetAudioInput(&node);
		away.SetRelease(true);
		break;
	case B_MEDIA_RAW_VIDEO:
	case B_MEDIA_ENCODED_VIDEO:
		err = BMediaRoster::Roster()->GetVideoInput(&node);
		away.SetRelease(true);
		break;
	//	give up?
	default:
		return B_MEDIA_BAD_FORMAT;
		break;
	}

	_mInputNode = node;

	//	figure out the output
	//	provided?
	if (in_output != 0) {
		out = *in_output;
	}
	//	iterate?
	else if (err == B_OK) {
		media_output outputs[10];
		int32 count = 10;
		err = BMediaRoster::Roster()->GetFreeOutputsFor(node, outputs, count, &count, fmt.type);
		if (err == B_OK) {
			err = B_MEDIA_BAD_FORMAT;
			for (int ix=0; ix<count; ix++) {
				if (format_is_compatible(outputs[ix].format, fmt)) {
					out = outputs[ix];
					err = B_OK;
					fmt = outputs[ix].format;
					break;
				}
			}
		}
	}
	//	give up?
	if (err != B_OK) {
		return err;
	}
	if (out.source == media_source::null) {
		return B_MEDIA_BAD_SOURCE;
	}

	//	find our Node's free input
	media_input in;
	err = _mNode->GetInput(&in);
	media_node time_source;

	if (node.kind & B_TIME_SOURCE) {
		time_source = node;
	}
	else {
		BMediaRoster::Roster()->GetSystemTimeSource(&time_source);
	}

	//	set time source
	if (err == B_OK) {
		BMediaRoster::Roster()->SetTimeSourceFor(_mOutputNode.node, time_source.node);
		_mTimeSource = BMediaRoster::CurrentRoster()->MakeTimeSourceFor(_mOutputNode);
	}

	//	start the recorder node (it's always running)
	if (err == B_OK) {
		err = BMediaRoster::CurrentRoster()->StartNode(_mOutputNode, _mTimeSource->Now());
	}

	//	perform the connection
	if (err == B_OK) {
		_mInput = out;
		_mOutput = in;
		err = BMediaRoster::CurrentRoster()->Connect(_mInput.source,
			_mOutput.destination, &fmt, &_mInput, &_mOutput, BMediaRoster::B_CONNECT_MUTED);
		if (err == B_OK) {
			_mConnected = true;
			away.SetRelease(false);
		}
	}
	return err;
}

