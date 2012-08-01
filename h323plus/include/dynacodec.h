/*
 * dynacodec.h
 *
 * Dynamic codec loading
 *
 * Open H323 Library
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: dynacodec.h,v $
 * Revision 1.1  2007/08/06 20:50:48  shorne
 * First commit of h323plus
 *
 * Revision 1.2  2003/04/30 04:57:13  craigs
 * Changed interface to DLL codec to improve Opal compatibility
 *
 * Revision 1.1  2003/04/27 23:48:24  craigs
 * Initial version
 *
 */

#ifndef _OPALDYNACODEC
#define _OPALDYNACODEC

extern "C" {
#include "dllcodec.h"
};

#include "h323caps.h"

/////////////////////////////////////////////////////////////////////////////

class OpalDLLCodecRec;

class OpalDynaCodecDLL : public PDynaLink
{
  public:
    PCLASSINFO(OpalDynaCodecDLL, PDynaLink);

    OpalDynaCodecDLL(const PFilePath & codec);
    BOOL Load();

    OpalDLLCodecInfo * EnumerateCodecs(unsigned * count);

    static BOOL LoadCodecs();
    static BOOL LoadCodecs(const PDirectory & dir);
    static BOOL LoadCodec(const PFilePath & file);

    static PINDEX AddAudioCapabilities(H323EndPoint & ep,
                                       PINDEX descriptorNum,
                                       PINDEX simultaneousNum,
                                       H323Capabilities & capabilities);

    static PINDEX AddVideoCapabilities(H323EndPoint & ep,
                                       PINDEX descriptorNum,
                                       PINDEX simultaneousNum,
                                       H323Capabilities & capabilities);

    static PINDEX AddCapabilities(H323EndPoint & ep,
                                  PINDEX descriptorNum,
                                  PINDEX simultaneousNum,
                                  H323Capabilities & capabilities,
                                  const PString & type);

    static PDirectory defaultCodecDir;

  protected:
    OpalDLLCodecInfo * (*EnumerateCodecsFn)(unsigned apiVersion, unsigned * count);
    static PMutex mutex;
    static BOOL inited;
    int referenceCount;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDLLCodecRec : public PObject {
  PCLASSINFO(OpalDLLCodecRec, PObject);
  public:
    OpalDLLCodecRec(OpalDynaCodecDLL & _encoder, 
               const PStringToString & _attributes, 
              const OpalDLLCodecInfo & _encoderCodeInfo,
                     OpalMediaFormat * _mediaFormat);

    void * CreateContext() const;
    void DestroyContext(void * context) const;

    H323Capability * CreateCapability(H323EndPoint & ep) const;

    void SetParameter(const PString & attribute, const PString & value) const;
    PString GetParameter(const PString & attribute, const char * defValue = NULL) const;

    OpalDynaCodecDLL & encoder;
    PStringToString    attributes;
    OpalDLLCodecInfo   info;

    OpalMediaFormat  * mediaFormat;
    //H323Capability   * capability;
    //PString            h323Name;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaAudioCodec : public H323FramedAudioCodec
{
  PCLASSINFO(OpalDynaAudioCodec, H323FramedAudioCodec)

  public:
    OpalDynaAudioCodec(const OpalDLLCodecRec & _info, Direction direction);
    ~OpalDynaAudioCodec();
    virtual BOOL EncodeFrame(BYTE * buffer, unsigned & length);
    virtual BOOL DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written);

  protected:
    const OpalDLLCodecRec & info;
    void * context;
    unsigned samplesPerFrame;
    unsigned bytesPerFrame;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaCodecRegistration : public H323CapabilityRegistration
{
  PCLASSINFO(OpalDynaCodecRegistration, H323CapabilityRegistration);
  public:
    OpalDynaCodecRegistration(const PString & name, 
                              OpalDLLCodecRec * _encoderInfo,
                              OpalDLLCodecRec * _decoderInfo,
                              OpalMediaFormat * _mediaFormat);

    H323Capability * Create(H323EndPoint & ep) const;

    friend class OpalDynaCodecDLL;

  protected:
    OpalDLLCodecRec * encoderInfo;
    OpalDLLCodecRec * decoderInfo;
    OpalMediaFormat * mediaFormat;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaCodecNonStandardAudioCapability : public H323NonStandardAudioCapability
{
  PCLASSINFO(OpalDynaCodecNonStandardAudioCapability, H323NonStandardAudioCapability);

  public:
    OpalDynaCodecNonStandardAudioCapability(
      const OpalDLLCodecRec & _info,
      H323EndPoint & _endpoint,
      unsigned maxPacketSize,         /// Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     /// Desired transmit size of an audio packet in frames
      BYTE country,                   /// t35 information
      BYTE extension,                 /// t35 information
      WORD maufacturer,               /// t35 information
      const BYTE * nonstdHeader,      /// nonstandard header
      PINDEX nonstdHeaderLen);

    virtual PObject * Clone() const;
    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const;
    virtual PString GetFormatName() const;

  protected:
    const OpalDLLCodecRec & info;
    H323EndPoint & endpoint;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaNonStandardAudioCodec : public OpalDynaAudioCodec
{
  PCLASSINFO(OpalDynaNonStandardAudioCodec, OpalDynaAudioCodec)

  public:
    OpalDynaNonStandardAudioCodec(const OpalDLLCodecRec & _info, Direction direction);
    ~OpalDynaNonStandardAudioCodec();
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaCodecStandardAudioCapability : public H323AudioCapability
{
  PCLASSINFO(OpalDynaCodecStandardAudioCapability, H323AudioCapability);

  public:
    OpalDynaCodecStandardAudioCapability(
      const OpalDLLCodecRec & _info,
      H323EndPoint & _endpoint,
      unsigned maxPacketSize,         /// Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     /// Desired transmit size of an audio packet in frames
      unsigned subType);

    BOOL OnSendingPDU(H245_AudioCapability & cap, unsigned packetSize) const;
    BOOL OnReceivedPDU(const H245_AudioCapability & cap, unsigned & packetSize);

    PObject * Clone() const;
    H323Codec * CreateCodec(H323Codec::Direction direction) const;
    PString GetFormatName() const;
    unsigned GetSubType() const;

  protected:
    const OpalDLLCodecRec & info;
    H323EndPoint & endpoint;
    unsigned subType;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaCodecStandardVideoCapability : public H323VideoCapability
{
  PCLASSINFO(OpalDynaCodecStandardVideoCapability, H323VideoCapability);

  public:
    OpalDynaCodecStandardVideoCapability(
      const OpalDLLCodecRec & _info,
      H323EndPoint & _endpoint,
      unsigned subType);

    BOOL OnSendingPDU(H245_VideoCapability & pdu) const;
    BOOL OnSendingPDU(H245_VideoMode & pdu) const;
    BOOL OnReceivedPDU(const H245_VideoCapability & pdu);

    PObject * Clone() const;
    H323Codec * CreateCodec(H323Codec::Direction direction) const;
    PString GetFormatName() const;
    unsigned GetSubType() const;

  protected:
    const OpalDLLCodecRec & info;
    H323EndPoint & endpoint;
    unsigned subType;
};

/////////////////////////////////////////////////////////////////////////////

class OpalDynaVideoCodec : public H323VideoCodec
{
  PCLASSINFO(OpalDynaVideoCodec, H323VideoCodec)

  public:
    OpalDynaVideoCodec(
      const OpalDLLCodecRec & _info, Direction direction);

    ~OpalDynaVideoCodec();

    BOOL Read(BYTE * buffer,unsigned & length,RTP_DataFrame & rtpFrame);
    BOOL Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & rtp, unsigned & written);

  protected:
    const OpalDLLCodecRec & info;
    void * context;
};


#endif
