#include "stdafx.h"
#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4018)


#include <windows.h>
#include "tPixel.h"
#include <fcntl.h>
#include <vector>

struct MIPMapData
{
	size_t mipLevel;
	size_t width;
	size_t height;
	int faceNumber; // current face number for this image
	int numFaces;   // total number of faces (depth for volume textures, 6 for cube maps)
};


typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned long	DWORD;

struct DDS_PIXELFORMAT
 {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
	};

struct DDS_HEADER
 {
	DWORD dwSize;
	DWORD dwHeaderFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwPitchOrLinearSize;
	DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
	DWORD dwMipMapCount;
	DWORD dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD dwSurfaceFlags;
	DWORD dwCubemapFlags;
	DWORD dwReserved2[3];
	};

#include "nvtt.h"

#pragma warning(pop)
#include "DXT.h"

BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

static HFILE gFileOut;

using namespace nvtt;

struct dds_writer : public OutputHandler 
{
	dds_writer(HFILE& fileout);

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel);
	virtual bool writeData(const void* data, int size);
	virtual void endImage();
	HFILE& w;
};

inline dds_writer::dds_writer(HFILE& _fileout) : w(_fileout) {}

void dds_writer::beginImage(int size, int width, int height, int depth, int face, int miplevel) {}
void dds_writer::endImage() {}

bool dds_writer::writeData(const void* data, int size)
{
	if (size == sizeof(DDS_HEADER))
	{
		// correct DDS header
		DDS_HEADER* hdr = (DDS_HEADER*)data;

		if (hdr->dwSize == size)
			hdr->ddspf.dwRGBBitCount = 0;		
	}

	_write(w, data, size);
	return true;
}


int DXTCompressImage(LPCSTR out_name, u8* raw_data, u32 w, u32 h,bool useRgba)
{
	R_ASSERT((0 != w) && (0 != h));

	gFileOut = _open(out_name, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC, _S_IWRITE);
	
	if (gFileOut == -1) 
	{
		fprintf(stderr, "Can't open output file %s\n", out_name);
		return 1;
	}

	bool result = false;

	InputOptions in_opts;

	in_opts.setTextureLayout(TextureType_2D, w, h);
	in_opts.setMipmapGeneration(false);
	in_opts.setMipmapFilter(MipmapFilter_Box);
	in_opts.setWrapMode(WrapMode_Clamp);
	in_opts.setNormalMap(false);
	in_opts.setConvertToNormalMap(false);
	in_opts.setGamma(2.2F, 2.2F);
	in_opts.setNormalizeMipmaps(false);

	CompressionOptions comp_opts;

	comp_opts.setFormat(useRgba? Format_RGBA:Format_DXT5); 
	comp_opts.setQuality(Quality_Highest);
	comp_opts.setQuantization(false, false, false);

	HFILE fileout = _open(out_name, _O_WRONLY | _O_BINARY | _O_CREAT | _O_TRUNC, _S_IWRITE);
	
	if (fileout == -1) 
	{
		fprintf(stderr, "Can't open output file %s\n", out_name);
		return 2;
	}

	OutputOptions out_opts;

	dds_writer dds(gFileOut);
	out_opts.setOutputHandler(&dds);
	
	RGBAImage pImage(w, h);
	rgba_t* pixels = pImage.pixels();
		
	u8* pixel = raw_data;
	
	for (u32 k = 0; k < w * h; k++, pixel += 4)
		pixels[k].set(pixel[0], pixel[1], pixel[2], pixel[3]);
	
	in_opts.setMipmapData(pixels, w, h);
	result = Compressor().process(in_opts, comp_opts, out_opts);
	
	_close(gFileOut);

	if (result == false) 
	{
		unlink(out_name);
		return 3;
	}
	else					
		return 4;
}

void DXTCompress(LPCSTR out_name, u8* raw_data, u32 w, u32 h, bool useRgba)
{
	int i = DXTCompressImage(out_name, raw_data, w, h,useRgba);

	Msg("result %i",i);
}
