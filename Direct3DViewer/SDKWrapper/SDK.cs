using Direct3DViewer.Interface;
using System;
using System.Runtime.InteropServices;

namespace Direct3DViewer.SDKWrapper
{
    public class SDK
    {
        public delegate void FrameReceived(ref VideoFrame frame);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_create_instance")]
        public static extern IntPtr CreateInstance(string streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frame);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_init_instance")]
        public static extern StatusCode InitInstance(IntPtr instance, bool enableHardwareDecode = false);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_keep_receive")]
        public static extern void KeepReceive(IntPtr instance);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_destroy_instance")]
        public static extern void DestroyInstance(IntPtr instance);
    }
}
