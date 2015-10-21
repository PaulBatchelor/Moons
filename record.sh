#ffmpeg -video_size 1024x768 -framerate 25 -y -f x11grab -i :0.0+0,0 output.mp4

./moons &
#ffmpeg -loglevel quiet -video_size 1024,768 -framerate 24 -y -f x11grab -i :0.0+0,0 -f jack -i ffmpeg output.mkv &
#ffmpeg -loglevel quiet -video_size 1919x1079 -framerate 60 -y -f x11grab -i :0.0+0,0 -f jack -i ffmpeg output.mkv &
ffmpeg -loglevel quiet -video_size 1919x1079 -framerate 60 -y -f x11grab -i :0.0+0,0 output.mkv &
sleep 2
#jack_connect "RtApiJack:outport 0" "ffmpeg:input_1"
#jack_connect "RtApiJack:outport 1" "ffmpeg:input_2"
sleep 10
killall ffmpeg

