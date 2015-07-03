

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