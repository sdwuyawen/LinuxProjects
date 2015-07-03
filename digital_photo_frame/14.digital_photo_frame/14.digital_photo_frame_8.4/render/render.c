#include <stdio.h>
#include "config.h"
#include "pic_operation.h"
#include "disp_manager.h"
#include "page_manager.h"
#include "file.h"

extern T_PicFileParser g_tBMPParser;

int GetFontPixel()
{
	return 0;
}

int GetPicPixel()
{	
	return 0;
}


int DrawPixel()
{
	return 0;
}


void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy(GetDefaultDispDev()->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineBytes);
	if (!ptVideoMem->bDevFrameBuffer)
	{
		DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
		GetDefaultDispDev()->ShowPage(ptVideoMem);
	}	
}

int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas)
{
	T_FileMap tFileMap;
	int iError;
	int iRet;

	int iXres, iYres, iBpp;

	/* 图标存在 /etc/digitpic/icons */
	snprintf(tFileMap.strFileName, 128, "%s/%s", ICON_PATH, strFileName);
	tFileMap.strFileName[127] = '\0';

	iError = MapFile(&tFileMap);
	if (iError)
	{
		printf("MapFile %s error!\n", strFileName);
		return -1;
	}

	/* 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 */
	iRet = g_tBMPParser.isSupport(tFileMap.pucFileMapMem);
	if (iRet == 0)
	{
		printf("%s is not bmp file\n", tFileMap.strFileName);
		return -1;		
	}	

	GetDispResolution(&iXres, &iYres, &iBpp);
	/* 设置目的BPP */
	ptPixelDatas->iBpp = iBpp;
	/* 获取点阵数据和参数，并分配相应缓冲区给tPixelDatas
	 * 按照目的BPP，把bmp转换后存储在分配的tPixelDatas->aucPixelDatas
	 */
	iRet = g_tBMPParser.GetPixelDatas(tFileMap.pucFileMapMem, ptPixelDatas);
	if (iRet)
	{
		printf("GetPixelDatas error!\n");
		return -1;		
	}

	return 0;
}

void FreePixelDatasForIcon(PT_PixelDatas ptPixelDatas)
{
	g_tBMPParser.FreePixelDatas(ptPixelDatas);
}

/* 直接修改硬件显存 */
static void InvertButton(PT_Layout ptLayout)
{
	int iY;
	int i;
	int iButtonWidthBytes;
	unsigned char *pucVideoMem;
	PT_DispOpr ptDispOpr = GetDefaultDispDev();

	pucVideoMem = ptDispOpr->pucDispMem;
	pucVideoMem += ptLayout->iTopLeftY * ptDispOpr->iLineWidth + ptLayout->iTopLeftX * ptDispOpr->iBpp / 8; /* 图标在Framebuffer中的地址 */
	iButtonWidthBytes = (ptLayout->iBotRightX - ptLayout->iTopLeftX) * ptDispOpr->iBpp / 8;

	for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
	{
		for (i = 0; i < iButtonWidthBytes; i++)
		{
			pucVideoMem[i] = ~pucVideoMem[i];  /* 取反 */
		}
		pucVideoMem += ptDispOpr->iLineWidth;
	}
	
}

void ReleaseButton(PT_Layout ptLayout)
{
	InvertButton(ptLayout);
}

void PressButton(PT_Layout ptLayout)
{
	InvertButton(ptLayout);
}