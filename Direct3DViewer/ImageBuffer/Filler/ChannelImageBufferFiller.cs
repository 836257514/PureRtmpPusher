using SharpDX;
using System;

namespace Direct3DViewer.ImageBuffer.Filler
{
    public class ChannelImageBufferFiller
    {
        public static void FillBuffer(IntPtr srcBuffer, DataRectangle rect, IChannelImageInfo channelImageInfo)
        {
            IntPtr surfaceBufferPtr = rect.DataPointer;
            if (rect.Pitch == channelImageInfo.Stride)
            {
                Interop.Memcpy(surfaceBufferPtr, srcBuffer, channelImageInfo.Size);
            }
            else
            {
                IntPtr src = srcBuffer;
                for (int i = 0; i < channelImageInfo.Height; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, src, channelImageInfo.Stride);
                    surfaceBufferPtr += rect.Pitch;
                    srcBuffer += channelImageInfo.Stride; 
                }
            }
        }
    }
}
