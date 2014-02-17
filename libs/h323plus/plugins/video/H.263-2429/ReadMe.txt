Compilation notes
-----------------

Craig Southeren 4 October 2006


The ffmpeg library headers and a compiled version of the ffmpeg shared library
are required to use this codec plugin. These can be found at

Linux
   http://www.voxgratia.org/bin/libavcodec-0.4.7-linux.bin.tar.gz

Windows
   http://www.voxgratia.org/bin/libavcodec-0.4.7-windows.bin.zip

Put the contents of the correct archive file into a subdirectory called
ffmpeg. The makefile should detect this and correctly compile the plugin

Note that the libavcodec.so or libavcodec.dll file must be put into the same
directory as the rest of the plugins. Failure to do this will cause the plugin
to be inoperable.

If you insist on compiling ffmpeg from source, then read the instructions at
http://www.voxgratia.org/docs/h263_codec.html

   ----------------
