namespace Direct3DViewer.ImageBuffer
{
    public interface IImageInfo
    {
    }

    public interface IYUVImageInfo : IImageInfo
    {
        int YStride { get; }

        int YSize { get; }

        int YHeight { get; }
    }

    public abstract class AbstractYUVImageInfo : IYUVImageInfo
    {
        public int YStride { get; }

        public int YSize { get; protected set; }

        public int YHeight { get; }

  

        public AbstractYUVImageInfo(int yStride, int yHeight)
        {
            YStride = yStride;
            YHeight = yHeight;
            YSize = YStride * YHeight;
        }
    }

    public class YUVI420ImageInfo : AbstractYUVImageInfo
    {
        public int UVSize { get; }

        public int UVHeight { get; }

        public int UVStride { get; }

        public YUVI420ImageInfo(int yStride, int yHeight) : base(yStride, yHeight)
        {
            UVStride = YStride >> 1;
            UVHeight = YHeight >> 1;
            UVSize = YSize >> 2;
        }
    }

    public class NV12ImageInfo : AbstractYUVImageInfo
    {
        public int UVSize { get; }

        public int UVHeight { get; }

        public int UVStride { get; }

        public NV12ImageInfo(int yStride, int yHeight) : base(yStride, yHeight)
        {
            UVStride = YStride;
            UVHeight = YHeight >> 1;
            UVSize = YSize >> 1;
        }
    }

    public class YUY2ImageInfo : AbstractYUVImageInfo
    {
        public YUY2ImageInfo(int width, int height) : base(width * 2, height)
        {
        }
    }
}
