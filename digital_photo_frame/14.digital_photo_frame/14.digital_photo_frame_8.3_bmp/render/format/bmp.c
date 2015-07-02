#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#pragma pack(push) /* ����ǰpack����ѹջ���� */
#pragma pack(1)    /* �����ڽṹ�嶨��֮ǰʹ�� */

typedef struct tagBITMAPFILEHEADER 
{
	unsigned short bfType;//�̶�Ϊ0x4d42;
	unsigned long bfSize; //�ļ���С
	unsigned short bfReserved1; //�����֣�������
	unsigned short bfReserved2; //�����֣�ͬ��
	unsigned long bfOffBits; //ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	unsigned long biSize; //ָ���˽ṹ��ĳ��ȣ�Ϊ40
	unsigned long biWidth; //λͼ��
	unsigned long biHeight; //λͼ��
	unsigned short biPlanes; //ƽ������Ϊ1
	unsigned short biBitCount; //������ɫλ����������1��2��4��8��16��24���µĿ�����32
	unsigned long biCompression; //ѹ����ʽ��������0��1��2������0��ʾ��ѹ��
	unsigned long biSizeImage; //ʵ��λͼ����ռ�õ��ֽ���
	unsigned long biXPelsPerMeter; //X����ֱ���
	unsigned long biYPelsPerMeter; //Y����ֱ���
	unsigned long biClrUsed; //ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)
	unsigned long biClrImportant; //��Ҫ��ɫ�������Ϊ0�����ʾ������ɫ������Ҫ��
} BITMAPINFOHEADER;

#pragma pack(pop) /* �ָ���ǰ��pack���� */

static int isBMPFormat(unsigned char *aFileHead);
static int GetPixelDatasFrmBMP(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasForBMP(PT_PixelDatas ptPixelDatas);

T_PicFileParser g_tBMPParser = {
	.name           = "bmp",
	.isSupport      = isBMPFormat,
	.GetPixelDatas  = GetPixelDatasFrmBMP,
	.FreePixelDatas = FreePixelDatasForBMP,	
};

/* �����ظ�����Դbpp��Ŀ��bpp��Դ��ַ��Ŀ�ĵ�ַ 
 * ֧��Դbpp:24
 * ֧��Ŀ��bpp:16,24,32
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
			/* bmp��24λRGB���ݣ�����BGR888��˳��洢 */
			dwBlue = pucSrcDatas[pos++];
			dwGreen = pucSrcDatas[pos++];
			dwRed = pucSrcDatas[pos++];

			/* Ŀ��16λ������RGB565��˳��洢 */
			if(iDstBpp == 16)
			{
				dwRed >>= 3;
				dwGreen >>= 2;
				dwBlue >>= 3;

				dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue << 0);
				*pwDstDatas16bpp = dwColor;
				pwDstDatas16bpp++;
			}
			/* Ŀ��32λ������RGB888��˳��洢 */
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
 * ptPixelDatas->iBpp ������Ĳ���, ��������BMP�õ�������Ҫת��Ϊ�ø�ʽ
 */
static int GetPixelDatasFrmBMP(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas)
{
	int iWidth;
	int iHeight;
	int iBMPBpp;

	/* ���ֽ�����4�����ĳ��� */
	int iLineWidthAlign;
	/* ���ֽ��� */
	int iLineWidthReal;

	int y;
	
	unsigned char *pucSrc;
	unsigned char *pucDest;
	
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));

	/* ���bmpͼ��Ŀ�ȡ��߶ȡ�bpp */
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
	/* ����������ݻ����� */
	ptPixelDatas->aucPixelDatas = malloc(iWidth * iHeight * ptPixelDatas->iBpp / 8);
	/* ���������г��ȣ�����ӦĿ��bpp���г��� */
	ptPixelDatas->iLineBytes    = iWidth * ptPixelDatas->iBpp / 8;

	if (NULL == ptPixelDatas->aucPixelDatas)
	{
		return -1;
	}

	/* bmp��ʵ���ֽ��� */
	iLineWidthReal = iWidth * iBMPBpp / 8;
	/* bmp���ֽ�����4�ֽڶ��� */
	iLineWidthAlign = (iLineWidthReal + (4 -1 )) & ~(4 - 1);

	/* bmpλͼ������ʼ��ַ */
	pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;
	/* ��0��������ʼ��ַ */
	pucSrc += (iHeight - 1) * iLineWidthAlign;

	pucDest = ptPixelDatas->aucPixelDatas;

	for(y = 0; y < iHeight; y++)
	{
		/* �����������г��ȵ�Ŀ�ĵ�ַ */
		//memcpy(pucDest, pucSrc, iLineWidthReal);
		/* �����ظ�����Դbpp��Ŀ��bpp��Դ��ַ��Ŀ�ĵ�ַ */
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

