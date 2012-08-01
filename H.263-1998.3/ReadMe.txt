Building and installing FFmpeg on Windows
=========================================

Most importantly, don't try to compile FFMpeg for Windows unless you enjoy
pain.

Seriously. Many have tried and failed.

The best way to use FFMpeg on Windows is to use a precompiled shared library
version from the Unoffical FFmpeg Win32 Build site
http://ffmpeg.arrozcru.org/builds/

The recommended version is r16537, which is also available to download from
http://www.opalvoip.org/bin/ffmpeg-r16537-gpl-lshared-win32.tar.bz2

If you simply must compile it yourself, then read on below...


Using a precompiled version of FFmpeg
-------------------------------------

Method 1

   * Download and unpack the pre-compiled tarball into
        ...\opal\plugins\video\H.263=1998\ffmpeg

   * Copy the contents of ...\opal\plugins\video\H.263=1998\ffmpeg\bin to
     C:\PTLib_PlugIns

   * Compile the H.263-1998 project in the Opal plugins solution

Method 2
   
   * Download and unpack the pre-compiled tarball to anywhere you like. The
     recommended location is C:\FFMPEG. The remainder of these instructions
     will refer to this directory as %FFMPEG%

   * Add the directory %FFMPEG%\BIN to the PATH environment variable

   * Open MSVC and add the directory %FFMPEG%\LIB to the VC++ Lib directories
     in the Tools->Options menu

   * Compile the H.263-1998 project in the Opal plugins solution


For hard-core fanatics only.
----------------------------

FFmpeg can only be built on Windows using the Msys+MingW toolchain. The FFMpeg
on Windows Wiki gives detailed instructions on how to do this.

IMPORTANT: You will need to add "c:\msys" to the
           VSNET2005_PWLIB_CONFIGURE_EXCLUDE_DIRS environment variable to
           avoid Opal configure from attempting to use the new header files.

Note 1: If you experience a "hang" while running the "./configure" for FFmpeg,
        try opening the Task Manager and killing the "lvprcsrv.exe" process.
        This is apparently a known issue when compiling with the MingW
        toolchain.

Note 2: If the compile fails with errors relating to "vfw", add the
        "--disable-devices" option when confguring FFmpeg.
