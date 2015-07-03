
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;

int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)
	{
		g_ptDispOprHead   = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}


void ShowDispOpr(void)
{
	int i = 0;
	PT_DispOpr ptTmp = g_ptDispOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmp = g_ptDispOprHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

void SelectAndInitDefaultDispDev(char *name)
{
	g_ptDefaultDispOpr = GetDispOpr(name);
	if (g_ptDefaultDispOpr)
	{
		g_ptDefaultDispOpr->DeviceInit();
		g_ptDefaultDispOpr->CleanScreen(0x00FF00);
	}
	else
	{
		printf("SelectAndInitDefaultDispDev %s error\n", name);
	}
}

PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}

int GetDispResolution(int *piXres, int *piYres, int *piBpp)
{
	if(g_ptDefaultDispOpr)
	{
		*piXres = g_ptDefaultDispOpr->iXres;
		*piYres = g_ptDefaultDispOpr->iYres;
		*piBpp  = g_ptDefaultDispOpr->iBpp;

		return 0;
	}
	else
	{
		return -1;
	}
}

int AllocVideoMem(int iNum)
{
	int i;

	int iXres = 0;
	int iYres = 0;;
	int iBpp = 0;

	int iVMSize;
	int iLineBytes;

	PT_VideoMem ptNew;
	PT_VideoMem ptTemp;
	
	GetDispResolution(&iXres, &iYres, &iBpp);

	iVMSize = iXres * iYres * iBpp / 8;
	iLineBytes = iXres * iBpp / 8;

	DBG_PRINTF("%s iVMSize = %d, iLineBytes = %d\n", __FUNCTION__, iVMSize, iLineBytes);

	/* 物理显存也分配一个显存控制块管理
	 * 该显存的物理内存是framebuffer的内存
	 */
	/* 先把设备本身的framebuffer放入链表 */
	ptNew = malloc(sizeof(T_VideoMem));
	if (ptNew == NULL)
	{
		return -1;
	}

	/* 物理显存 */
	ptNew->tPixelDatas.aucPixelDatas = g_ptDefaultDispOpr->pucDispMem;

	ptNew->iID = 0;
	/* 是设备的framebuffer */
	ptNew->bDevFrameBuffer = 1;
	ptNew->eVideoMemState = VMS_FREE;
	ptNew->ePicState = PS_BLANK;
	ptNew->tPixelDatas.iWidth = iXres;
	ptNew->tPixelDatas.iHeight = iYres;
	ptNew->tPixelDatas.iBpp = iBpp;
	ptNew->tPixelDatas.iLineBytes = iLineBytes;
	ptNew->tPixelDatas.iTotalBytes = iVMSize;

	/* 如果有App显存的话，则把framebuffer显存设置为已占用，以后GetVideoMem()不会把framebuffer分配出去 */
	if (iNum != 0)
	{
		ptNew->eVideoMemState = VMS_USED_FOR_CUR;
	}
	
#if 0
		/* 放入链表头 */
		ptNew->ptNext = g_ptVideoMemHead;
		g_ptVideoMemHead = ptNew;
#else
		/* 放入链表尾 */
		if(!g_ptVideoMemHead)
		{
			g_ptVideoMemHead = ptNew;
			ptNew->ptNext = NULL;
		}
		else
		{
			ptTemp = g_ptVideoMemHead;
			while(ptTemp->ptNext)
			{
				ptTemp = ptTemp->ptNext;
			}
			ptTemp->ptNext = ptNew;
			ptNew->ptNext = NULL;
		}
#endif

	for (i = 0; i < iNum; i++)
	{
		ptNew = malloc(sizeof(T_VideoMem) + iVMSize);
		if (ptNew == NULL)
		{
			return -1;
		}

		/* 显存控制块中的像素数据中的缓冲区，指向刚分配的缓冲区 */
		ptNew->tPixelDatas.aucPixelDatas = (unsigned char *)(ptNew + 1);

		ptNew->iID = 0;
		/* 不是设备的framebuffer */
		ptNew->bDevFrameBuffer = 0;
		ptNew->eVideoMemState = VMS_FREE;
		ptNew->ePicState = PS_BLANK;
		ptNew->tPixelDatas.iWidth = iXres;
		ptNew->tPixelDatas.iHeight = iYres;
		ptNew->tPixelDatas.iBpp = iBpp;
		ptNew->tPixelDatas.iLineBytes = iLineBytes;
		ptNew->tPixelDatas.iTotalBytes = iVMSize;

#if 0
		/* 放入链表头 */
		ptNew->ptNext = g_ptVideoMemHead;
		g_ptVideoMemHead = ptNew;
#else
		/* 放入链表尾 */
		if(!g_ptVideoMemHead)
		{
			g_ptVideoMemHead = ptNew;
			ptNew->ptNext = NULL;
		}
		else
		{
			ptTemp = g_ptVideoMemHead;
			while(ptTemp->ptNext)
			{
				ptTemp = ptTemp->ptNext;
			}
			ptTemp->ptNext = ptNew;
			ptNew->ptNext = NULL;
		}
#endif
	}

	return 0;
}

PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptTmp = g_ptVideoMemHead;

	/* 1. 优先: 取出空闲的、ID相同的videomem */
	while(ptTmp)
	{
		if(ptTmp->eVideoMemState == VMS_FREE && ptTmp->iID == iID)
		{
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	
	/* 2. 优先: 取出任意一个空闲videomem */
	ptTmp = g_ptVideoMemHead;
	while(ptTmp)
	{
		if(ptTmp->eVideoMemState == VMS_FREE)
		{
			ptTmp->iID = iID;
			ptTmp->ePicState = PS_BLANK;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

	return NULL;
}

void PutVideoMem(PT_VideoMem ptVideoMem)
{
	ptVideoMem->eVideoMemState = VMS_FREE;
}

int DisplayInit(void)
{
	int iError;
	
	iError = FBInit();

	return iError;
}

