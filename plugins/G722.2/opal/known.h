/*
 * known.h
 *
 * Known media format names
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * $Revision: 31724 $
 * $Author: rjongbloed $
 * $Date: 2014-04-10 15:45:21 +1000 (Thu, 10 Apr 2014) $
 */

#ifndef OPAL_CODEC_KNOWN_H
#define OPAL_CODEC_KNOWN_H

#define OPAL_PCM16          "PCM-16"
#define OPAL_PCM16S         "PCM-16S"
#define OPAL_PCM16_8KHZ     OPAL_PCM16
#define OPAL_PCM16S_8KHZ    OPAL_PCM16S
#define OPAL_PCM16_12KHZ    "PCM-16-12kHz"
#define OPAL_PCM16S_12KHZ   "PCM-16S-12kHz"
#define OPAL_PCM16_16KHZ    "PCM-16-16kHz"
#define OPAL_PCM16S_16KHZ   "PCM-16S-16kHz"
#define OPAL_PCM16_24KHZ    "PCM-16-24kHz"
#define OPAL_PCM16S_24KHZ   "PCM-16S-24kHz"
#define OPAL_PCM16_32KHZ    "PCM-16-32kHz"
#define OPAL_PCM16S_32KHZ   "PCM-16S-32kHz"
#define OPAL_PCM16_48KHZ    "PCM-16-48kHz"
#define OPAL_PCM16S_48KHZ   "PCM-16S-48kHz"

#define OPAL_L16_MONO_8KHZ  "Linear-16-Mono-8kHz"
#define OPAL_L16_STEREO_8KHZ "Linear-16-Stereo-8kHz"
#define OPAL_L16_MONO_16KHZ "Linear-16-Mono-16kHz"
#define OPAL_L16_STEREO_16KHZ "Linear-16-Stereo-16kHz"
#define OPAL_L16_MONO_32KHZ "Linear-16-Mono-32kHz"
#define OPAL_L16_STEREO_32KHZ "Linear-16-Stereo-32kHz"
#define OPAL_L16_MONO_48KHZ "Linear-16-Mono-48kHz"
#define OPAL_L16_STEREO_48KHZ "Linear-16-Stereo-48kHz"

#define OPAL_G711_ULAW_64K  "G.711-uLaw-64k"
#define OPAL_G711_ALAW_64K  "G.711-ALaw-64k"
#define OPAL_G722           "G.722-64k"
#define OPAL_G7221          OPAL_G7221_32K
#define OPAL_G7221_24K      "G.722.1-24K"
#define OPAL_G7221_32K      "G.722.1-32K"
#define OPAL_G7221C_24K     "G.722.1C-24K"
#define OPAL_G7221C_32K     "G.722.1C-32K"
#define OPAL_G7221C_48K     "G.722.1C-48K"
#define OPAL_G7222          "G.722.2"
#define OPAL_G726_40K       "G.726-40K"
#define OPAL_G726_32K       "G.726-32K"
#define OPAL_G726_24K       "G.726-24K"
#define OPAL_G726_16K       "G.726-16K"
#define OPAL_G728           "G.728"
#define OPAL_G729           "G.729"
#define OPAL_G729A          "G.729A"
#define OPAL_G729B          "G.729B"
#define OPAL_G729AB         "G.729A/B"
#define OPAL_G7231          "G.723.1"
#define OPAL_G7231_6k3      OPAL_G7231
#define OPAL_G7231_5k3      "G.723.1(5.3k)"
#define OPAL_G7231A_6k3     "G.723.1A(6.3k)"
#define OPAL_G7231A_5k3     "G.723.1A(5.3k)"
#define OPAL_G7231_Cisco_A  "G.723.1-Cisco-a"
#define OPAL_G7231_Cisco_AR "G.723.1-Cisco-ar"
#define OPAL_GSM0610        "GSM-06.10"
#define OPAL_GSMAMR         "GSM-AMR"
#define OPAL_iLBC           "iLBC"

#define OPAL_RGB24          "RGB24"
#define OPAL_RGB32          "RGB32"
#define OPAL_BGR24          "BGR24"
#define OPAL_BGR32          "BGR32"
#define OPAL_YUV420P        "YUV420P"
#define OPAL_H261           "H.261"
#define OPAL_H263           "H.263"
#define OPAL_H263plus       "H.263plus"
#define OPAL_H264           OPAL_H264_MODE1
#define OPAL_H264_MODE0     "H.264-0"
#define OPAL_H264_MODE1     "H.264-1"
#define OPAL_H264_High      "H.264-High"
#define OPAL_MPEG4          "MPEG4"
#define OPAL_VP8            "VP8-WebM"

#define OPAL_RFC2833        "UserInput/RFC2833"
#define OPAL_CISCONSE       "NamedSignalEvent"
#define OPAL_T38            "T.38"
#define OPAL_FECC_RTP       "FECC-RTP"
#define OPAL_FECC_HDLC      "FECC-HDLC"


#ifndef OPAL_PLUGIN_COMPILE


#endif // OPAL_PLUGIN_COMPILE


#endif  // OPAL_CODEC_KNOWN_H


// End of File ///////////////////////////////////////////////////////////////
