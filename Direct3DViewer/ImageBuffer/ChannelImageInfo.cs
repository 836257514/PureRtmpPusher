namespace Direct3DViewer.ImageBuffer
{
    public interface IChannelImageInfo : IImageInfo
    {
        int Stride { get; }

        int Height { get; }

        int Size { get; }
    }

    public abstract class AbastractChannelImageInfo : IChannelImageInfo
    {
        public int Stride { get; }

        public int Height { get; }

        public int Size { get; }

        public AbastractChannelImageInfo(int width, int height, int channel)
        {
            Stride = width * channel;
            Height = height;
            Size = Stride * Height;
        }
    }

    public class Rgba4ChannelImageInfo : AbastractChannelImageInfo
    {
        public Rgba4ChannelImageInfo(int width, int height) : base(width, height, 4)
        {
        }
    }

    public class Rgb2ChannelImageInfo : AbastractChannelImageInfo
    {
        public Rgb2ChannelImageInfo(int width, int height) : base(width, height, 2)
        {
        }
    }
}
