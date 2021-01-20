using System;
using System.Runtime.InteropServices;

namespace Direct3DViewer
{
    [StructLayout(LayoutKind.Sequential), Serializable]
    public struct VideoFrame
    {
        public int width;
        public int height;
        public int length;
        public int index;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public IntPtr[] data;
    }
}
