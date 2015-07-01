#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "setjmp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>

#define FB_DEVICE_NAME "/dev/fb0"


static int FBDeviceInit(void);
static int FBShowPixel(int iX, int iY, unsigned int dwColor);
static int FBCleanScreen(unsigned int dwBackColor);


static int g_fd;

static struct fb_var_screeninfo g_tFBVar;
static struct fb_fix_screeninfo g_tFBFix;			
static unsigned char *g_pucFBMem;
static unsigned int g_dwScreenSize;

static unsigned int g_dwLineWidth;
static unsigned int g_dwPixelWidth;

static int FBDeviceInit(void)
{
	int ret;
	
	g_fd = open(FB_DEVICE_NAME, O_RDWR);
	if (0 > g_fd)
	{
		printf("can't open %s\n", FB_DEVICE_NAME);
	}

	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &g_tFBVar);
	if (ret < 0)
	{
		printf("can't get fb's var\n");
		return -1;
	}

	ret = ioctl(g_fd, FBIOGET_FSCREENINFO, &g_tFBFix);
	if (ret < 0)
	{
		printf("can't get fb's fix\n");
		return -1;
	}
	
	g_dwScreenSize = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;
	g_pucFBMem = (unsigned char *)mmap(NULL , g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fd, 0);
	if (0 > g_pucFBMem)	
	{
		printf("can't mmap\n");
		return -1;
	}

	g_dwLineWidth  = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;
	g_dwPixelWidth = g_tFBVar.bits_per_pixel / 8;
	
	return 0;
}


static int FBShowPixel(int iX, int iY, unsigned int dwColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	if ((iX >= g_tFBVar.xres) || (iY >= g_tFBVar.yres))
	{
		printf("out of region\n");
		return -1;
	}

	pucFB      = g_pucFBMem + g_dwLineWidth * iY + g_dwPixelWidth * iX;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;
	
	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			*pucFB = (unsigned char)dwColor;
			break;
		}
		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;

//			iRed   = 0x00;
//			iGreen = 0x00;
//			iBlue  = 0xFF;
			
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwFB16bpp	= wColor16bpp;
			break;
		}
		case 32:
		{
			*pdwFB32bpp = dwColor;
			break;
		}
		default :
		{
			printf("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}

static int FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucFB      = g_pucFBMem;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;

	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			memset(g_pucFBMem, dwBackColor, g_dwScreenSize);
			break;
		}
		case 16:
		{
			iRed   = (dwBackColor >> (16+3)) & 0x1f;
			iGreen = (dwBackColor >> (8+2)) & 0x3f;
			iBlue  = (dwBackColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < g_dwScreenSize)
			{
				*pwFB16bpp	= wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < g_dwScreenSize)
			{
				*pdwFB32bpp	= dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			printf("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}

static int FBShowLine(int iXStart, int iXLen, int iY, unsigned char *pucRGBArray)
{
	int i = iXStart * 3;
	int iX;
	unsigned int dwColor;

	if (iY >= g_tFBVar.yres)
		return -1;

	if (iXStart >= g_tFBVar.xres)
		return -1;

	if (iXStart + iXLen >= g_tFBVar.xres)
	{
		iXLen = g_tFBVar.xres - iXStart;
	}
	
	for (iX = iXStart; iX < iXLen; iX++)
	{
		/* 0xRRGGBB */
		dwColor = (pucRGBArray[i]<<16) + (pucRGBArray[i+1]<<8) + (pucRGBArray[i+2]<<0);
		i += 3;
		FBShowPixel(iX, iY, dwColor);
	}
	return 0;	
}

#if 0
        Allocate and initialize a JPEG decompression object
        Specify the source of the compressed data (eg, a file)
        Call jpeg_read_header() to obtain image info
        Set parameters for decompression
        jpeg_start_decompress(...);
        while (scan lines remain to be read)
                jpeg_read_scanlines(...);
        jpeg_finish_decompress(...);
        Release the JPEG decompression object
#endif

/* Uage: jpg2rgb <jpg_file>
 */
int main(int argc, char *argv[])
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * infile;
	int row_stride;
	unsigned char *buffer;

	if(argc != 2)
	{
		printf("Usage: %s <jpeg_file>\n", argv[0]);
		return -1;
	}

	if(FBDeviceInit())
	{
		printf("FBDeviceInit error\n");
		return -1;
	}

	FBCleanScreen(0);

	/* 分配和初始化一个decompression结构体 */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* 指定源文件 */
	if ((infile = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

	/* 获取图像信息 */
#if 0
	JDIMENSION image_width                  Width and height of image
	JDIMENSION image_height
	int num_components                      Number of color components
	J_COLOR_SPACE jpeg_color_space          Colorspace of image
	boolean saw_JFIF_marker                 TRUE if a JFIF APP0 marker was seen
	UINT8 JFIF_major_version              Version information from JFIF marker
	UINT8 JFIF_minor_version
	UINT8 density_unit                    Resolution data from JFIF marker
	UINT16 X_density
	UINT16 Y_density
	boolean saw_Adobe_marker                TRUE if an Adobe APP14 marker was seen
	UINT8 Adobe_transform                 Color transform code from Adobe marker
#endif
	jpeg_read_header(&cinfo, TRUE);
	/* 源信息 */
	printf("image_width = %d\n", cinfo.image_width);
	printf("image_height = %d\n", cinfo.image_height);
	printf("num_components = %d\n", cinfo.num_components);
	
	/* 设置解压参数，比如放大，缩小 */
	/* 缩放系数是scale_num / scale_denom */
	printf("Enter M/N\n");
	scanf("%d %d", &cinfo.scale_num, &cinfo.scale_denom);
	printf("Scaled to %d/%d\n", cinfo.scale_num, cinfo.scale_denom);

	/* 启动解压 */
	jpeg_start_decompress(&cinfo);
#if 0
	JDIMENSION output_width         Actual dimensions of output image.
	JDIMENSION output_height
	int out_color_components        Number of color components in out_color_space.
	int output_components           Number of color components returned.
	int rec_outbuf_height           Recommended height of scanline buffer.
#endif
	printf("output_width = %d\n", cinfo.output_width);
	printf("output_height = %d\n", cinfo.output_height);
	printf("num_components = %d\n", cinfo.output_components);

	// 一行数据的字节数
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = malloc(row_stride);

	if(buffer == NULL)
	{
		printf("buf malloc error\n");
		return -1;
	}

	/* 逐行获得解压数据 */
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could ask for
		* more than one scanline at a time if that's more convenient.
		*/
		/* jpeg_read_scanlines()的第二个参数是char **，即是一个二维数组的首地址
		 * 使用一维数组时，需要对buffer再次取址
		 */
		(void) jpeg_read_scanlines(&cinfo, &buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
//		put_scanline_someplace(buffer[0], row_stride);

		// 写到LCD去
		FBShowLine(0, cinfo.output_width, cinfo.output_scanline, buffer);
	}

	/* 停止解压 */
	jpeg_finish_decompress(&cinfo);

	/* 释放decompression结构体 */
	jpeg_destroy_decompress(&cinfo);

	return 0;
}