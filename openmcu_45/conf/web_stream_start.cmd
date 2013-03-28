@set frame=704x576
@set framerate=10
@
@echo off
set room=%1
if "%room%"=="" set room=room101

ffmpeg -y^
 -f s16le -ac 1 -ar 16000 -i \\.\pipe\sound_%room%^
 -f rawvideo -r %framerate% -s %frame% -i \\.\pipe\video_%room%^
 -f mpegts^
 -acodec mp2 -ab 32k^
 -vcodec mpeg2video -b 384k^
 - | "%ProgramFiles%\VideoLAN\VLC\vlc.exe" - --sout=#standard{access=http,mux=ts,dst=:8090}
