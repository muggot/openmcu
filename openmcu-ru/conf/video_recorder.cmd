@set frame=704x576
@set framerate=10
@
@echo off
set room=%1
if "%room%"=="" set room=room101

set rname=%date:~6,4%-%date:~3,2%-%date:~0,2%_%time:~0,2%-%time:~3,2%_%room%

ffmpeg -y^
 -f s16le -ac 1 -ar 16000 -i \\.\pipe\sound_%room%^
 -f rawvideo -r %framerate% -s %frame% -i \\.\pipe\video_%room%^
 -f asf -acodec pcm_s16le -vcodec msmpeg4v2^
 "%rname%.asf"
