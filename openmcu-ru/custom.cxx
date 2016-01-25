
#include "precompile.h"

////////////////////////////////////////////////////
//
// Variables required for PHTTPServiceProcess
//
////////////////////////////////////////////////////

#define	COPYRIGHT_HOLDER	MANUFACTURER_TEXT

#define	EMAIL                   ""
#define	HOME_PAGE               "http://www.openmcu.ru"
#define	PRODUCT_NAME_HTML       PRODUCT_NAME_TEXT

#ifndef GIF_NAME
#define GIF_NAME  		EXE_NAME_TEXT ".gif"
#define	GIF_WIDTH  176
#define GIF_HEIGHT 144
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
    GIF_HEIGHT,

    MANUFACTURER_TEXT,
    HOME_PAGE,
    EMAIL

};


#endif
