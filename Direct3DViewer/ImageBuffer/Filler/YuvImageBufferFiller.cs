using Direct3DViewer.ImageBuffer;
using SharpDX;
using System;

namespace Direct3DViewer
{
    public class YuvImageBufferFiller
    {
        public static void FillYV12(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer, DataRectangle rect, YUVI420ImageInfo bufferInfo)
        {
            IntPtr surfaceBufferPtr = rect.DataPointer;
            if (rect.Pitch == bufferInfo.YStride)
            {
                Interop.Memcpy(surfaceBufferPtr, yBuffer, bufferInfo.YSize); // Y
                surfaceBufferPtr += bufferInfo.YSize;
                Interop.Memcpy(surfaceBufferPtr, vBuffer, bufferInfo.UVSize); // V 
                surfaceBufferPtr += bufferInfo.UVSize;
                Interop.Memcpy(surfaceBufferPtr, uBuffer, bufferInfo.UVSize); // U
            }
            else
            {
                IntPtr yPtr = yBuffer; // Y
                int yPitch = rect.Pitch;
                for (int i = 0; i < bufferInfo.YHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, yPtr, bufferInfo.YStride);
                    surfaceBufferPtr += yPitch;
                    yPtr += bufferInfo.YStride;
                }

                int uvPitch = yPitch >> 1;

                IntPtr vPtr = vBuffer; // V
                for (int i = 0; i < bufferInfo.UVHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, vPtr, bufferInfo.UVStride);
                    surfaceBufferPtr += uvPitch;
                    vPtr += bufferInfo.UVStride;
                }

                IntPtr uPtr = uBuffer; // U
                for (int i = 0; i < bufferInfo.UVHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, uPtr, bufferInfo.UVStride);
                    surfaceBufferPtr += uvPitch;
                    uPtr += bufferInfo.UVStride;
                }
            }
        }

        public static void FillYU12(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer, DataRectangle rect, YUVI420ImageInfo bufferInfo)
        {
            IntPtr surfaceBufferPtr = rect.DataPointer;
            if (rect.Pitch == bufferInfo.YStride)
            {
                Interop.Memcpy(surfaceBufferPtr, yBuffer, bufferInfo.YSize); // Y
                surfaceBufferPtr += bufferInfo.YSize;
                Interop.Memcpy(surfaceBufferPtr, uBuffer, bufferInfo.UVSize); //U 
                surfaceBufferPtr += bufferInfo.UVSize;
                Interop.Memcpy(surfaceBufferPtr, vBuffer, bufferInfo.UVSize); // V
            }
            else
            {
                IntPtr yPtr = yBuffer; // Y
                int yPitch = rect.Pitch;
                for (int i = 0; i < bufferInfo.YHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, yPtr, bufferInfo.YStride);
                    surfaceBufferPtr += yPitch;
                    yPtr += bufferInfo.YStride;
                }

                int uvPitch = yPitch >> 1;

                IntPtr uPtr = uBuffer; // U
                for (int i = 0; i < bufferInfo.UVHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, uPtr, bufferInfo.UVStride);
                    surfaceBufferPtr += uvPitch;
                    uPtr += bufferInfo.UVStride;
                }

                IntPtr vPtr = uBuffer; // V
                for (int i = 0; i < bufferInfo.UVHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, vPtr, bufferInfo.UVStride);
                    surfaceBufferPtr += uvPitch;
                    vPtr += bufferInfo.UVStride;
                }
            }
        }

        public static void FillNV12(IntPtr yBuffer, IntPtr uBuffer, DataRectangle rect, NV12ImageInfo bufferInfo)
        {
            IntPtr surfaceBufferPtr = rect.DataPointer;
            if (rect.Pitch == bufferInfo.YStride)
            {
                Interop.Memcpy(surfaceBufferPtr, yBuffer, bufferInfo.YSize); // Copy Y数据
                surfaceBufferPtr += bufferInfo.YSize;
                Interop.Memcpy(surfaceBufferPtr, uBuffer, bufferInfo.UVSize); // Copy UV打包数据
            }
            else
            {
                // Copy Y数据
                IntPtr yPtr = yBuffer;
                for (int i = 0; i < bufferInfo.YHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, yPtr, bufferInfo.YStride);
                    surfaceBufferPtr += rect.Pitch;
                    yPtr += bufferInfo.YStride;
                }

                // Copy UV打包数据
                IntPtr uvPtr = uBuffer;
                for (int i = 0; i < bufferInfo.UVHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, uvPtr, bufferInfo.UVStride);
                    surfaceBufferPtr += rect.Pitch; // 此时uv打包保存，每行数据与y相同
                    uvPtr += bufferInfo.UVStride;
                }
            }
        }

        public static void FillYUY2(IntPtr yBuffer, DataRectangle rect, YUY2ImageInfo bufferInfo)
        {
            IntPtr surfaceBufferPtr = rect.DataPointer;
            if (rect.Pitch == bufferInfo.YStride)
            {
                Interop.Memcpy(surfaceBufferPtr, yBuffer, bufferInfo.YSize);
            }
            else
            {
                IntPtr src = yBuffer;
                for (int i = 0; i < bufferInfo.YHeight; i++)
                {
                    Interop.Memcpy(surfaceBufferPtr, src, bufferInfo.YStride);
                    surfaceBufferPtr += rect.Pitch;
                    src += bufferInfo.YStride;
                }
            }
        }
    }
}
