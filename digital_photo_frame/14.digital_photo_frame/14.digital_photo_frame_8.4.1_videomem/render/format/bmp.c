#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#pragma pack(push) /* 将当前pack设置压栈保存 */
#pragma pack(1)    /* 必须在结构体定义之前使用 */

typedef struct tagBITMAPFILEHEADER 
{
	unsigned short bfType;//固定为0x4d42;
	unsigned long bfSize; //文件大小
	unsigned short bfReserved1; //保留字，不考虑
	unsigned short bfReserved2; //保留字，同上
	unsigned long bfOffBits; //实际位图数据的偏移字节数，即前三个部分长度之和
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	unsigned long biSize; //指定此结构体的长度，为40
	unsigned long biWidth; //位图宽
	unsigned long biHeight; //位图高
	unsigned short biPlanes; //平面数，为1
	unsigned short biBitCount; //采用颜色位数，可以是1，2，4，8，16，24，新的可以是32
	unsigned long biCompression; //压缩方式，可以是0，1，2，其中0表示不压缩
	unsigned long biSizeImage; //实际位图数据占用的字节数
	unsigned long biXPelsPerMeter; //X方向分辨率
	unsigned long biYPelsPerMeter; //Y方向分辨率
	unsigned long biClrUsed; //使用的颜色数，如果为0，则表示默认值(2^颜色位数)
	unsigned long biClrImportant; //重要颜色数，如果为0，则表示所有颜色都是重要的
} BITMAPINFOHEADER;

#pragma pack(pop) /* 恢复先前的pack设置 */

static int isBMPFormat(unsigned char *aFileHead);
static int GetPixelDatasFrmBMP(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasForBMP(PT_PixelDatas ptPixelDatas);

T_PicFileParser g_tBMPParser = {
	.name           = "bmp",
	.isSupport      = isBMPFormat,
	.GetPixelDatas  = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,	
};

/* 行像素个数，源bpp，目的bpp，源地址，目的地址 
 * 支持源bpp:24
 * 支持目的bpp:16,24,32
 */
static int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, unsigned char *pucSrcDatas, unsigned char *pucDstDatas)
{
	unsigned int dwRed;
	unsigned int dwGreen;
	unsigned int dwBlue;
	unsigned int dwColor;

	unsigned short *pwDstDatas16bpp = (unsigned short *)pucDstDatas;
	unsigned int   *pwDstDatas32bpp = (unsigned int *)pucDstDatas;
	
	int i;
	int pos = 0;
	
	if(iSrcBpp != 24)
	{
		return -1;
	}

	if(iDstBpp == 24)
	{
		memcpy(pucDstDatas, pucSrcDatas, iWidth * iDstBpp / 3);
	}
	else
	{
		for (i = 0; i < iWidth; i++)
		{
			/* bmp的24位RGB数据，按照BGR888的顺序存储 */
			dwBlue = pucSrcDatas[pos++];
			dwGreen = pucSrcDatas[pos++];
			dwRed = pucSrcDatas[pos++];

			/* 目的16位，按照RGB565的顺序存储 */
			if(iDstBpp == 16)
			{
				dwRed >>= 3;
				dwGreen >>= 2;
				dwBlue >>= 3;

				dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue << 0);
				*pwDstDatas16bpp = dwColor;
				pwDstDatas16bpp++;
			}
			/* 目的32位，按照RGB888的顺序存储 */
			else if(iDstBpp == 32)
			{
				dwColor = (dwRed << 16) | (dwGreen << 8) | (dwBlue << 0);
				*pwDstDatas32bpp = dwColor;
				pwDstDatas32bpp++;
			}
		}

		
	}
}
/*
 * ptPixelDatas->iBpp 是输入的参数, 它决定从BMP得到的数据要转换为该格式
 */
static int GetPixelDatasFrmBMP(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas)
{
	int iWidth;
	int iHeight;
	int iBMPBpp;

	/* 行字节数向4对齐后的长度 */
	int iLineWidthAlign;
	/* 行字节数 */
	int iLineWidthReal;

	int y;
	
	unsigned char *pucSrc;
	unsigned char *pucDest;
	
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));

	/* 获得bmp图像的宽度、高度、bpp */
	iWidth = ptBITMAPINFOHEADER->biWidth;
	iHeight = ptBITMAPINFOHEADER->biHeight;
	iBMPBpp = ptBITMAPINFOHEADER->biBitCount;

	if (iBMPBpp != 24)
	{
		printf("iBMPBpp = %d\n", iBMPBpp);
		printf("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return -1;
	}

	ptPixelDatas->iWidth = iWidth;
	ptPixelDatas->iHeight = iHeight;
	//ptPixelDatas->iBpp    = iBpp;
	/* 分配点阵数据缓冲区 */
	ptPixelDatas->aucPixelDatas = malloc(iWidth * iHeight * ptPixelDatas->iBpp / 8);
	/* 缓冲区的行长度，即对应目的bpp的行长度 */
	ptPixelDatas->iLineBytes    = iWidth * ptPixelDatas->iBpp / 8;

	if (NULL == ptPixelDatas->aucPixelDatas)
	{
		return -1;
	}

	/* bmp行实际字节数 */
	iLineWidthReal = iWidth * iBMPBpp / 8;
	/* bmp行字节数向4字节对齐 */
	iLineWidthAlign = (iLineWidthReal + (4 -1 )) & ~(4 - 1);

	/* bmp位图数据起始地址 */
	pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;
	/* 第0行数据起始地址 */
	pucSrc += (iHeight - 1) * iLineWidthAlign;

	pucDest = ptPixelDatas->aucPixelDatas;

	for(y = 0; y < iHeight; y++)
	{
		/* 复制真正的行长度到目的地址 */
		//memcpy(pucDest, pucSrc, iLineWidthReal);
		/* 行像素个数，源bpp，目的bpp，源地址，目的地址 */
		CovertOneLine(iWidth, iBMPBpp, ptPixelDatas->iBpp, pucSrc, pucDest);
		pucSrc  -= iLineWidthAlign;
		pucDest += ptPixelDatas->iLineBytes;
	}

	return 0;
}
static int isBMPFormat(unsigned char *aFileHead)
{
	if (aFileHead[0] != 0x42 || aFileHead[1] != 0x4d)
		return 0;
	else
		return 1;
}

static int FreePixelDatasForBMP(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->aucPixelDatas);
	ptPixelDatas->aucPixelDatas = NULL;
	return 0;
}

