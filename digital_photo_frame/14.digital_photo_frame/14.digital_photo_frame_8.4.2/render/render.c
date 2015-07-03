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

	/* ͼ����� /etc/digitpic/icons */
	snprintf(tFileMap.strFileName, 128, "%s/%s", ICON_PATH, strFileName);
	tFileMap.strFileName[127] = '\0';

	iError = MapFile(&tFileMap);
	if (iError)
	{
		printf("MapFile %s error!\n", strFileName);
		return -1;
	}

	/* ��ȡBMP�ļ���RGB����, ����, ��LCD����ʾ���� */
	iRet = g_tBMPParser.isSupport(tFileMap.pucFileMapMem);
	if (iRet == 0)
	{
		printf("%s is not bmp file\n", tFileMap.strFileName);
		return -1;		
	}	

	GetDispResolution(&iXres, &iYres, &iBpp);
	/* ����Ŀ��BPP */
	ptPixelDatas->iBpp = iBpp;
	/* ��ȡ�������ݺͲ�������������Ӧ��������tPixelDatas
	 * ����Ŀ��BPP����bmpת����洢�ڷ����tPixelDatas->aucPixelDatas
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


