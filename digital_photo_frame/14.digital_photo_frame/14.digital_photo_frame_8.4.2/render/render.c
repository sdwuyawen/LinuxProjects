extern T_PicFileParser g_tBMPParser;

int GetFontPixel()
{
}

int GetPicPixel()
{
}


int DrawPixel()
{
}


void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy(GetDefaultDispDev()->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineBytes);
	if (!ptVideoMem->bDevFrameBuffer)
	{
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
	g_tBMPParser->FreePixelDatas(ptPixelDatas);
}


