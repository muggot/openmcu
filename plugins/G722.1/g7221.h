/*
 * g7221mf.cxx
 *
 * GSM-AMR Media Format descriptions
 *
 * Open Phone Abstraction Library
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2008 Vox Lucida
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
 * The Original Code is Open Phone Abstraction Library
 *
 * The Initial Developer of the Original Code is Vox Lucida
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 31724 $
 * $Author: rjongbloed $
 * $Date: 2014-04-10 15:45:21 +1000 (Thu, 10 Apr 2014) $
 */

#include "opal/opalplugin.h"
#include "opal/known.h"


///////////////////////////////////////////////////////////////////////////////

static const char G7221_24K_FormatName [] = OPAL_G7221_24K;
static const char G7221_32K_FormatName [] = OPAL_G7221_32K;
static const char G7221C_24K_FormatName[] = OPAL_G7221C_24K;
static const char G7221C_32K_FormatName[] = OPAL_G7221C_32K;
static const char G7221C_48K_FormatName[] = OPAL_G7221C_48K;

static const char G7221EncodingName[] = "G7221"; // MIME name rfc's 3047, 5577

#define G7221_24K_BIT_RATE         24000
#define G7221_32K_BIT_RATE         32000
#define G7221C_24K_BIT_RATE        24000
#define G7221C_32K_BIT_RATE        32000
#define G7221C_48K_BIT_RATE        48000

#define G7221_FRAME_MS             20
#define G7221_24K_SAMPLE_RATE      16000
#define G7221_32K_SAMPLE_RATE      16000
#define G7221C_24K_SAMPLE_RATE     32000
#define G7221C_32K_SAMPLE_RATE     32000
#define G7221C_48K_SAMPLE_RATE     32000

static const char G7221BitRateOptionName[] = "Bit Rate";
static const char G7221BitRateFMTPName[]   = "bitrate";
static const char G7221ExtendedModesOptionName[] = "Extended Modes";

enum
{
    G7221_H241_RxFramesPerPacket = 1 | PluginCodec_H245_Collapsing | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7221_H241_ExtendedModes     = 2 | PluginCodec_H245_Collapsing | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode | PluginCodec_H245_BooleanArray,
};

#define G7221_24K_OID    OpalPluginCodec_Identifer_G7221
#define G7221_32K_OID    OpalPluginCodec_Identifer_G7221
#define G7221C_24K_OID   OpalPluginCodec_Identifer_G7221ext
#define G7221C_32K_OID   OpalPluginCodec_Identifer_G7221ext
#define G7221C_48K_OID   OpalPluginCodec_Identifer_G7221ext


// End of File ///////////////////////////////////////////////////////////////
