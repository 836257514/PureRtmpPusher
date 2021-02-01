using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;

namespace Direct3DViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private delegate void FrameReceived(ref VideoFrame frame);

        D3DImageSource _imageSource = new D3DImageSource();

        private FrameReceived _received;

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_create_instance")]
        private static extern IntPtr CreateInstance(string streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frame);

        [DllImport("StreamDecoder.dll", EntryPoint = "sd_init_instance")]
        private static extern StatusCode InitInstance(IntPtr instance, bool enableHardwareDecode = false);

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
            imgHoster.Source = _imageSource.ImageSource;
            IntPtr instance = CreateInstance("rtmp://192.168.6.98:1935/hls/stream", 3000000, InputPixelFormat.NV12, _received);
            Task.Run(() =>
            {
                StatusCode statusCode = InitInstance(instance, true);
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
                _imageSource.SetupSurface(frame.width, frame.height, FrameFormat.NV12);
            }

            _imageSource.Render(frame.data[0], frame.data[1], frame.data[2]);
        }
    }
}
