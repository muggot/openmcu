/*
 * custom.cxx
 *
 * PWLib application source file for xmlrpcsrvr
 *
 * Customisable application configurationfor OEMs.
 *
 * Copyright 2001 Equivalence Pty. Ltd.
 */

#ifdef RC_INVOKED
#include <winver.h>
#else
#include <ptlib.h>
#include "custom.h"
#endif

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

#ifndef BUILD_TYPE
#define BUILD_TYPE ReleaseCode
#endif

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif


////////////////////////////////////////////////////
//
// Variables required for PHTTPServiceProcess
//
////////////////////////////////////////////////////

#ifndef PRODUCT_NAME_TEXT
#define PRODUCT_NAME_TEXT "PWLib XML/RPC Server"
#endif

#ifndef EXE_NAME_TEXT
#define EXE_NAME_TEXT         "xmlrpcsrvr"
#endif

#ifndef MANUFACTURER_TEXT
#define MANUFACTURER_TEXT "Equivalence Pty. Ltd."
#endif

#ifndef COPYRIGHT_HOLDER
#define COPYRIGHT_HOLDER MANUFACTURER_TEXT
#endif

#ifndef GIF_NAME
#define GIF_NAME    EXE_NAME_TEXT ".gif"
#define GIF_WIDTH  380
#define GIF_HEIGHT 101
#endif

#ifndef EMAIL
#define EMAIL NULL
#endif

#ifndef HOME_PAGE
#define HOME_PAGE NULL
#endif

#ifndef PRODUCT_NAME_HTML
#define PRODUCT_NAME_HTML PRODUCT_NAME_TEXT
#endif


#ifdef RC_INVOKED

#define AlphaCode alpha
#define BetaCode beta
#define ReleaseCode pl

#define MkStr2(s) #s
#define MkStr(s) MkStr2(s)

#if BUILD_NUMBER==0
#define VERSION_STRING \
    MkStr(MAJOR_VERSION) "." MkStr(MINOR_VERSION)
#else
#define VERSION_STRING \
    MkStr(MAJOR_VERSION) "." MkStr(MINOR_VERSION) MkStr(BUILD_TYPE) MkStr(BUILD_NUMBER)
#endif


VS_VERSION_INFO VERSIONINFO
#define alpha 1
#define beta 2
#define pl 3
  FILEVERSION     MAJOR_VERSION,MINOR_VERSION,BUILD_TYPE,BUILD_NUMBER
  PRODUCTVERSION  MAJOR_VERSION,MINOR_VERSION,BUILD_TYPE,BUILD_NUMBER
#undef alpha
#undef beta
#undef pl
  FILEFLAGSMASK   VS_FFI_FILEFLAGSMASK
#ifdef _DEBUG
  FILEFLAGS       VS_FF_DEBUG
#else
  FILEFLAGS       0
#endif
  FILEOS          VOS_NT_WINDOWS32
  FILETYPE        VFT_APP
  FILESUBTYPE     VFT2_UNKNOWN
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "0c0904b0"
        BEGIN
            VALUE "CompanyName",      MANUFACTURER_TEXT "\0"
            VALUE "FileDescription",  PRODUCT_NAME_TEXT "\0"
            VALUE "FileVersion",      VERSION_STRING "\0"
            VALUE "InternalName",     EXE_NAME_TEXT "\0"
            VALUE "LegalCopyright",   "Copyright © " COPYRIGHT_HOLDER " 2001\0"
            VALUE "OriginalFilename", EXE_NAME_TEXT ".exe\0"
            VALUE "ProductName",      PRODUCT_NAME_TEXT "\0"
            VALUE "ProductVersion",   VERSION_STRING "\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0xc09, 1200
    END
END

#else

PHTTPServiceProcess::Info ProductInfo = {
    PRODUCT_NAME_TEXT,
    MANUFACTURER_TEXT,
    MAJOR_VERSION, MINOR_VERSION, PProcess::BUILD_TYPE, BUILD_NUMBER, __TIME__ __DATE__,
    
    {{ 0 }}, { NULL }, 0, {{ 0 }},  // Only relevent for commercial apps

    HOME_PAGE,
    EMAIL,
    PRODUCT_NAME_HTML,
    NULL,  // GIF HTML, use calculated from below
    GIF_NAME,
    GIF_WIDTH,
    GIF_HEIGHT
};


#endif


// End of File ///////////////////////////////////////////////////////////////
