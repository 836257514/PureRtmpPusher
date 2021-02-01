using System;
using System.Windows.Interop;
using System.Windows;
using System.Windows.Media;
using SharpDX.Direct3D9;
using SharpDX;
using SharpDX.Mathematics.Interop;
using System.Windows.Forms;
using Direct3DViewer.ImageBuffer;
using Direct3DViewer.ImageBuffer.Filler;

namespace Direct3DViewer
{
    public interface IRenderSource : IDisposable
    {
        ImageSource ImageSource { get; }

        event EventHandler ImageSourceChanged;

        bool CheckFormat(FrameFormat format);

        bool SetupSurface(int videoWidth, int videoHeight, FrameFormat format);

        void Render(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer);
    }
    public class D3DImageSource : IRenderSource
    {
        private static Format D3DFormatYU12 = (Format)0x30323449;

        private static Format D3DFormatYV12 = D3DX.MakeFourCC((byte)'Y', (byte)'V', (byte)'1', (byte)'2');
        //private static Format D3DFormatYU12 = D3DX.MakeFourCC((byte)'Y', (byte)'U', (byte)'1', (byte)'2');
        private static Format D3DFormatNV12 = D3DX.MakeFourCC((byte)'N', (byte)'V', (byte)'1', (byte)'2');
        private static RawColorBGRA BlackColor = new RawColorBGRA(0, 0, 0, 0xFF);

        private bool isDisposed = false;
        private D3DImage _d3dImage;
        private Int32Rect _imageRect;
        private Direct3DEx _context;
        private int _adapterId;
        private CreateFlags createFlag;
        private DeviceEx _device;
        private IntPtr _hwnd;
        private Surface _offScreenSurface;
        private Surface _renderSurface;
        private DisplayMode _displayMode;

        // 视频格式信息
        private FrameFormat _frameFormat;

        private IImageInfo _imageBufferInfo;

        public ImageSource ImageSource => _d3dImage;

        public bool IsDeviceAvailable
        {
            get
            {
                return CheckDevice();
            }
        }

        public bool IsDeviceCreated => _device != null;

        public event EventHandler ImageSourceChanged;

        public D3DImageSource() : this(0) { }

        public D3DImageSource(int adapterId)
        {
            _d3dImage = new D3DImage();
            _d3dImage.IsFrontBufferAvailableChanged += OnIsFrontBufferAvailableChanged;
            InitD3D(adapterId);
            var form = new Form();
            _hwnd = form.Handle;
        }

        public bool CheckFormat(FrameFormat format)
        {
            return CheckFormat(ConvertToD3D(format));
        }

        public bool SetupSurface(int videoWidth, int videoHeight, FrameFormat format)
        {
            Format d3dFormat = ConvertToD3D(format);
            if (!CheckFormat(d3dFormat))
            {
                return false;
            }

            _frameFormat = format;
            switch (format)
            {
                case FrameFormat.YV12:
                case FrameFormat.YU12:
                    _imageBufferInfo = new YUVI420ImageInfo(videoWidth, videoHeight);
                    break;
                case FrameFormat.NV12:
                    _imageBufferInfo = new NV12ImageInfo(videoWidth, videoHeight);
                    break;
                case FrameFormat.YUY2:
                case FrameFormat.UYVY:
                    _imageBufferInfo = new YUY2ImageInfo(videoWidth, videoHeight);
                    break;
                case FrameFormat.RGB15: // rgb555
                case FrameFormat.RGB16: // rgb565
                    _imageBufferInfo = new Rgb2ChannelImageInfo(videoWidth, videoHeight);
                    break;
                case FrameFormat.RGB32:
                case FrameFormat.ARGB32:
                    _imageBufferInfo = new Rgba4ChannelImageInfo(videoWidth, videoHeight);
                    break;
                default:
                    return false;
            }
            ReleaseResource();
            CreateResource(d3dFormat, videoWidth, videoHeight);
            ImageSourceChanged?.Invoke(this, EventArgs.Empty);
            return true;
        }

        public void Render(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer)
        {
            FillBuffer(yBuffer, uBuffer, vBuffer);
            StretchSurface();
            if (!_d3dImage.Dispatcher.CheckAccess())
            {
                _d3dImage.Dispatcher.Invoke(() => InvalidateImage());
            }
            else
            {
                InvalidateImage();
            }
        }

        private void InitD3D(int adapterId = 0)
        {
            _adapterId = adapterId;
            _context = new Direct3DEx();
            _displayMode = _context.GetAdapterDisplayMode(adapterId);
            Capabilities deviceCap = _context.GetDeviceCaps(_adapterId, DeviceType.Hardware);
            createFlag = CreateFlags.Multithreaded;
            if (deviceCap.VertexProcessingCaps != 0)
            {
                createFlag |= CreateFlags.HardwareVertexProcessing;
            }
            else
            {
                createFlag |= CreateFlags.SoftwareVertexProcessing;
            }
        }

        private void CreateResource(Format format, int width, int height)
        {
            PresentParameters presentParameters = GetPresentParameters(width, height);
            _device = new DeviceEx(_context, _adapterId, DeviceType.Hardware, _hwnd, createFlag, presentParameters);
            _device.SetRenderState(RenderState.CullMode, Cull.None);
            _device.SetRenderState(RenderState.ZEnable, ZBufferType.DontUseZBuffer);
            _device.SetRenderState(RenderState.Lighting, false);
            _device.SetRenderState(RenderState.DitherEnable, true);
            _device.SetRenderState(RenderState.MultisampleAntialias, true);
            _device.SetRenderState(RenderState.AlphaBlendEnable, true);
            _device.SetRenderState(RenderState.SourceBlend, Blend.SourceAlpha);
            _device.SetRenderState(RenderState.DestinationBlend, Blend.InverseSourceAlpha);
            _device.SetSamplerState(0, SamplerState.MagFilter, TextureFilter.Linear);
            _device.SetSamplerState(0, SamplerState.MinFilter, TextureFilter.Linear);
            _device.SetSamplerState(0, SamplerState.AddressU, TextureAddress.Clamp);
            _device.SetSamplerState(0, SamplerState.AddressV, TextureAddress.Clamp);
            _device.SetTextureStageState(0, TextureStage.ColorOperation, TextureOperation.SelectArg1);
            _device.SetTextureStageState(0, TextureStage.ColorArg1, TextureArgument.Texture);
            _device.SetTextureStageState(0, TextureStage.ColorArg2, TextureArgument.Specular);
            _device.SetTextureStageState(0, TextureStage.AlphaOperation, TextureOperation.Modulate);
            _device.SetTextureStageState(0, TextureStage.AlphaArg1, TextureArgument.Texture);
            _device.SetTextureStageState(0, TextureStage.AlphaArg2, TextureArgument.Diffuse);
            _renderSurface = Surface.CreateRenderTargetEx(_device, width, height, _displayMode.Format, MultisampleType.None, 0, true, Usage.None);
            _offScreenSurface = Surface.CreateOffscreenPlainEx(_device, width, height, format, Pool.Default, Usage.None);
            _device.ColorFill(_offScreenSurface, BlackColor);
            SetImageSourceBackBuffer();
        }

        private void ReleaseResource()
        {
            _renderSurface?.Dispose();
            _offScreenSurface?.Dispose();
            _device?.Dispose();
        }

        private PresentParameters GetPresentParameters(int width, int height)
        {
            PresentParameters presentParams = new PresentParameters();
            presentParams.PresentFlags = PresentFlags.Video | PresentFlags.OverlayYCbCrBt709;
            presentParams.Windowed = true;
            presentParams.DeviceWindowHandle = _hwnd;
            presentParams.BackBufferWidth = width == 0 ? 1 : width;
            presentParams.BackBufferHeight = height == 0 ? 1 : height;
            presentParams.SwapEffect = SwapEffect.Discard;
            presentParams.PresentationInterval = PresentInterval.Immediate;
            presentParams.BackBufferFormat = _displayMode.Format;
            presentParams.BackBufferCount = 1;
            presentParams.EnableAutoDepthStencil = false;
            return presentParams;
        }

        private void FillBuffer(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer)
        {
            if (_offScreenSurface == null)
            {
                return;
            }

            DataRectangle rect = _offScreenSurface.LockRectangle(LockFlags.None);
            switch (_frameFormat)
            {
                case FrameFormat.YV12:
                    YuvImageBufferFiller.FillYV12(yBuffer, uBuffer, vBuffer, rect, (YUVI420ImageInfo)_imageBufferInfo);
                    break;
                case FrameFormat.YU12:
                    YuvImageBufferFiller.FillYU12(yBuffer, uBuffer, vBuffer, rect, (YUVI420ImageInfo)_imageBufferInfo);
                    break;
                case FrameFormat.NV12:
                    YuvImageBufferFiller.FillNV12(yBuffer, uBuffer, rect, (NV12ImageInfo)_imageBufferInfo);
                    break;
                case FrameFormat.YUY2:
                case FrameFormat.UYVY:
                    YuvImageBufferFiller.FillYUY2(yBuffer, rect, (YUY2ImageInfo)_imageBufferInfo);
                    break;
                case FrameFormat.RGB15:
                case FrameFormat.RGB16:
                case FrameFormat.RGB32:
                case FrameFormat.ARGB32:
                default:
                    ChannelImageBufferFiller.FillBuffer(yBuffer, rect, (IChannelImageInfo)_imageBufferInfo);
                    break;
            }

            _offScreenSurface.UnlockRectangle();
        }

        private void StretchSurface()
        {
            //_device.ColorFill(_renderSurface, BlackColor);

            _device.StretchRectangle(_offScreenSurface, _renderSurface, TextureFilter.Linear);
        }

        private bool CheckDevice()
        {
            DeviceState state = _device.CheckDeviceState(_hwnd);
            return state == DeviceState.Ok;
        }

        private void OnIsFrontBufferAvailableChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (_d3dImage.IsFrontBufferAvailable && _renderSurface != null)
            {
                _d3dImage.Lock();
                _d3dImage.SetBackBuffer(D3DResourceType.IDirect3DSurface9, _renderSurface.NativePointer);
                _d3dImage.Unlock();
            }
        }

        private void SetImageSourceBackBuffer()
        {
            if (!_d3dImage.Dispatcher.CheckAccess())
            {
                _d3dImage.Dispatcher.Invoke(() => SetImageSourceBackBuffer());
                return;
            }
            _d3dImage.Lock();
            _d3dImage.SetBackBuffer(D3DResourceType.IDirect3DSurface9, _renderSurface.NativePointer);
            _d3dImage.Unlock();
            _imageRect = new Int32Rect(0, 0, _d3dImage.PixelWidth, _d3dImage.PixelHeight);
        }

        private void InvalidateImage()
        {
            if (!_d3dImage.IsFrontBufferAvailable)
            {
                return;
            }

            try
            {
                _d3dImage.Lock();
                _d3dImage.AddDirtyRect(_imageRect);
                _d3dImage.Unlock();
            }
            catch
            {
                return;
            }
        }

        private bool CheckFormat(Format d3dFormat)
        {
            if (!_context.CheckDeviceFormat(_adapterId, DeviceType.Hardware, _displayMode.Format, Usage.None, ResourceType.Surface, d3dFormat))
            {
                return false;
            }

            return _context.CheckDeviceFormatConversion(_adapterId, DeviceType.Hardware, d3dFormat, _displayMode.Format);

        }
        private static Format ConvertToD3D(FrameFormat format)
        {
            switch (format)
            {
                case FrameFormat.YV12:
                    return D3DFormatYV12;
                case FrameFormat.YU12:
                    return D3DFormatYU12;
                case FrameFormat.NV12:
                    return D3DFormatNV12;
                case FrameFormat.YUY2:
                    return Format.Yuy2;
                case FrameFormat.UYVY:
                    return Format.Uyvy;
                case FrameFormat.RGB15:
                    return Format.X1R5G5B5;
                case FrameFormat.RGB16:
                    return Format.R5G6B5;
                case FrameFormat.RGB32:
                    return Format.X8R8G8B8;
                case FrameFormat.ARGB32:
                    return Format.A8R8G8B8;
                case FrameFormat.RGB24:
                    return Format.R8G8B8;
                default:
                    throw new ArgumentException("Unknown pixel format", "format");
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!isDisposed)
            {
                isDisposed = true;

                if (disposing)
                {
                    ReleaseResource();
                    _context?.Dispose();
                }
            }
        }
    }
}
