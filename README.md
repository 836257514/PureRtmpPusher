# PureRtmpPusher
The ffmpeg rtmp push demo support h264_qsv hardware encode and software encode

rtmp推流客户端，支持h264软编码和QSV硬编码

The direct 3d viewer use the sharpdx wrapper to display yuvi420 data and support h264_qsv decode.

rtmp播放器，基于Sharpdx包装的Direct3d9制作，支持yuvi420和nv12，rgba等多种格式，支持QSV硬解码

1.Select the folder named 'pusher' under solution and choose the RtmpPusher.cpp then change the push stream url by yourself.

2.Select the folder named 'player' under solution and in the MainWindow.xaml.cs then change the pull stream url to your own.
