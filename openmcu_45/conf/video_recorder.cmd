@echo off
if "%1" == "" (
 echo Usage: video_recorder ROOM_NAME
 echo eg. video_recorder room101
 exit /b 1
)
set rname=%date:~6,4%-%date:~3,2%-%date:~0,2%_%time:~0,2%-%time:~3,2%_%1

ffmpeg -y^
 -f s16le -ac 1 -ar 16000 -i \\.\pipe\sound_%1^
 -f rawvideo -r 10 -s 4cif -i \\.\pipe\video_%1^
 -f asf -acodec pcm_s16le -vcodec msmpeg4v2^
 "%rname%.asf"
