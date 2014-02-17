//	MediaRecorderNode.h
//	-------------------
//	Copyright 1999, Be Incorporated.   All Rights Reserved.
//	This file may be used under the terms of the Be Sample Code License.

#if !defined(_MEDIA_RECORDER_NODE_H)
#define _MEDIA_RECORDER_NODE_H

#include <MediaEventLooper.h>
#include <BufferConsumer.h>
#include <String.h>

class BMediaRecorder;

namespace BPrivate {

class BMediaRecorderNode : public BMediaEventLooper, public BBufferConsumer {
public:
							BMediaRecorderNode(
									const char * name,
									BMediaRecorder * rec,
									int32 priority);

		//	these are not thread safe; we should fix that...
		void				SetOKFormat(
									const media_format & format);
		status_t			GetInput(
									media_input * out_input);

#if 0
		thread_id			ThreadID()
								{
									return BMediaEventLooper::ControlThread();
								}
#endif
		void				SetDataEnabled(
									bool enabled);

protected:

		virtual	BMediaAddOn*AddOn(
									int32 * internal_id) const;

		virtual void		HandleEvent(
									const media_timed_event *event,
									bigtime_t lateness,
									bool realTimeEvent);

		virtual	status_t	HandleMessage(
									int32 message,
									const void * data,
									size_t size);
		
			/* Someone, probably the producer, is asking you about this format. Give */
			/* your honest opinion, possibly modifying *format. Do not ask upstream */
			/* producer about the format, since he's synchronously waiting for your */
			/* reply. */
		virtual	status_t	AcceptFormat(
									const media_destination & dest,
									media_format * format);
		virtual	status_t	GetNextInput(
									int32 * cookie,
									media_input * out_input);
		virtual	void		DisposeInputCookie(
									int32 cookie);
		virtual	void		BufferReceived(
									BBuffer * buffer);
		virtual	void		ProducerDataStatus(
									const media_destination & for_whom,
									int32 status,
									bigtime_t at_performance_time);
		virtual	status_t	GetLatencyFor(
									const media_destination & for_whom,
									bigtime_t * out_latency,
									media_node_id * out_timesource);
		virtual	status_t	Connected(
									const media_source & producer,
									const media_destination & where,
									const media_format & with_format,
									media_input * out_input);
		virtual	void		Disconnected(
									const media_source & producer,
									const media_destination & where);
		virtual	status_t	FormatChanged(
									const media_source & producer,
									const media_destination & consumer, 
									int32 change_tag,
									const media_format & format);

protected:

		virtual				~BMediaRecorderNode();

		BMediaRecorder *	_mRecorder;
		media_format		_mOKFormat;
		media_input			_mInput;
		BString				_mName;
};

};

using namespace BPrivate;

#endif	//	_MEDIA_RECORDER_NODE_H
