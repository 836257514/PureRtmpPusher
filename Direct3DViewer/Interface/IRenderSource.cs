using System;

namespace Direct3DViewer.Interface
{
    interface IRenderSource : IDisposable
    {
        bool IsDeviceCreated { get; }

        event EventHandler ImageSourceChanged;

        bool CheckFormat(FrameFormat format);

        bool SetupSurface(int videoWidth, int videoHeight, FrameFormat format);

        void Render(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer);
    }
}
