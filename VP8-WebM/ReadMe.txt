WebM Project VP8 Video Codec
----------------------------

For Windows, go to http://www.webmproject.org/, downloads link and get a ZIP
file that looks something like vpx-vp8-debug-src-x86-win32mt-vs9-v1.1.0.zip
and unpack it to this directory, rename it to just vpx-vp8.

Things should now build, if not, good luck finding out why!


For Unix variants, it is expected that libvpx is installed via whatever the
package system that is in use, e.g. "sudo yum install libvpx-devel", etc. The
configure script should then find it.

Again, if things do not build, figure it out and send patches!
