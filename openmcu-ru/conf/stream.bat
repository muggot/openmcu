#!/bin/bash
rname=$(date +"%F")
./ffmpeg -y -f s16le -acodec pcm_s16le -ac 1 -ar 16000 -i sound.$1 -f rawvideo -r 10 -s 4cif -i video.$1 -r 11 -f asf -acodec wmav2 -ac 1 -ar 16000 -vcodec msmpeg4v2 -b 512000 "$rname-$1.asf"
#ffmpeg -y  -f s16le -acodec pcm_s16le -ac 1 -ar 8000 -i sound.$1 -i video.$1 -s cif -r 10 -f asf -acodec wmav2 -vcodec msmpeg4v2 test.asf
#./ffmpeg -y -f s16le -acodec pcm_s16le -ac 1 -ar 16000 -i sound.$1 -f wav -acodec pcm_s16le "$rname-$1.wav"
