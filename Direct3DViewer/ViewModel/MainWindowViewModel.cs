using Direct3DViewer.Interface;
using Direct3DViewer.SDKWrapper;
using Direct3DViewer.ViewModel.Command;
using System;
using System.Threading.Tasks;
using System.Windows.Input;
using static Direct3DViewer.SDKWrapper.SDK;

namespace Direct3DViewer.ViewModel
{
    class MainWindowViewModel
    {
        private IntPtr _instance;

        private readonly IRenderSource _renderSource;

        private FrameReceived _received;

        public ICommand PlayCommand { get; }

        public ICommand StopCommand { get; }

        public bool EnableHardwareDecode { get; set; }

        public MainWindowViewModel(IRenderSource renderSource)
        {
            PlayCommand = new RelayCommand(StartReceive, "Play");
            StopCommand = new RelayCommand(DestroyInstance, "Stop");
            _received = FrameDecodedCallback;
            _renderSource = renderSource;
        }

        private void DestroyInstance(object e)
        {
            SDK.DestroyInstance(_instance);
            _instance = IntPtr.Zero;
        }

        private void StartReceive(object e)
        {
            if (_instance == IntPtr.Zero)
            {
                _instance = SDK.CreateInstance("rtmp://192.168.6.98:1935/hls/stream", 3000000, InputPixelFormat.NV12, _received);
                Task.Run(() =>
                {
                    StatusCode statusCode = SDK.InitInstance(_instance, EnableHardwareDecode);
                    if (statusCode == StatusCode.Success)
                    {
                        SDK.Receive(_instance);
                    }
                });
            }
        }

        private void FrameDecodedCallback(ref VideoFrame frame)
        {
            if (!_renderSource.IsDeviceCreated)
            {
                _renderSource.SetupSurface(frame.width, frame.height, FrameFormat.NV12);
            }

            _renderSource.Render(frame.data[0], frame.data[1], frame.data[2]);
        }
    }
}
