# PureRtmpPusher
The ffmpeg rtmp push demo support h264 hardware encode and software encode

rtmp推流客户端，支持h264软编码和QSV硬编码

The direct 3d viewer use the sharpdx wrapper to display yuvi420 data.

使用Sharpdx制作的播放器，支持yuvi420和nv12，rgba等多种格式

1.Select the folder named 'pusher' under solution and choose the RtmpPusher.cpp then change the push stream url by yourself.

2.Select the folder named 'player' under solution and in the MainWindow.xaml.cs then change the pull stream url to your own.
