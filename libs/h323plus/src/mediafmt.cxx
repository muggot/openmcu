/*
 * mediafmt.cxx
 *
 * Media Format descriptions
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
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
 * $Log: mediafmt.cxx,v $
 * Revision 1.3  2007/10/19 19:54:18  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.2  2007/08/20 19:13:29  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.30.2.5  2007/10/03 13:10:00  rjongbloed
 * Removed duplicate OpalMediaFormat list caused by merge clash from OPAL
 *
 * Revision 1.30.2.4  2007/08/31 05:30:21  rjongbloed
 * Fixed missing part of back port from OPAL, allow signed/unsigned integer media options.
 *
 * Revision 1.30.2.3  2007/08/17 08:38:23  rjongbloed
 * Back ported OPAL meda options based plug ins and H.323 generic capabilties.
 *
 * Revision 1.30.2.2  2007/02/11 00:29:35  shorne
 * Fix for broken Audio
 *
 * Revision 1.30.2.1  2006/12/23 19:08:03  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.30  2006/09/05 23:56:57  csoutheren
 * Convert media format and capability factories to use std::string
 *
 * Revision 1.29  2005/01/11 07:12:13  csoutheren
 * Fixed namespace collisions with plugin starup factories
 *
 * Revision 1.28  2005/01/04 12:20:12  csoutheren
 * Fixed more problems with global statics
 *
 * Revision 1.27  2004/06/30 12:31:16  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.26  2004/06/18 03:06:00  csoutheren
 * Changed dynamic payload type allocation code to avoid needless renumbering of
 * media formats when new formats are created
 *
 * Revision 1.25  2004/06/18 02:24:46  csoutheren
 * Fixed allocation of dynamic RTP payload types as suggested by Guilhem Tardy
 *
 * Revision 1.24  2004/06/03 13:32:01  csoutheren
 * Renamed INSTANTIATE_FACTORY
 *
 * Revision 1.23  2004/06/03 12:48:35  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with DLLs
 *
 * Revision 1.22  2004/06/01 05:50:32  csoutheren
 * Increased usage of typedef'ed factory rather than redefining
 *
 * Revision 1.21  2004/05/23 12:49:34  rjongbloed
 * Tidied some of the OpalMediaFormat usage after abandoning some previous
 *   code due to MSVC6 compiler bug.
 *
 * Revision 1.20  2004/05/20 02:07:29  csoutheren
 * Use macro to work around MSVC internal compiler errors
 *
 * Revision 1.19  2004/05/19 09:48:35  csoutheren
 * Fixed problem with non-RTP media formats causing endless loops
 *
 * Revision 1.18  2004/05/19 07:38:24  csoutheren
 * Changed OpalMediaFormat handling to use abstract factory method functions
 *
 * Revision 1.17  2004/05/05 09:40:05  csoutheren
 * OpalMediaFormat.Clone() does not exist - use copy constructor instead
 *
 * Revision 1.16  2004/05/03 00:52:24  csoutheren
 * Fixed problem with OpalMediaFormat::GetMediaFormatsList
 * Added new version of OpalMediaFormat::GetMediaFormatsList that minimses copying
 *
 * Revision 1.15  2004/04/03 10:38:25  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.14.2.1  2004/03/31 11:11:59  csoutheren
 * Initial public release of plugin codec code
 *
 * Revision 1.14  2004/02/26 23:41:22  csoutheren
 * Fixed multi-threading problem
 *
 * Revision 1.13  2004/02/26 11:45:44  csoutheren
 * Fixed problem with OpalMediaFormat failing incorrect reason
 *
 * Revision 1.12  2004/02/26 08:19:32  csoutheren
 * Fixed threading problem with GetMediaFormatList
 *
 * Revision 1.11  2002/12/03 09:20:01  craigs
 * Fixed problem with RFC2833 and a dynamic RTP type using the same RTP payload number
 *
 * Revision 1.10  2002/12/02 03:06:26  robertj
 * Fixed over zealous removal of code when NO_AUDIO_CODECS set.
 *
 * Revision 1.9  2002/10/30 05:54:17  craigs
 * Fixed compatibilty problems with G.723.1 6k3 and 5k3
 *
 * Revision 1.8  2002/08/05 10:03:48  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.7  2002/06/25 08:30:13  robertj
 * Changes to differentiate between stright G.723.1 and G.723.1 Annex A using
 *   the OLC dataType silenceSuppression field so does not send SID frames
 *   to receiver codecs that do not understand them.
 *
 * Revision 1.6  2002/01/22 07:08:26  robertj
 * Added IllegalPayloadType enum as need marker for none set
 *   and MaxPayloadType is a legal value.
 *
 * Revision 1.5  2001/12/11 04:27:28  craigs
 * Added support for 5.3kbps G723.1
 *
 * Revision 1.4  2001/09/21 02:51:45  robertj
 * Implemented static object for all "known" media formats.
 * Added default session ID to media format description.
 *
 * Revision 1.3  2001/05/11 04:43:43  robertj
 * Added variable names for standard PCM-16 media format name.
 *
 * Revision 1.2  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.1  2001/01/25 07:27:16  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "mediafmt.h"
#endif

#include "mediafmt.h"
#include "rtp.h"
#include "h323pluginmgr.h"


#include <ptclib/cypher.h>

namespace PWLibStupidLinkerHacks {
  extern int h323Loader;
};

static class PMediaFormatInstantiateMe
{
  public:
    PMediaFormatInstantiateMe()
    { PWLibStupidLinkerHacks::h323Loader = 1; }
} instance;


/////////////////////////////////////////////////////////////////////////////

char OpalPCM16[] = OPAL_PCM16;

OPAL_MEDIA_FORMAT_DECLARE(OpalPCM16Format, 
          OpalPCM16,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::L16_Mono,
          TRUE,   // Needs jitter
          128000, // bits/sec
          16, // bytes/frame
          8, // 1 millisecond
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG711uLaw64k[] = OPAL_G711_ULAW_64K;

OPAL_MEDIA_FORMAT_DECLARE(OpalG711uLaw64kFormat,
          OpalG711uLaw64k,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::PCMU,
          TRUE,   // Needs jitter
          64000, // bits/sec
          8, // bytes/frame
          8, // 1 millisecond/frame
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG711ALaw64k[] = OPAL_G711_ALAW_64K;

OPAL_MEDIA_FORMAT_DECLARE(OpalG711ALaw64kFormat,
          OpalG711ALaw64k,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::PCMA,
          TRUE,   // Needs jitter
          64000, // bits/sec
          8, // bytes/frame
          8, // 1 millisecond/frame
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG728[] = OPAL_G728;

OPAL_MEDIA_FORMAT_DECLARE(OpalG728Format,
          OpalG728,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G728,
          TRUE, // Needs jitter
          16000,// bits/sec
          5,    // bytes
          20,   // 2.5 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG729[] = OPAL_G729;

OPAL_MEDIA_FORMAT_DECLARE(OpalG729Format,
          OpalG729,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G729,
          TRUE, // Needs jitter
          8000, // bits/sec
          10,   // bytes
          80,   // 10 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG729A[] = OPAL_G729A;

OPAL_MEDIA_FORMAT_DECLARE(OpalG729AFormat,
          OpalG729A,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G729,
          TRUE, // Needs jitter
          8000, // bits/sec
          10,   // bytes
          80,   // 10 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG729B[] = OPAL_G729B;

OPAL_MEDIA_FORMAT_DECLARE(OpalG729BFormat,
          OpalG729B,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G729,
          TRUE, // Needs jitter
          8000, // bits/sec
          10,   // bytes
          80,   // 10 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG729AB[] = OPAL_G729AB;

OPAL_MEDIA_FORMAT_DECLARE(OpalG729ABFormat,
          OpalG729AB,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G729,
          TRUE, // Needs jitter
          8000, // bits/sec
          10,   // bytes
          80,   // 10 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG7231_6k3[] = OPAL_G7231_6k3;

OPAL_MEDIA_FORMAT_DECLARE(OpalG7231_6k3Format,
          OpalG7231_6k3,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G7231,
          TRUE, // Needs jitter
          6400, // bits/sec
          24,   // bytes
          240,  // 30 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG7231_5k3[] = OPAL_G7231_5k3;

OPAL_MEDIA_FORMAT_DECLARE(OpalG7231_5k3Format,
          OpalG7231_5k3,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G7231,
          TRUE, // Needs jitter
          5300, // bits/sec
          24,   // bytes
          240,  // 30 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG7231A_6k3[] = OPAL_G7231A_6k3;

OPAL_MEDIA_FORMAT_DECLARE(OpalG7231A_6k3Format,
          OpalG7231A_6k3,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G7231,
          TRUE, // Needs jitter
          6400, // bits/sec
          24,   // bytes
          240,  // 30 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalG7231A_5k3[] = OPAL_G7231A_5k3;

OPAL_MEDIA_FORMAT_DECLARE(OpalG7231A_5k3Format,
          OpalG7231A_5k3,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::G7231,
          TRUE, // Needs jitter
          5300, // bits/sec
          24,   // bytes
          240,  // 30 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalGSM0610[] = OPAL_GSM0610;

OPAL_MEDIA_FORMAT_DECLARE(OpalGSM0610Format,
          OpalGSM0610,
          OpalMediaFormat::DefaultAudioSessionID,
          RTP_DataFrame::GSM,
          TRUE,  // Needs jitter
          13200, // bits/sec
          33,    // bytes
          160,   // 20 milliseconds
          OpalMediaFormat::AudioTimeUnits,
          0)

/////////////////////////////////////////////////////////////////////////////

char OpalT120[] = "T.120";

OPAL_MEDIA_FORMAT_DECLARE(OpalT120Format,
          OpalT120,
          OpalMediaFormat::DefaultDataSessionID,
          RTP_DataFrame::IllegalPayloadType,
          FALSE,   // No jitter for data
          825000,   // 100's bits/sec
          0,0,0,0)

/////////////////////////////////////////////////////////////////////////////

OpalMediaOption::OpalMediaOption(const char * name, bool readOnly, MergeType merge)
  : m_name(name),
    m_readOnly(readOnly),
    m_merge(merge)
{
  m_name.Replace("=", "_", TRUE);
  memset(&m_H245Generic, 0, sizeof(m_H245Generic));
}


PObject::Comparison OpalMediaOption::Compare(const PObject & obj) const
{
  const OpalMediaOption * otherOption = PDownCast(const OpalMediaOption, &obj);
  if (otherOption == NULL)
    return GreaterThan;
  return m_name.Compare(otherOption->m_name);
}


bool OpalMediaOption::Merge(const OpalMediaOption & option)
{
  switch (m_merge) {
    case MinMerge :
      if (CompareValue(option) == GreaterThan)
        Assign(option);
      break;

    case MaxMerge :
      if (CompareValue(option) == LessThan)
        Assign(option);
      break;

    case EqualMerge :
      return CompareValue(option) == EqualTo;

    case NotEqualMerge :
      return CompareValue(option) != EqualTo;
      
    case AlwaysMerge :
      Assign(option);
      break;

    default :
      break;
  }

  return true;
}


PString OpalMediaOption::AsString() const
{
  PStringStream strm;
  PrintOn(strm);
  return strm;
}


bool OpalMediaOption::FromString(const PString & value)
{
  PStringStream strm;
  strm = value;
  ReadFrom(strm);
  return !strm.fail();
}


OpalMediaOptionEnum::OpalMediaOptionEnum(const char * name,
                                         bool readOnly,
                                         const char * const * enumerations,
                                         PINDEX count,
                                         MergeType merge,
                                         PINDEX value)
  : OpalMediaOption(name, readOnly, merge),
    m_enumerations(count, enumerations),
    m_value(value)
{
  if (m_value >= count)
    m_value = count;
}


PObject * OpalMediaOptionEnum::Clone() const
{
  return new OpalMediaOptionEnum(*this);
}


void OpalMediaOptionEnum::PrintOn(ostream & strm) const
{
  if (m_value < m_enumerations.GetSize())
    strm << m_enumerations[m_value];
  else
    strm << m_value;
}


void OpalMediaOptionEnum::ReadFrom(istream & strm)
{
  PCaselessString str;
  while (strm.good()) {
    char ch;
    strm.get(ch);
    str += ch;
    for (PINDEX i = 0; i < m_enumerations.GetSize(); i++) {
      if (str == m_enumerations[i]) {
        m_value = i;
        return;
      }
    }
  }

  m_value = m_enumerations.GetSize();

#ifdef __USE_STL__
   strm.setstate(ios::badbit);
#else
   strm.setf(ios::badbit , ios::badbit);
#endif
}


PObject::Comparison OpalMediaOptionEnum::CompareValue(const OpalMediaOption & option) const
{
  const OpalMediaOptionEnum * otherOption = PDownCast(const OpalMediaOptionEnum, &option);
  if (otherOption == NULL)
    return GreaterThan;

  if (m_value > otherOption->m_value)
    return GreaterThan;

  if (m_value < otherOption->m_value)
    return LessThan;

  return EqualTo;
}


void OpalMediaOptionEnum::Assign(const OpalMediaOption & option)
{
  const OpalMediaOptionEnum * otherOption = PDownCast(const OpalMediaOptionEnum, &option);
  if (otherOption != NULL)
    m_value = otherOption->m_value;
}


void OpalMediaOptionEnum::SetValue(PINDEX value)
{
  if (value < m_enumerations.GetSize())
    m_value = value;
  else
    m_value = m_enumerations.GetSize();
}


OpalMediaOptionString::OpalMediaOptionString(const char * name, bool readOnly)
  : OpalMediaOption(name, readOnly, MinMerge)
{
}


OpalMediaOptionString::OpalMediaOptionString(const char * name, bool readOnly, const PString & value)
  : OpalMediaOption(name, readOnly, MinMerge),
    m_value(value)
{
}


PObject * OpalMediaOptionString::Clone() const
{
  OpalMediaOptionString * newObj = new OpalMediaOptionString(*this);
  newObj->m_value.MakeUnique();
  return newObj;
}


void OpalMediaOptionString::PrintOn(ostream & strm) const
{
  strm << m_value.ToLiteral();
}


void OpalMediaOptionString::ReadFrom(istream & strm)
{
  char c;
  strm >> c; // Skip whitespace

  if (c != '"') {
    strm.putback(c);
    strm >> m_value; // If no " then read to end of line.
  }
  else {
    // If there was a '"' then assume it is a C style literal string with \ escapes etc

    PINDEX count = 0;
    PStringStream str;
    str << '"';

    while (strm.get(c).good()) {
      str << c;

      // Keep reading till get a '"' that is not preceded by a '\' that is not itself preceded by a '\'
      if (c == '"' && count > 0 && (str[count] != '\\' || !(count > 1 && str[count-1] == '\\')))
        break;

      count++;
    }

    m_value = PString(PString::Literal, (const char *)str);
  }
}


PObject::Comparison OpalMediaOptionString::CompareValue(const OpalMediaOption & option) const
{
  const OpalMediaOptionString * otherOption = PDownCast(const OpalMediaOptionString, &option);
  if (otherOption == NULL)
    return GreaterThan;

  return m_value.Compare(otherOption->m_value);
}


void OpalMediaOptionString::Assign(const OpalMediaOption & option)
{
  const OpalMediaOptionString * otherOption = PDownCast(const OpalMediaOptionString, &option);
  if (otherOption != NULL) {
    m_value = otherOption->m_value;
    m_value.MakeUnique();
  }
}


void OpalMediaOptionString::SetValue(const PString & value)
{
  m_value = value;
  m_value.MakeUnique();
}


OpalMediaOptionOctets::OpalMediaOptionOctets(const char * name, bool readOnly, bool base64)
  : OpalMediaOption(name, readOnly, NoMerge)
  , m_base64(base64)
{
}


OpalMediaOptionOctets::OpalMediaOptionOctets(const char * name, bool readOnly, bool base64, const PBYTEArray & value)
  : OpalMediaOption(name, readOnly, NoMerge)
  , m_value(value)
  , m_base64(base64)
{
}


OpalMediaOptionOctets::OpalMediaOptionOctets(const char * name, bool readOnly, bool base64, const BYTE * data, PINDEX length)
  : OpalMediaOption(name, readOnly, NoMerge)
  , m_value(data, length)
  , m_base64(base64)
{
}


PObject * OpalMediaOptionOctets::Clone() const
{
  OpalMediaOptionOctets * newObj = new OpalMediaOptionOctets(*this);
  newObj->m_value.MakeUnique();
  return newObj;
}


void OpalMediaOptionOctets::PrintOn(ostream & strm) const
{
  if (m_base64)
    strm << PBase64::Encode(m_value);
  else {
    _Ios_Fmtflags flags = strm.flags();
    char fill = strm.fill();

    strm << hex << setfill('0');
    for (PINDEX i = 0; i < m_value.GetSize(); i++)
      strm << setw(2) << (unsigned)m_value[i];

    strm.fill(fill);
    strm.flags(flags);
  }
}


void OpalMediaOptionOctets::ReadFrom(istream & strm)
{
  if (m_base64) {
    PString str;
    strm >> str;
    PBase64::Decode(str, m_value);
  }
  else {
    char pair[3];
    pair[2] = '\0';

    PINDEX count = 0;

    while (isxdigit(strm.peek())) {
      pair[0] = (char)strm.get();
      if (!isxdigit(strm.peek())) {
        strm.putback(pair[0]);
        break;
      }
      pair[1] = (char)strm.get();
      if (!m_value.SetMinSize((count+1+99)%100))
        break;
      m_value[count++] = (BYTE)strtoul(pair, NULL, 16);
    }

    m_value.SetSize(count);
  }
}


PObject::Comparison OpalMediaOptionOctets::CompareValue(const OpalMediaOption & option) const
{
  const OpalMediaOptionOctets * otherOption = PDownCast(const OpalMediaOptionOctets, &option);
  if (otherOption == NULL)
    return GreaterThan;

  return m_value.Compare(otherOption->m_value);
}


void OpalMediaOptionOctets::Assign(const OpalMediaOption & option)
{
  const OpalMediaOptionOctets * otherOption = PDownCast(const OpalMediaOptionOctets, &option);
  if (otherOption != NULL) {
    m_value = otherOption->m_value;
    m_value.MakeUnique();
  }
}


void OpalMediaOptionOctets::SetValue(const PBYTEArray & value)
{
  m_value = value;
  m_value.MakeUnique();
}


void OpalMediaOptionOctets::SetValue(const BYTE * data, PINDEX length)
{
  m_value = PBYTEArray(data, length);
}


/////////////////////////////////////////////////////////////////////////////

OpalMediaFormat::OpalMediaFormat()
{
  rtpPayloadType = RTP_DataFrame::IllegalPayloadType;

  needsJitter = FALSE;
  bandwidth = 0;
  frameSize = 0;
  frameTime = 0;
  timeUnits = 0;
  codecBaseTime = 0;
  defaultSessionID = 0;
}


OpalMediaFormat::OpalMediaFormat(const char * search, BOOL exact)
{
  rtpPayloadType = RTP_DataFrame::IllegalPayloadType;

  needsJitter = FALSE;
  bandwidth = 0;
  frameSize = 0;
  frameTime = 0;
  timeUnits = 0;
  codecBaseTime = 0;
  defaultSessionID = 0; 

  // look for the media type in the factory. 
  // Don't make a copy of the list - lock the list and use the raw data
  if (exact) {
    OpalMediaFormat * registeredFormat = OpalMediaFormatFactory::CreateInstance(search);
    if (registeredFormat != NULL)
      *this = *registeredFormat;
  }
  else {
    PWaitAndSignal m(OpalMediaFormatFactory::GetMutex());
    OpalMediaFormatFactory::KeyMap_T & keyMap = OpalMediaFormatFactory::GetKeyMap();
    OpalMediaFormatFactory::KeyMap_T::const_iterator r;
    for (r = keyMap.begin(); r != keyMap.end(); ++r) {
      if (r->first.find(search) != std::string::npos) {
        *this = *OpalMediaFormatFactory::CreateInstance(r->first);
        break;
      }
    }
  }
}


OpalMediaFormat::OpalMediaFormat(const char * fullName,
                                 unsigned dsid,
                                 RTP_DataFrame::PayloadTypes pt,
                                 BOOL     nj,
                                 unsigned bw,
                                 PINDEX   fs,
                                 unsigned ft,
                                 unsigned tu,
                                 time_t ts)
  : PCaselessString(fullName)
{
  rtpPayloadType = pt;
  defaultSessionID = dsid;
  needsJitter = nj;
  bandwidth = bw;
  frameSize = fs;
  frameTime = ft;
  timeUnits = tu;
  codecBaseTime = ts;

  // assume non-dynamic payload types are correct and do not need deconflicting
  if (rtpPayloadType < RTP_DataFrame::DynamicBase || rtpPayloadType == RTP_DataFrame::IllegalPayloadType)
    return;

  // find the next unused dynamic number, and find anything with the new 
  // rtp payload type if it is explicitly required
  PWaitAndSignal m(OpalMediaFormatFactory::GetMutex());
  OpalMediaFormatFactory::KeyMap_T & keyMap = OpalMediaFormatFactory::GetKeyMap();

  OpalMediaFormat * match = NULL;
  RTP_DataFrame::PayloadTypes nextUnused = RTP_DataFrame::DynamicBase;
  OpalMediaFormatFactory::KeyMap_T::iterator r;

  do {
    for (r = keyMap.begin(); r != keyMap.end(); ++r) {
      if (r->first == fullName)
        continue;
      OpalMediaFormat & fmt = *OpalMediaFormatFactory::CreateInstance(r->first);
      if (fmt.GetPayloadType() == nextUnused) {
        nextUnused = (RTP_DataFrame::PayloadTypes)(nextUnused + 1);
        break; // restart the search
      }
      if (fmt.GetPayloadType() == rtpPayloadType)
        match = &fmt;
    }
  } while (r != keyMap.end());

  // If we found a match to the payload type, then it needs to be deconflicted
  // If the new format is just requesting any dynamic payload number, then give it the next unused one
  // If it is requesting a specific code (like RFC 2833) then renumber the old format. This could be dangerous
  // as media formats could be created when there is a session in use with the old media format and payload type
  // For this reason, any media formats that require specific dynamic codes should be created before any calls are made
  if (match != NULL) {
    if (rtpPayloadType == RTP_DataFrame::DynamicBase)
      rtpPayloadType = nextUnused;
    else 
      match->rtpPayloadType = nextUnused;
  }
}

OpalMediaFormat & OpalMediaFormat::operator=(const OpalMediaFormat &format)
{
  PWaitAndSignal m1(media_format_mutex);
  PWaitAndSignal m2(format.media_format_mutex);
  *static_cast<PCaselessString *>(this) = *static_cast<const PCaselessString *>(&format);
  options = format.options;
  options.MakeUnique();
  rtpPayloadType = format.rtpPayloadType;
  defaultSessionID = format.defaultSessionID;
  needsJitter = format.NeedsJitterBuffer();
  bandwidth = format.GetBandwidth();
  frameSize = format.GetFrameSize();
  frameTime = format.GetFrameTime();
  timeUnits = format.GetTimeUnits();
  return *this;  
}


void OpalMediaFormat::GetRegisteredMediaFormats(OpalMediaFormat::List & list)
{
  list.DisallowDeleteObjects();
  PWaitAndSignal m(OpalMediaFormatFactory::GetMutex());
  OpalMediaFormatFactory::KeyMap_T & keyMap = OpalMediaFormatFactory::GetKeyMap();
  OpalMediaFormatFactory::KeyMap_T::const_iterator r;
  for (r = keyMap.begin(); r != keyMap.end(); ++r)
    list.Append(OpalMediaFormatFactory::CreateInstance(r->first));
}


OpalMediaFormat::List OpalMediaFormat::GetRegisteredMediaFormats()
{
  OpalMediaFormat::List list;
  GetRegisteredMediaFormats(list);
  return list;
}


bool OpalMediaFormat::Merge(const OpalMediaFormat & mediaFormat)
{
  PWaitAndSignal m1(media_format_mutex);
  PWaitAndSignal m2(mediaFormat.media_format_mutex);
  for (PINDEX i = 0; i < options.GetSize(); i++) {
    OpalMediaOption * option = mediaFormat.FindOption(options[i].GetName());
    if (option != NULL && !options[i].Merge(*option))
      return false;
  }

  return true;
}


bool OpalMediaFormat::GetOptionValue(const PString & name, PString & value) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  value = option->AsString();
  return true;
}


bool OpalMediaFormat::SetOptionValue(const PString & name, const PString & value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  return option->FromString(value);
}


bool OpalMediaFormat::GetOptionBoolean(const PString & name, bool dflt) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return dflt;

//  return PDownCast(OpalMediaOptionBoolean, option)->GetValue();
  OpalMediaOptionBoolean * opt = dynamic_cast<OpalMediaOptionBoolean*>(option);
  if(opt==NULL) return dflt;
  return opt->GetValue();
}


bool OpalMediaFormat::SetOptionBoolean(const PString & name, bool value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  PDownCast(OpalMediaOptionBoolean, option)->SetValue(value);
  return true;
}


int OpalMediaFormat::GetOptionInteger(const PString & name, int dflt) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return dflt;

  OpalMediaOptionUnsigned * optUnsigned = dynamic_cast<OpalMediaOptionUnsigned *>(option);
  if (optUnsigned != NULL)
    return optUnsigned->GetValue();

  OpalMediaOptionInteger * optInteger = dynamic_cast<OpalMediaOptionInteger *>(option);
  if (optInteger != NULL)
    return optInteger->GetValue();

  return 0;
}


bool OpalMediaFormat::SetOptionInteger(const PString & name, int value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  OpalMediaOptionUnsigned * optUnsigned = dynamic_cast<OpalMediaOptionUnsigned *>(option);
  if (optUnsigned != NULL) {
    optUnsigned->SetValue(value);
    return true;
  }

  OpalMediaOptionInteger * optInteger = dynamic_cast<OpalMediaOptionInteger *>(option);
  if (optInteger != NULL) {
    optInteger->SetValue(value);
    return true;
  }

  return false;
}


double OpalMediaFormat::GetOptionReal(const PString & name, double dflt) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return dflt;

//  return PDownCast(OpalMediaOptionReal, option)->GetValue();
  OpalMediaOptionReal * opt = dynamic_cast<OpalMediaOptionReal*>(option);
  if(opt==NULL) return dflt;
  return opt->GetValue();
}


bool OpalMediaFormat::SetOptionReal(const PString & name, double value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  PDownCast(OpalMediaOptionReal, option)->SetValue(value);
  return true;
}


PINDEX OpalMediaFormat::GetOptionEnum(const PString & name, PINDEX dflt) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return dflt;

//  return PDownCast(OpalMediaOptionEnum, option)->GetValue();
  OpalMediaOptionEnum * opt = dynamic_cast<OpalMediaOptionEnum*>(option);
  if(opt==NULL) return dflt;
  return opt->GetValue();
}


bool OpalMediaFormat::SetOptionEnum(const PString & name, PINDEX value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  PDownCast(OpalMediaOptionEnum, option)->SetValue(value);
  return true;
}


PString OpalMediaFormat::GetOptionString(const PString & name, const PString & dflt) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return dflt;

//  return PDownCast(OpalMediaOptionString, option)->GetValue();
  OpalMediaOptionString * opt = dynamic_cast<OpalMediaOptionString*>(option);
  if(opt==NULL) return dflt;
  return opt->GetValue();
}


bool OpalMediaFormat::SetOptionString(const PString & name, const PString & value)
{
  PWaitAndSignal m(media_format_mutex);
  options.MakeUnique();

  OpalMediaOption * option = FindOption(name);
  if (option == NULL)
    return false;

  PDownCast(OpalMediaOptionString, option)->SetValue(value);
  return true;
}


bool OpalMediaFormat::AddOption(OpalMediaOption * option, BOOL overwrite)
{
  PWaitAndSignal m(media_format_mutex);
  if (PAssertNULL(option) == NULL)
    return false;

  PINDEX index = options.GetValuesIndex(*option);
  if (index != P_MAX_INDEX) {
    if (!overwrite) {
      delete option;
      return false;
    }

    options.RemoveAt(index);
  }

  options.MakeUnique();
  options.Append(option);
  return true;
}


OpalMediaOption * OpalMediaFormat::FindOption(const PString & name) const
{
  PWaitAndSignal m(media_format_mutex);
  OpalMediaOptionString search(name, false);
  PINDEX index = options.GetValuesIndex(search);
  if (index == P_MAX_INDEX)
    return NULL;

// The following part was modified corresponding the Ilya Pleshchinskii's fix,
// for more details see: http://www.spinics.net/lists/openh323/msg16546.html

//  return &options[index];

// begin of Ilya fix
  OpalMediaOption * op = &options[index];
  int i = 0;
  while ((op->GetName() != name) && (i < 50))
  {
    OpalMediaOptionString search(name, false);
    PINDEX index = options.GetValuesIndex(search);
    if (index == P_MAX_INDEX)
      return NULL;
    op = &options[index];
    i++;
  }
  if (op->GetName() != name)
    return NULL;
  return op;
// end of Ilya fix

}


bool OpalMediaFormat::SetRegisteredMediaFormat(const OpalMediaFormat & mediaFormat)
{
  PWaitAndSignal mutex(OpalMediaFormatFactory::GetMutex());
  OpalMediaFormat * registeredFormat = OpalMediaFormatFactory::CreateInstance(mediaFormat);
  if (registeredFormat == NULL)
    return false;

  *registeredFormat = mediaFormat;
  return true;
}


#if PTRACING
void OpalMediaFormat::DebugOptionList(const OpalMediaFormat & fmt)
{
  PStringStream traceStream;
  traceStream << "         " << fmt.GetOptionCount() << " options found:\n";
  for (PINDEX i = 0; i < fmt.GetOptionCount(); i++) {
    const OpalMediaOption & option = fmt.GetOption(i);
    traceStream << "         " << option.GetName() << " = " << option.AsString() << '\n';
  }
  PTRACE(6,traceStream);
}
#endif

#ifdef H323_VIDEO

const char * const OpalVideoFormat::FrameWidthOption = "Frame Width";
const char * const OpalVideoFormat::FrameHeightOption = "Frame Height";
const char * const OpalVideoFormat::EncodingQualityOption = "Encoding Quality";
const char * const OpalVideoFormat::TargetBitRateOption = "Target Bit Rate";
const char * const OpalVideoFormat::DynamicVideoQualityOption = "Dynamic Video Quality";
const char * const OpalVideoFormat::AdaptivePacketDelayOption = "Adaptive Packet Delay";

const char * const OpalVideoFormat::NeedsJitterOption = "Needs Jitter";
const char * const OpalVideoFormat::MaxBitRateOption = "Max Bit Rate";
const char * const OpalVideoFormat::MaxFrameSizeOption = "Max Frame Size";
const char * const OpalVideoFormat::FrameTimeOption = "Frame Time";
const char * const OpalVideoFormat::ClockRateOption = "Clock Rate";

OpalVideoFormat::OpalVideoFormat(const char * fullName,
                                 RTP_DataFrame::PayloadTypes rtpPayloadType,
                                 unsigned /*frameWidth*/,
                                 unsigned /*frameHeight*/,
                                 unsigned frameRate,
                                 unsigned bitRate,
                                   time_t timeStamp)
  : OpalMediaFormat(fullName,
                    OpalMediaFormat::DefaultVideoSessionID,
                    rtpPayloadType,
                    FALSE,
                    bitRate,
                    0,
                    OpalMediaFormat::VideoTimeUnits/frameRate,
                    OpalMediaFormat::VideoTimeUnits,
                    timeStamp)
{

}

PObject * OpalVideoFormat::Clone() const
{
  return new OpalVideoFormat(*this);
}


bool OpalVideoFormat::Merge(const OpalMediaFormat & mediaFormat)
{
  if (!OpalMediaFormat::Merge(mediaFormat))
    return false;

  unsigned maxBitRate = GetOptionInteger(MaxBitRateOption);
  unsigned targetBitRate = GetOptionInteger(TargetBitRateOption);
  if (targetBitRate > maxBitRate)
    SetOptionInteger(TargetBitRateOption, maxBitRate);

  return true;
}

#endif // H323_VIDEO


// End of File ///////////////////////////////////////////////////////////////
