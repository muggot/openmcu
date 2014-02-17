//	MediaRecorder.h
//	---------------
//	Copyright 1999, Be Incorporated.   All Rights Reserved.
//	This file may be used under the terms of the Be Sample Code License.

#if !defined(_MEDIA_RECORDER_H)
#define _MEDIA_RECORDER_H

#include <MediaDefs.h>
#include <Locker.h>
#include <MediaNode.h>


namespace BPrivate {
	class BMediaRecorderNode;
};

class BMediaRecorder {
public:

	/*	Should static enumerator helpers go here?	*/

							BMediaRecorder(
									const char * name,
									int32 priority = 0);
		virtual				~BMediaRecorder();
		status_t			InitCheck();

		status_t			SetBufferHook(
									void (*buffer_hook)(
											void * cookie,
											const void * data,
											size_t size,
											const media_header & header),
									void *cookie);

		void				SetCookie(
									void *cookie);

		virtual void		BufferReceived(
									void * data,
									size_t size,
									const media_header & header);

		status_t			Connect(
									const media_format & format,
									uint32 flags = 0);
		status_t			Connect(
									const dormant_node_info & info,
									const media_format * format = 0,
									uint32 flags = 0);
		status_t			Connect(
									const media_node & node,
									const media_output * use_output = 0,
									const media_format * format = 0,
									uint32 flags = 0);
		status_t			Disconnect();
		status_t			Start(
									bool force = false);
		status_t			Stop(
									bool force = false);
		bool				IsRunning();
		bool				IsConnected();

		const media_node &	Node();
		const media_output &Input();
		const media_input &	Output();
		const media_format &Format();

protected:

private:


		//	unimplemented constructors
							BMediaRecorder();
							BMediaRecorder(const BMediaRecorder &);
							BMediaRecorder & operator=(const BMediaRecorder &);

		friend class BPrivate::BMediaRecorderNode;

		status_t			_mInitErr;
		BPrivate::BMediaRecorderNode *_mNode;
		void				(*_mBufferHook)(
									void * cookie,
									void * data,
									size_t size,
									const media_header & header);
		void *				_mBufferCookie;
		media_node			_mInputNode;
		media_output		_mInput;
		media_node			_mOutputNode;
		media_input			_mOutput;
		bool				_mConnected;
		bool				_mRunning;
		BTimeSource *		_mTimeSource;

		status_t			giga_connect(
									const media_format * format,
									uint32 flags,
									const dormant_node_info * dormant,
									const media_node * node,
									const media_output * output);
};


#endif	//	_MEDIA_RECORDER_H
