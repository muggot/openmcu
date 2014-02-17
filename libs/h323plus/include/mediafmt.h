/*
 * mediafmt.h
 *
 * Media Format descriptions
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2001 Equivalence Pty. Ltd.
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
 * $Log: mediafmt.h,v $
 * Revision 1.4  2008/02/06 13:19:13  shorne
 * Critical Bug Fix: Stop Assert on unhandled mediaOptions
 *
 * Revision 1.3  2007/10/19 19:53:44  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.2  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.26.2.4  2007/10/03 13:10:00  rjongbloed
 * Removed duplicate OpalMediaFormat list caused by merge clash from OPAL
 *
 * Revision 1.26.2.3  2007/08/17 08:38:22  rjongbloed
 * Back ported OPAL meda options based plug ins and H.323 generic capabilties.
 *
 * Revision 1.26.2.2  2007/03/24 23:39:42  shorne
 * More H.239 work
 *
 * Revision 1.26.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.26  2006/09/05 23:56:57  csoutheren
 * Convert media format and capability factories to use std::string
 *
 * Revision 1.25  2006/06/23 03:18:23  shorne
 * Backport FECC (H.224) from Opal
 *
 * Revision 1.24  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.23  2005/08/13 19:35:57  shorne
 * Fix compile error on MSVC6
 *
 * Revision 1.22  2004/07/07 08:04:54  csoutheren
 * Added video codecs to default codec list, but H.263 is only loaded if the .so/DLL is found
 *
 * Revision 1.21  2004/07/07 03:52:12  csoutheren
 * Fixed incorrect strings returned by GetFormatName on G.711 codecs
 *
 * Revision 1.20  2004/06/30 12:31:09  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.19  2004/05/23 12:49:20  rjongbloed
 * Tidied some of the OpalMediaFormat usage after abandoning some previous
 *   code due to MSVC6 compiler bug.
 *
 * Revision 1.18  2004/05/20 02:07:28  csoutheren
 * Use macro to work around MSVC internal compiler errors
 *
 * Revision 1.17  2004/05/19 07:38:22  csoutheren
 * Changed OpalMediaFormat handling to use abstract factory method functions
 *
 * Revision 1.16  2004/05/03 00:52:23  csoutheren
 * Fixed problem with OpalMediaFormat::GetMediaFormatsList
 * Added new version of OpalMediaFormat::GetMediaFormatsList that minimses copying
 *
 * Revision 1.15  2004/04/03 10:38:24  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.14.2.1  2004/03/31 11:11:59  csoutheren
 * Initial public release of plugin codec code
 *
 * Revision 1.14  2004/02/26 08:19:31  csoutheren
 * Fixed threading problem with GetMediaFormatList
 *
 * Revision 1.13  2002/12/02 03:06:26  robertj
 * Fixed over zealous removal of code when NO_AUDIO_CODECS set.
 *
 * Revision 1.12  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.11  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.10  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.9  2002/06/25 08:30:08  robertj
 * Changes to differentiate between stright G.723.1 and G.723.1 Annex A using
 *   the OLC dataType silenceSuppression field so does not send SID frames
 *   to receiver codecs that do not understand them.
 *
 * Revision 1.8  2002/03/21 02:39:15  robertj
 * Added backward compatibility define
 *
 * Revision 1.7  2002/02/11 04:15:56  robertj
 * Put G.723.1 at 6.3kbps back to old string value of "G.723.1" to improve
 *   backward compatibility. New #define is a synonym for it.
 *
 * Revision 1.6  2002/01/22 07:08:26  robertj
 * Added IllegalPayloadType enum as need marker for none set
 *   and MaxPayloadType is a legal value.
 *
 * Revision 1.5  2001/12/11 04:27:50  craigs
 * Added support for 5.3kbps G723.1
 *
 * Revision 1.4  2001/09/21 02:49:44  robertj
 * Implemented static object for all "known" media formats.
 * Added default session ID to media format description.
 *
 * Revision 1.3  2001/05/11 04:43:41  robertj
 * Added variable names for standard PCM-16 media format name.
 *
 * Revision 1.2  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.1  2001/01/25 07:27:14  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 */

#ifndef __OPAL_MEDIAFMT_H
#define __OPAL_MEDIAFMT_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "rtp.h"

#include <limits>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

class OpalMediaFormat;


///////////////////////////////////////////////////////////////////////////////

/**Base class for options attached to an OpalMediaFormat.
  */
class OpalMediaOption : public PObject
{
    PCLASSINFO(OpalMediaOption, PObject);
  public:
    enum MergeType {
      NoMerge,
      MinMerge,
      MaxMerge,
      EqualMerge,
      NotEqualMerge,
      AlwaysMerge,

      // Synonyms
      AndMerge = MaxMerge,
      OrMerge  = MinMerge,
      XorMerge = NotEqualMerge,
      NotXorMerge = EqualMerge
    };

  protected:
    OpalMediaOption(
      const char * name,
      bool readOnly,
      MergeType merge
    );

  public:
    virtual Comparison Compare(const PObject & obj) const;

    bool Merge(
      const OpalMediaOption & option
    );
    virtual Comparison CompareValue(
      const OpalMediaOption & option
    ) const = 0;
    virtual void Assign(
      const OpalMediaOption & option
    ) = 0;

    PString AsString() const;
    bool FromString(const PString & value);

    const PString & GetName() const { return m_name; }

    bool IsReadOnly() const { return m_readOnly; }
    void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }

    MergeType GetMerge() const { return m_merge; }
    void SetMerge(MergeType merge) { m_merge = merge; }

    const PString & GetFMTPName() const { return m_FMTPName; }
    void SetFMTPName(const char * name) { m_FMTPName = name; }

    const PString & GetFMTPDefault() const { return m_FMTPDefault; }
    void SetFMTPDefault(const char * value) { m_FMTPDefault = value; }

    struct H245GenericInfo {
      unsigned ordinal:16;
      enum Modes {
        None,
        Collapsing,
        NonCollapsing
      } mode:3;
      enum IntegerTypes {
        UnsignedInt,
        Unsigned32,
        BooleanArray
      } integerType:3;
      bool excludeTCS:1;
      bool excludeOLC:1;
      bool excludeReqMode:1;
    };

    const H245GenericInfo & GetH245Generic() const { return m_H245Generic; }
    void SetH245Generic(const H245GenericInfo & generic) { m_H245Generic = generic; }

  protected:
    PCaselessString m_name;
    bool            m_readOnly;
    MergeType       m_merge;
    PCaselessString m_FMTPName;
    PString         m_FMTPDefault;
    H245GenericInfo m_H245Generic;
};

#ifndef __USE_STL__
__inline istream & operator>>(istream & strm, bool& b)
{
   int i;strm >> i;b = i; return strm;
}
#endif

template <typename T>
class OpalMediaOptionValue : public OpalMediaOption
{
    PCLASSINFO(OpalMediaOptionValue, OpalMediaOption);
  public:
    OpalMediaOptionValue(
      const char * name,
      bool readOnly,
      MergeType merge = MinMerge,
      T value = 0,
      T minimum = std::numeric_limits<T>::min(),
      T maximum = std::numeric_limits<T>::max()
    ) : OpalMediaOption(name, readOnly, merge),
        m_value(value),
        m_minimum(minimum),
        m_maximum(maximum)
    { }

    virtual PObject * Clone() const
    {
      return new OpalMediaOptionValue(*this);
    }

    virtual void PrintOn(ostream & strm) const
    {
      strm << m_value;
    }

    virtual void ReadFrom(istream & strm)
    {
      T temp;
      strm >> temp;
      if (temp >= m_minimum && temp <= m_maximum)
        m_value = temp;
      else {
#ifdef __USE_STL__
	   strm.setstate(ios::badbit);
#else
	   strm.setf(ios::badbit , ios::badbit);
#endif
       }
    }

    virtual Comparison CompareValue(const OpalMediaOption & option) const {
	  if (!PIsDescendant(&option, OpalMediaOptionValue)) {
		  PTRACE(6,"MediaOpt\t" << option.GetName() << " not compared! Not descendent of OpalMediaOptionValue");
		  return GreaterThan;
	  }
      const OpalMediaOptionValue * otherOption = PDownCast(const OpalMediaOptionValue, &option);
      if (otherOption == NULL)
        return GreaterThan;
      if (m_value < otherOption->m_value)
        return LessThan;
      if (m_value > otherOption->m_value)
        return GreaterThan;
      return EqualTo;
    }

    virtual void Assign(
      const OpalMediaOption & option
    ) {
	  if (!PIsDescendant(&option, OpalMediaOptionValue)) {
		  PTRACE(6,"MediaOpt\t" << option.GetName() << " not assigned! Not descendent of OpalMediaOptionValue");
		  return;
	  }
      const OpalMediaOptionValue * otherOption = PDownCast(const OpalMediaOptionValue, &option);
      if (otherOption != NULL)
        m_value = otherOption->m_value;
    }

    T GetValue() const { return m_value; }
    void SetValue(T value) { m_value = value; }

  protected:
    T m_value;
    T m_minimum;
    T m_maximum;
};


typedef OpalMediaOptionValue<bool>     OpalMediaOptionBoolean;
typedef OpalMediaOptionValue<int>      OpalMediaOptionInteger;
typedef OpalMediaOptionValue<unsigned> OpalMediaOptionUnsigned;
typedef OpalMediaOptionValue<double>   OpalMediaOptionReal;


class OpalMediaOptionEnum : public OpalMediaOption
{
    PCLASSINFO(OpalMediaOptionEnum, OpalMediaOption);
  public:
    OpalMediaOptionEnum(
      const char * name,
      bool readOnly,
      const char * const * enumerations,
      PINDEX count,
      MergeType merge = EqualMerge,
      PINDEX value = 0
    );

    virtual PObject * Clone() const;
    virtual void PrintOn(ostream & strm) const;
    virtual void ReadFrom(istream & strm);

    virtual Comparison CompareValue(const OpalMediaOption & option) const;
    virtual void Assign(const OpalMediaOption & option);

    PINDEX GetValue() const { return m_value; }
    void SetValue(PINDEX value);

  protected:
    PStringArray m_enumerations;
    PINDEX       m_value;
};


class OpalMediaOptionString : public OpalMediaOption
{
    PCLASSINFO(OpalMediaOptionString, OpalMediaOption);
  public:
    OpalMediaOptionString(
      const char * name,
      bool readOnly
    );
    OpalMediaOptionString(
      const char * name,
      bool readOnly,
      const PString & value
    );

    virtual PObject * Clone() const;
    virtual void PrintOn(ostream & strm) const;
    virtual void ReadFrom(istream & strm);

    virtual Comparison CompareValue(const OpalMediaOption & option) const;
    virtual void Assign(const OpalMediaOption & option);

    const PString & GetValue() const { return m_value; }
    void SetValue(const PString & value);

  protected:
    PString m_value;
};


class OpalMediaOptionOctets : public OpalMediaOption
{
    PCLASSINFO(OpalMediaOptionOctets, OpalMediaOption);
  public:
    OpalMediaOptionOctets(
      const char * name,
      bool readOnly,
      bool base64
    );
    OpalMediaOptionOctets(
      const char * name,
      bool readOnly,
      bool base64,
      const PBYTEArray & value
    );
    OpalMediaOptionOctets(
      const char * name,
      bool readOnly,
      bool base64,
      const BYTE * data,
      PINDEX length
    );

    virtual PObject * Clone() const;
    virtual void PrintOn(ostream & strm) const;
    virtual void ReadFrom(istream & strm);

    virtual Comparison CompareValue(const OpalMediaOption & option) const;
    virtual void Assign(const OpalMediaOption & option);

    const PBYTEArray & GetValue() const { return m_value; }
    void SetValue(const PBYTEArray & value);
    void SetValue(const BYTE * data, PINDEX length);

  protected:
    PBYTEArray m_value;
    bool       m_base64;
};


///////////////////////////////////////////////////////////////////////////////

/**This class describes a media format as used in the OPAL system. A media
   format is the type of any media data that is trasferred between OPAL
   entities. For example an audio codec such as G.723.1 is a media format, a
   video codec such as H.261 is also a media format.
  */
class OpalMediaFormat : public PCaselessString
{
  PCLASSINFO(OpalMediaFormat, PCaselessString);

  public:
    PLIST(List, OpalMediaFormat);

    /**Default constructor creates a PCM-16 media format.
      */
    OpalMediaFormat();

    /**A constructor that only has a string name will search through the
       RegisteredMediaFormats list for the full specification so the other
       information fields can be set from the database.
      */
    OpalMediaFormat(
      const char * search,  ///<  Name to search for
      BOOL exact = TRUE     ///<  Flag for if search is to match name exactly
    );

    /**Return TRUE if media format info is valid. This may be used if the
       single string constructor is used to check that it matched something
       in the registered media formats database.
      */
    BOOL IsValid() const { return rtpPayloadType <= RTP_DataFrame::MaxPayloadType; }

    /**Copy a media format
      */
    OpalMediaFormat & operator=(
      const OpalMediaFormat & fmt ///<  other media format
    );

    /**Merge with another media format. This will alter and validate
       the options for this media format according to the merge rule for
       each option. The parameter is typically a "capability" while the
       current object isthe proposed channel format. This if the current
       object has a tx number of frames of 3, but the parameter has a value
       of 1, then the current object will be set to 1.

       Returns FALSE if the media formats are incompatible and cannot be
       merged.
      */
    virtual bool Merge(
      const OpalMediaFormat & mediaFormat
    );

    /**Get the RTP payload type that is to be used for this media format.
       This will either be an intrinsic one for the media format eg GSM or it
       will be automatically calculated as a dynamic media format that will be
       uniqueue amongst the registered media formats.
      */
    RTP_DataFrame::PayloadTypes GetPayloadType() const { return rtpPayloadType; }

    void SetPayloadType(RTP_DataFrame::PayloadTypes type) { rtpPayloadType = type; }
    enum {
      FirstSessionID            = 1,
      DefaultAudioSessionID     = 1,
      DefaultVideoSessionID     = 2,
      DefaultDataSessionID      = 3,
      DefaultH224SessionID      = 4,
      DefaultExtVideoSessionID  = 5,
      LastSessionID             = 5
    };

    /**Get the default session ID for media format.
      */
    unsigned GetDefaultSessionID() const { return defaultSessionID; }

    /**Determine if the media format requires a jitter buffer. As a rule an
       audio codec needs a jitter buffer and all others do not.
      */
    BOOL NeedsJitterBuffer() const { return needsJitter; }

    /**Get the average bandwidth used in bits/second.
      */
    unsigned GetBandwidth() const { return bandwidth; }

    /**Get the maximum frame size in bytes. If this returns zero then the
       media format has no intrinsic maximum frame size, eg G.711 would 
       return zero but G.723.1 whoud return 24.
      */
    PINDEX GetFrameSize() const { return frameSize; }
	void SetFrameSize(PINDEX size) { frameSize = size; }

    /**Get the frame rate in RTP timestamp units. If this returns zero then
       the media format is not real time and has no intrinsic timing eg
      */
    unsigned GetFrameTime() const { return frameTime; }

	/**Set the frame rate in RTP timestamp units.
	  */
	void SetFrameTime(unsigned ft) { frameTime = ft; }

    // Multi channels support
    unsigned GetEncoderChannels() const
    {
      PString OptionValue;
      return (GetOptionValue((const PString)"Encoder Channels", OptionValue)) ? atoi(OptionValue) : 1;
    }

    // Multi channels support
    unsigned GetDecoderChannels() const
    {
      PString OptionValue;
      return (GetOptionValue((const PString)"Decoder Channels", OptionValue)) ? atoi(OptionValue) : 1;
    }

    /**Get the number of RTP timestamp units per millisecond.
      */
    virtual unsigned GetTimeUnits() const { return timeUnits; }
    virtual void SetTimeUnits(unsigned units) { timeUnits = units; }

    enum StandardTimeUnits {
      AudioTimeUnits = 8,  ///<  8kHz sample rate
      VideoTimeUnits = 90  ///<  90kHz sample rate
    };
 
    /**Get the list of media formats that have been registered.
      */
    static List GetRegisteredMediaFormats();
    static void GetRegisteredMediaFormats(List & list);

    friend class OpalStaticMediaFormat;

    /**This form of the constructor will register the full details of the
       media format into an internal database. This would typically be used
       as a static global. In fact it would be very dangerous for an instance
       to use this constructor in any other way, especially local variables.

       If the rtpPayloadType is RTP_DataFrame::DynamicBase, then the RTP
       payload type is actually set to teh first unused dynamic RTP payload
       type that is in the registers set of media formats.

       The frameSize parameter indicates that the media format has a maximum
       size for each data frame, eg G.723.1 frames are no more than 24 bytes
       long. If zero then there is no intrinsic maximum, eg G.711.
      */
    OpalMediaFormat(
      const char * fullName,  ///<  Full name of media format
      unsigned defaultSessionID,  ///<  Default session for codec type
      RTP_DataFrame::PayloadTypes rtpPayloadType, ///<  RTP payload type code
      BOOL     needsJitter,   ///<  Indicate format requires a jitter buffer
      unsigned bandwidth,     ///<  Bandwidth in bits/second
      PINDEX   frameSize = 0, ///<  Size of frame in bytes (if applicable)
      unsigned frameTime = 0, ///<  Time for frame in RTP units (if applicable)
      unsigned timeUnits = 0, ///<  RTP units for frameTime (if applicable)
      time_t timeStamp = 0    ///<  timestamp (for versioning)

    );
    
    bool GetOptionValue(
      const PString & name,   ///<  Option name
      PString & value         ///<  String to receive option value
    ) const;

    /**Set the option value of the specified name as a string.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present.
      */
    bool SetOptionValue(
      const PString & name,   ///<  Option name
      const PString & value   ///<  New option value as string
    );

    /**Get the option value of the specified name as a boolean. The default
       value is returned if the option is not present.
      */
    bool GetOptionBoolean(
      const PString & name,   ///<  Option name
      bool dflt = FALSE       ///<  Default value if option not present
    ) const;

    /**Set the option value of the specified name as a boolean.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present or is not of the same type.
      */
    bool SetOptionBoolean(
      const PString & name,   ///<  Option name
      bool value              ///<  New value for option
    );

    /**Get the option value of the specified name as an integer. The default
       value is returned if the option is not present.
      */
    int GetOptionInteger(
      const PString & name,   ///<  Option name
      int dflt = 0            ///<  Default value if option not present
    ) const;

    /**Set the option value of the specified name as an integer.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present, not of the same type or
       is putside the allowable range.
      */
    bool SetOptionInteger(
      const PString & name,   ///<  Option name
      int value               ///<  New value for option
    );

    /**Get the option value of the specified name as a real. The default
       value is returned if the option is not present.
      */
    double GetOptionReal(
      const PString & name,   ///<  Option name
      double dflt = 0         ///<  Default value if option not present
    ) const;

    /**Set the option value of the specified name as a real.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present or is not of the same type.
      */
    bool SetOptionReal(
      const PString & name,   ///<  Option name
      double value            ///<  New value for option
    );

    /**Get the option value of the specified name as an index into an
       enumeration list. The default value is returned if the option is not
       present.
      */
    PINDEX GetOptionEnum(
      const PString & name,   ///<  Option name
      PINDEX dflt = 0         ///<  Default value if option not present
    ) const;

    /**Set the option value of the specified name as an index into an enumeration.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present or is not of the same type.
      */
    bool SetOptionEnum(
      const PString & name,   ///<  Option name
      PINDEX value            ///<  New value for option
    );

    /**Get the option value of the specified name as a string. The default
       value is returned if the option is not present.
      */
    PString GetOptionString(
      const PString & name,                   ///<  Option name
      const PString & dflt = PString::Empty() ///<  Default value if option not present
    ) const;

    /**Set the option value of the specified name as a string.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present or is not of the same type.
      */
    bool SetOptionString(
      const PString & name,   ///<  Option name
      const PString & value   ///<  New value for option
    );

    /**Get the option value of the specified name as an octet array.
       Returns FALSE if not present.
      */
    bool GetOptionOctets(
      const PString & name, ///<  Option name
      PBYTEArray & octets   ///<  Octets in option
    ) const;

    /**Set the option value of the specified name as an octet array.
       Note the option will not be added if it does not exist, the option
       must be explicitly added using AddOption().

       Returns false of the option is not present or is not of the same type.
      */
    bool SetOptionOctets(
      const PString & name,       ///<  Option name
      const PBYTEArray & octets   ///<  Octets in option
    );
    bool SetOptionOctets(
      const PString & name,       ///<  Option name
      const BYTE * data,          ///<  Octets in option
      PINDEX length               ///<  Number of octets
    );

    /**Set the options on the master format list entry.
       The media format must already be registered. Returns false if not.
      */
    static bool SetRegisteredMediaFormat(
      const OpalMediaFormat & mediaFormat  ///<  Media format to copy to master list
    );

    /**
      * Add a new option to this media format
      */
    bool AddOption(
      OpalMediaOption * option,
      BOOL overwrite = FALSE
    );
    
    /**
      * Determine if media format has the specified option.
      */
    bool HasOption(const PString & name) const
    { return FindOption(name) != NULL; }

    /**
      * Get a pointer to the specified media format option.
      * Returns NULL if thee option does not exist.
      */
    OpalMediaOption * FindOption(
      const PString & name
    ) const;

	OpalMediaOption & GetOption(PINDEX i) const
	{ return options[i];  }

	PINDEX GetOptionCount() const
	{ return options.GetSize(); }

#if PTRACING
	static void DebugOptionList(const OpalMediaFormat & fmt);
#endif

  protected:
    RTP_DataFrame::PayloadTypes rtpPayloadType;
    unsigned defaultSessionID;
    BOOL     needsJitter;
    unsigned bandwidth;
    PINDEX   frameSize;
    unsigned frameTime;
    unsigned timeUnits;
    PMutex   media_format_mutex;
    PSortedList<OpalMediaOption> options;
    time_t codecBaseTime;

};

#ifdef H323_VIDEO
class OpalVideoFormat : public OpalMediaFormat
{
  friend class OpalPluginCodecManager;
    PCLASSINFO(OpalVideoFormat, OpalMediaFormat);
  public:
    OpalVideoFormat(
      const char * fullName,    ///<  Full name of media format
      RTP_DataFrame::PayloadTypes rtpPayloadType, ///<  RTP payload type code
      unsigned frameWidth,      ///<  Width of video frame
      unsigned frameHeight,     ///<  Height of video frame
      unsigned frameRate,       ///<  Number of frames per second
      unsigned bitRate,         ///<  Maximum bits per second
      time_t timeStamp = 0        ///<  timestamp (for versioning)
    );

    virtual PObject * Clone() const;

    virtual bool Merge(const OpalMediaFormat & mediaFormat);

    static const char * const FrameWidthOption;
    static const char * const FrameHeightOption;
    static const char * const EncodingQualityOption;
    static const char * const TargetBitRateOption;
    static const char * const DynamicVideoQualityOption;
    static const char * const AdaptivePacketDelayOption;

    static const char * const NeedsJitterOption;
    static const char * const MaxBitRateOption;
    static const char * const MaxFrameSizeOption;
    static const char * const FrameTimeOption;
	static const char * const ClockRateOption;

};
#endif
// List of known media formats

#define OPAL_PCM16         "PCM-16"
#define OPAL_G711_ULAW_64K "G.711-uLaw-64k"
#define OPAL_G711_ALAW_64K "G.711-ALaw-64k"
#define OPAL_G711_ULAW_56K "G.711-uLaw-56k"
#define OPAL_G711_ALAW_56K "G.711-ALaw-56k"
#define OPAL_G728          "G.728"
#define OPAL_G729          "G.729"
#define OPAL_G729A         "G.729A"
#define OPAL_G729B         "G.729B"
#define OPAL_G729AB        "G.729A/B"
#define OPAL_G7231         "G.723.1"
#define OPAL_G7231_6k3     OPAL_G7231
#define OPAL_G7231_5k3     "G.723.1(5.3k)"
#define OPAL_G7231A_6k3    "G.723.1A(6.3k)"
#define OPAL_G7231A_5k3    "G.723.1A(5.3k)"
#define OPAL_GSM0610       "GSM-06.10"

extern char OpalPCM16[];
extern char OpalG711uLaw64k[];
extern char OpalG711ALaw64k[];
extern char OpalG728[];
extern char OpalG729[];
extern char OpalG729A[];
extern char OpalG729B[];
extern char OpalG729AB[];
extern char OpalG7231_6k3[];
extern char OpalG7231_5k3[];
extern char OpalG7231A_6k3[];
extern char OpalG7231A_5k3[];
extern char OpalGSM0610[];

#define OpalG711uLaw      OpalG711uLaw64k
#define OpalG711ALaw      OpalG711ALaw64k
#define OpalG7231 OpalG7231_6k3

//
// Originally, the following inplace code was used instead of this macro:
//
// static PAbstractSingletonFactory<OpalMediaFormat, 
//     OpalStaticMediaFormatTemplate<
//          OpalPCM16,
//          OpalMediaFormat::DefaultAudioSessionID,
//          RTP_DataFrame::L16_Mono,
//          TRUE,   // Needs jitter
//          128000, // bits/sec
//          16, // bytes/frame
//          8, // 1 millisecond
//          OpalMediaFormat::AudioTimeUnits,
//          0
//     > 
// > opalPCM16Factory(OpalPCM16);
//
// This used the following macro:
//
//
//  template <
//        const char * _fullName,  /// Full name of media format
//        unsigned _defaultSessionID,  /// Default session for codec type
//        RTP_DataFrame::PayloadTypes _rtpPayloadType, /// RTP payload type code
//        BOOL     _needsJitter,       /// Indicate format requires a jitter buffer
//        unsigned _bandwidth,         /// Bandwidth in bits/second
//        PINDEX   _frameSize,         /// Size of frame in bytes (if applicable)
//        unsigned _frameTime,         /// Time for frame in RTP units (if applicable)
//        unsigned _timeUnits,         /// RTP units for frameTime (if applicable)
//        time_t _timeStamp            /// timestamp (for versioning)
//  >
//  class OpalStaticMediaFormatTemplate : public OpalStaticMediaFormat
//  {
//    public:
//      OpalStaticMediaFormatTemplate()
//        : OpalStaticMediaFormat(_fullName, _defaultSessionID, _rtpPayloadType, _needsJitter, _bandwidth
//        , _frameSize, _frameTime, _timeUnits, _timeStamp )
//      { }
//  };
//
// Unfortauntely, MSVC 6 did not like this so this crappy macro has to be used instead of a template
//

typedef PFactory<OpalMediaFormat, std::string> OpalMediaFormatFactory;

#define OPAL_MEDIA_FORMAT_DECLARE(classname, _fullName, _defaultSessionID, _rtpPayloadType, _needsJitter,_bandwidth, _frameSize, _frameTime, _timeUnits, _timeStamp) \
class classname : public OpalMediaFormat \
{ \
  public: \
    classname() \
      : OpalMediaFormat(_fullName, _defaultSessionID, _rtpPayloadType, _needsJitter, _bandwidth, \
        _frameSize, _frameTime, _timeUnits, _timeStamp){} \
}; \
OpalMediaFormatFactory::Worker<classname> classname##Factory(_fullName, true); \


#endif  // __OPAL_MEDIAFMT_H


// End of File ///////////////////////////////////////////////////////////////
