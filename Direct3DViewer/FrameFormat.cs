using System.Runtime.InteropServices;

namespace Direct3DViewer
{
	public enum FrameFormat
	{
		YV12 = 0,
		NV12 = 1,
		YUY2 = 2,
		UYVY = 3,

		RGB15 = 10,
		RGB16 = 11,
		RGB24 = 12,  //D3D通常不支持RGB24
		RGB32 = 13,
		ARGB32 = 14
	}

	[StructLayout(LayoutKind.Sequential)]
	struct VERTEX
	{
		public Vector3 Pos;        // vertex untransformed position
		public uint Color;         // diffuse color
		public Vector2 UV;     // texture relative coordinates
	};

	struct Vector3
	{
		public int X;
		public int Y;
		public int Z;

		public Vector3(int x, int y, int z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	}

	struct Vector2
	{
		public int U;
		public int V;

		public Vector2(int u, int v)
		{
			U = u;
			V = v;
		}
	}

	public enum StatusCode
	{
		Success,
		OpenInputFail,
		FindStreamInfoFail,
		CanNotFindVideoStream,
		CanNotFindDecoder,
		CodecParameterToContextFail,
		DecoderToContextFail
	};

	public enum InputPixelFormat
	{
		YuvI420P,
		NV12,
		BGR24,
		BGRA
	};

}
