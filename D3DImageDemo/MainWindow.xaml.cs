using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;

namespace D3DImageDemo
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private delegate void FrameReceived(ref VideoFrame frame);

        D3DImageSource _imageSource = new D3DImageSource();

        private FrameReceived _received;

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_create_instance")]
        private static extern IntPtr CreateInstance(string streamUrl, int timeOut, ConvertPixelFormat targetPixelFormat, FrameReceived frame);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_init_instance")]
        private static extern StatusCode InitInstance(IntPtr instance);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_receive")]
        private static extern void Receive(IntPtr instance);

        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnWindowLoaded;
        }

        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            _received = FrameDecodedCallback;
            imgelt.Source = _imageSource.ImageSource;
            IntPtr instance = CreateInstance("rtmp://liveplay.fis.plus/live/7f61306b4eb14f67b82d747da3599623", 3000000, ConvertPixelFormat.YuvI420P, _received);
            Task.Run(() =>
            {
                StatusCode statusCode = InitInstance(instance);
                if (statusCode == StatusCode.Success)
                {
                    Receive(instance);
                }
            });

        }

        private void FrameDecodedCallback(ref VideoFrame frame)
        {
            if (!_imageSource.IsDeviceCreated)
            {
                _imageSource.SetupSurface(frame.width, frame.height, FrameFormat.YV12);
            }

            _imageSource.Render(frame.data[0], frame.data[1], frame.data[2]);
        }
    }
}
