#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <debug_manager.h>
#include <pic_operation.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "render.h"




/* digitpic <bmp_file> */
int main(int argc, char **argv)
{
	int iFdBmp;
	unsigned char *pucBMPmem;
	struct stat tBMPstat;
	int iRet;
	int iError;
	
	PT_DispOpr ptDispOpr;
	extern T_PicFileParser g_tBMPParser;

	T_PixelDatas tPixelDatas;
	T_PixelDatas tPixelDatasSmall;

	T_PixelDatas tPixelDatasFB;

	if (argc != 2)
	{
		printf("%s <bmp_file>\n", argv[0]);
		return -1;
	}
	
	/* 一系列的初始化 */
	
	/* 在DebugInit()之前，调用DebugPrint()不会产生任何输出，因为g_ptDebugOprHead链表是空的 */
	iError = DebugInit();
	if (iError)
	{
		printf("DebugInit error!\n");
		return -1;
	}

	InitDebugChannels();

	iError = DisplayInit();
	if (iError)
	{
		printf("DisplayInit error!\n");
		return -1;
	}

	ptDispOpr = GetDispOpr("fb");
	ptDispOpr->DeviceInit();
	ptDispOpr->CleanScreen(0);

	/* 打开BMP文件 */
	iFdBmp = open(argv[1], O_RDWR);
	if(iFdBmp == -1)
	{
		printf("can't open %s\n", argv[1]);
	}

	if(fstat(iFdBmp, &tBMPstat))
	{
		printf("can't get fstat\n");
		return -1;
	}
	pucBMPmem = (unsigned char *)mmap(NULL , tBMPstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
	if (pucBMPmem == (unsigned char *)-1)
	{
		printf("can't mmap for bmp file\n");
		return -1;
	}

	/* 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 */
	iRet = g_tBMPParser.isSupport(pucBMPmem);
	if (iRet == 0)
	{
		printf("%s is not bmp file\n", argv[1]);
		return -1;		
	}	

	/* 设置目的BPP */
	tPixelDatas.iBpp = ptDispOpr->iBpp;
	/* 获取点阵数据和参数，并分配相应缓冲区给tPixelDatas */
	iRet = g_tBMPParser.GetPixelDatas(pucBMPmem, &tPixelDatas);
	if (iRet)
	{
		printf("GetPixelDatas error!\n");
		return -1;		
	}

	/* 显示原始图片 */
	tPixelDatasFB.iWidth = ptDispOpr->iXres;
	tPixelDatasFB.iHeight = ptDispOpr->iYres;
	tPixelDatasFB.iBpp = ptDispOpr->iBpp;
	tPixelDatasFB.iLineBytes = ptDispOpr->iXres * ptDispOpr->iBpp / 8;
	tPixelDatasFB.aucPixelDatas = ptDispOpr->pucDispMem;

	PicMerge(0, 0, &tPixelDatas, &tPixelDatasFB);

#if 1
	/* 缩放图片 */
	tPixelDatasSmall.iWidth  = tPixelDatas.iWidth / 2;
	tPixelDatasSmall.iHeight = tPixelDatas.iHeight / 2;
	tPixelDatasSmall.iBpp    = tPixelDatas.iBpp;
	tPixelDatasSmall.iLineBytes = tPixelDatasSmall.iWidth * tPixelDatasSmall.iBpp / 8;
	tPixelDatasSmall.aucPixelDatas = malloc(tPixelDatasSmall.iLineBytes * tPixelDatasSmall.iHeight);
	PicZoom(&tPixelDatas, &tPixelDatasSmall);

	PicMerge(128, 128, &tPixelDatasSmall, &tPixelDatasFB);
#endif

#if 1
	/* 缩放图片 */
	tPixelDatasSmall.iWidth  = tPixelDatas.iWidth / 4;
	tPixelDatasSmall.iHeight = tPixelDatas.iHeight / 4;
	tPixelDatasSmall.iBpp    = tPixelDatas.iBpp;
	tPixelDatasSmall.iLineBytes = tPixelDatasSmall.iWidth * tPixelDatasSmall.iBpp / 8;
	tPixelDatasSmall.aucPixelDatas = malloc(tPixelDatasSmall.iLineBytes * tPixelDatasSmall.iHeight);
	PicZoom(&tPixelDatas, &tPixelDatasSmall);

	PicMerge(128 + 64, 128 + 64, &tPixelDatasSmall, &tPixelDatasFB);
#endif
//	Page("main")->Run();
	return 0;	
}

