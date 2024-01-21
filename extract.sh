#!/bin/sh
if [ ! -e "badapple.mp4" ]; then
	echo Needs Bad Apple mp4 w/ 480x360
	exit 1
fi
rm -rf frames
mkdir -p frames
ffmpeg -i badapple.mp4 -r 30 frames/%8d.png
echo enjoy
