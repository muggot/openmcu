/*
 * rtsp.h
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 *
 * The Initial Developer of the Original Code is Andrey Burbovskiy (andrewb@yandex.ru), All Rights Reserved
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
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *
 */

#include "precompile.h"

#ifndef _MCU_RTSP_H
#define _MCU_RTSP_H

#include "sockets.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static const PString METHOD_OPTIONS    = "OPTIONS";
static const PString METHOD_DESCRIBE   = "DESCRIBE";
static const PString METHOD_SETUP      = "SETUP";
static const PString METHOD_PLAY       = "PLAY";
static const PString METHOD_TEARDOWN   = "TEARDOWN";

class ConferenceStreamMember;

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCURtspConnection : public MCUSipConnection
{
  friend class MCURtspServer;

  public:
    virtual BOOL ClearCall(CallEndReason reason = EndedByLocalUser);
    virtual void CleanUpOnCallEnd();

  protected:

    enum RtspStates
    {
      RTSP_NONE = 0,
      RTSP_CONNECT,
      RTSP_DESCRIBE,
      RTSP_SETUP,
      RTSP_SETUP_AUDIO,
      RTSP_SETUP_VIDEO,
      RTSP_PLAY,
      RTSP_PLAYING,
      RTSP_TEARDOWN
    };
    RtspStates rtsp_state;

    MCURtspConnection(MCUH323EndPoint *_ep, PString _callToken);
    ~MCURtspConnection();

    BOOL Connect(PString room, PString address);
    BOOL Connect(MCUSocket *socket, const msg_t *msg);

    void CreateLocalSipCaps();
    BOOL CreateInboundCaps();

    BOOL SendSetup(MediaTypes mtype, int pt);
    BOOL SendPlay();
    BOOL SendOptions();
    BOOL SendTeardown();
    BOOL SendDescribe();

    BOOL OnResponseReceived(const msg_t *msg);
    BOOL OnResponseDescribe(const msg_t *msg);
    BOOL OnResponseSetup(const msg_t *msg);
    BOOL OnResponsePlay(const msg_t *msg);

    BOOL OnRequestReceived(const msg_t *msg);
    BOOL OnRequestDescribe(const msg_t *msg);
    BOOL OnRequestSetup(const msg_t *msg);
    BOOL OnRequestPlay(const msg_t *msg);
    BOOL OnRequestTeardown(const msg_t *msg);
    BOOL OnRequestOptions(const msg_t *msg);

    BOOL RtspCheckAuth(const msg_t *msg);
    BOOL ParseTransportStr(SipCapability *sc, PString & transport_str);
    void AddHeaders(char *buffer, PString method_name="");
    BOOL SendRequest(char *buffer);

    int cseq;
    PString rtsp_session_str;
    PString rtsp_path;
    PString luri_str;

    static int OnReceived_wrap(void *context, MCUSocket *socket, PString data)
    { return ((MCURtspConnection *)context)->OnReceived(socket, data); }
    int OnReceived(MCUSocket *socket, PString data);

    MCUListener *listener;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCURtspServer
{
  public:
    MCURtspServer(MCUH323EndPoint *ep, MCUSipEndPoint *sep);
    ~MCURtspServer();

    bool CreateConnection(const PString & room, const PString & address, const PString & callToken);

    void StartListeners();
    void RemoveListeners();
    void AddListener(const PString & address);
    BOOL HasListener(const PString & host, const PString & port);

  protected:

    BOOL CreateConnection(MCUSocket *socket, const msg_t *msg);
    void SendResponse(MCUSocket *socket, const msg_t *msg, const PString & status_str);

    PString trace_section;

    static int OnReceived_wrap(void *context, MCUSocket *socket, PString data)
    { return ((MCURtspServer *)context)->OnReceived(socket, data); }
    int OnReceived(MCUSocket *socket, PString data);

    MCUListenerList listenerList;

    MCUH323EndPoint *ep;
    MCUSipEndPoint *sep;

    PMutex rtspMutex;
    PMutex listenerListMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceStreamMember : public ConferenceMember
{
  PCLASSINFO(ConferenceStreamMember, ConferenceMember);

  public:
    ConferenceStreamMember(Conference *_conference, const PString & _name, const PString & _callToken)
      : ConferenceMember(_conference)
    {
      memberType = MEMBER_TYPE_STREAM;
      callToken = _callToken;
      name = _name;
      nameID = MCUURL(name).GetMemberNameId();
    }
    ~ConferenceStreamMember()
    {
    }

    virtual PString GetName() const
    { return name; }

};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTSP_H
