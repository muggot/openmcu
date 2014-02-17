/* 
 *
 * Easy codecs for OpenH323/OPAL
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
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
 * Portions of this Code as Copyright Imtelephone.com. All rights reserved.
 *
 * The Original Code is derived from and used in conjunction with the 
 * OpenH323/OPAL Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 20384 $
 * $Author: rjongbloed $
 * $Date: 2008-06-04 10:14:22 +0000 (Wed, 04 Jun 2008) $
 */
#define HAS_EASYG722    1
//#define HAS_EASYG729A    1
//#define HAS_EASYG728    1
//#define HAS_EASYG7231    1

#ifdef HAS_EASYG722
 #include "g722codec.h"
#endif

#ifdef HAS_EASYG729A
 #include "g729Acodec.h"
#endif

#ifdef HAS_EASYG728
 #include "g728codec.h"
#endif

#ifdef HAS_EASYG7231
#include "g7231codec.h"
#endif

#define Ecodec EasyCodecs


///////////////////////////////////////////////////////////////////////////////////////////////

static void * create_encoder(const struct PluginCodec_Definition * codec)
{

  struct EasySession * session = new EasySession;

  switch ((int)codec->userData) {
    case Plugin_EasyG722:
#ifdef HAS_EASYG722
      if (m_G722codec == NULL)
        m_G722codec = new G722_EasyCodec();
      session->easy = m_G722codec;
#endif
      break;
    case Plugin_EasyG729A:
#ifdef HAS_EASYG729A
      if (m_G729Acodec == NULL)
        m_G729Acodec = new G729A_EasyCodec();
      session->easy = m_G729Acodec;
#endif
      break;
    case Plugin_EasyG728:
#ifdef HAS_EASYG728
      if (m_G728codec == NULL)
        m_G728codec = new G728_EasyCodec();
      session->easy = m_G728codec;
#endif
      break;
    case Plugin_EasyG7231_63:
#ifdef HAS_EASYG7231
      if (m_G7231_63_codec == NULL)
        m_G7231_63_codec = new G7231_63_EasyCodec();
      session->easy = m_G7231_63_codec;
#endif
      break;
  }

  session->hEcoder = session->easy->init_enc();

  return session; 
}


static int codec_encoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  struct EasySession * session = (EasySession *)context;

  if (*fromLen != codec->parm.audio.samplesPerFrame*2)
    return 0;

  session->easy->enc(session->hEcoder, (short *)from, (unsigned char *)to);

  *toLen   = codec->parm.audio.bytesPerFrame;
  *fromLen = codec->parm.audio.samplesPerFrame*2;

  return 1; 
}


static void destroy_encoder(const struct PluginCodec_Definition * codec, void * context)
{
  struct EasySession * session = (EasySession *)context;

  session->easy->release_enc(session->hEcoder);
}


static void * create_decoder(const struct PluginCodec_Definition * codec)
{

 struct EasySession * session = new EasySession;

  switch ((int)codec->userData) {
      case Plugin_EasyG722:
#ifdef HAS_EASYG722
          if (m_G722codec == NULL)
              m_G722codec = new G722_EasyCodec();
        session->easy = m_G722codec;
#endif
          break;
      case Plugin_EasyG729A:
#ifdef HAS_EASYG729A
          if (m_G729Acodec == NULL)
              m_G729Acodec = new G729A_EasyCodec();
        session->easy = m_G729Acodec;
#endif
          break;
      case Plugin_EasyG728:
#ifdef HAS_EASYG728
          if (m_G728codec == NULL)
            m_G728codec = new G728_EasyCodec();
        session->easy = m_G728codec;
#endif
          break;
      case Plugin_EasyG7231_63:
#ifdef HAS_EASYG7231
          if (m_G7231_63_codec == NULL)
            m_G7231_63_codec = new G7231_63_EasyCodec();
        session->easy = m_G7231_63_codec;
#endif
          break;
  }

   session->hDcoder = session->easy->init_dec();

  return session; 
}


static int codec_decoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{

  struct EasySession * session = (EasySession *)context;

  if (*fromLen !=  codec->parm.audio.bytesPerFrame)
    return 0;

   session->easy->dec(session->hDcoder,(unsigned char *)from, (short *)to);

  *toLen   = codec->parm.audio.samplesPerFrame*2;
  *fromLen = codec->parm.audio.bytesPerFrame;

  return 1; 
}


static void destroy_decoder(const struct PluginCodec_Definition * codec, void * context)
{
  struct EasySession * session = (EasySession *)context;

  session->easy->release_dec(session->hDcoder);
}


///////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition EasyCodecDefn[] = {
#ifdef HAS_EASYG722
  DECLARE_EASY_PARAM(EasyG722),
#endif
#ifdef HAS_EASYG729A
  DECLARE_EASY_PARAM(EasyG729A),
#endif
#ifdef HAS_EASYG728
  DECLARE_EASY_PARAM(EasyG728),
#endif
#ifdef HAS_EASYG7231
  DECLARE_EASY_PARAM(EasyG7231_63)
#endif
};

extern "C" {
  PLUGIN_CODEC_IMPLEMENT_ALL(EasyCodec, EasyCodecDefn, PLUGIN_CODEC_VERSION)
};

