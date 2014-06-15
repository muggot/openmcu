/*
 * g7222mf_inc.cxx
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
 * $Revision: 30035 $
 * $Author: rjongbloed $
 * $Date: 2013-06-24 18:32:14 +1000 (Mon, 24 Jun 2013) $
 */

#include "opal/opalplugin.h"
#include "opal/known.h"


///////////////////////////////////////////////////////////////////////////////

static const char G7222FormatName[]   = OPAL_G7222;
static const char G7222EncodingName[] = "AMR-WB";

#define G7222_SAMPLES_PER_FRAME    320   // 20 ms frame
#define G7222_SAMPLE_RATE          16000
#define G7222_MAX_BYTES_PER_FRAME  62
#define G7222_MAX_BIT_RATE         (G7222_MAX_BYTES_PER_FRAME*400)

static const char G7222InitialModeName[]               = "Initial Mode";
static const char G7222AlignmentOptionName[]           = PLUGINCODEC_OPTION_OCTET_ALIGNED;
static const char G7222ModeSetOptionName[]             = "Mode Set";
static const char G7222ModeChangePeriodOptionName[]    = "Mode Change Period";
static const char G7222ModeChangeNeighbourOptionName[] = "Mode Change Neighbour";
static const char G7222CRCOptionName[]                 = "CRC";
static const char G7222RobustSortingOptionName[]       = "Robust Sorting";
static const char G7222InterleavingOptionName[]        = "Interleaving";

// H.245 generic parameters; see G.722.2
enum
{
    G7222_H245_MAXAL_SDUFRAMES_RX    = 0 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS,
    G7222_H245_MAXAL_SDUFRAMES_TX    = 0 | PluginCodec_H245_Collapsing   | PluginCodec_H245_OLC,
    G7222_H245_REQUEST_MODE          = 1 | PluginCodec_H245_NonCollapsing| PluginCodec_H245_ReqMode,
    G7222_H245_OCTET_ALIGNED         = 2 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_MODE_SET              = 3 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_MODE_CHANGE_PERIOD    = 4 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_MODE_CHANGE_NEIGHBOUR = 5 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_CRC                   = 6 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_ROBUST_SORTING        = 7 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
    G7222_H245_INTERLEAVING          = 8 | PluginCodec_H245_Collapsing   | PluginCodec_H245_TCS | PluginCodec_H245_OLC | PluginCodec_H245_ReqMode,
};


static const char G7222InitialModeFMTPName[] = "mode";
static const char G7222AlignmentFMTPName[]   = "octet-align";

#define G7222_MAX_MODES              8
#define G7222_MODE_INITIAL_VALUE     7
#define G7222_MODE_SET_INITIAL_VALUE 0x1ff


// End of File ///////////////////////////////////////////////////////////////
