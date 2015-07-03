#include "page_manager.h"
#include "input_manager.h"
#include "disp_manager.h"
#include "config.h"
#include "render.h"
#include <stdlib.h>

static int MainPageRun(void);
static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent);

static T_PageAction g_tMainPageAction = {
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
//	.Prepare = MainPagePrepare,
};

static T_Layout g_atMainPageLayout[] = {
	{0, 0, 0, 0, "browse_mode.bmp"},
	{0, 0, 0, 0, "continue_mod.bmp"},
	{0, 0, 0, 0, "setting.bmp"},
	{0, 0, 0, 0, NULL},
};


static void ShowMainPage(PT_Layout atLayout)
{
	PT_VideoMem ptVideoMem;
	int iIconWidth;
	int iIconHeight;
	int iIconX;
	int iIconY;

	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;

#if 0
	T_PixelDatas tPixelDatasFB;

	PT_DispOpr ptDispOpr;
	ptDispOpr = GetDispOpr("fb");
	
	/* ��ʾԭʼͼƬ */
	tPixelDatasFB.iWidth = ptDispOpr->iXres;
	tPixelDatasFB.iHeight = ptDispOpr->iYres;
	tPixelDatasFB.iBpp = ptDispOpr->iBpp;
	tPixelDatasFB.iLineBytes = ptDispOpr->iXres * ptDispOpr->iBpp / 8;
	tPixelDatasFB.aucPixelDatas = ptDispOpr->pucDispMem;
#endif
	int iXres, iYres, iBpp;

	int iError;

	/* 1. ����Դ� */
	/* ����2����Ϊ1����ʾ���߳�Ҫ����ʹ�ø��Դ� */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		printf("can't get video mem for main page!\n");
		return;
	}

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);

	/* 2. �軭���� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
		
		GetDispResolution(&iXres, &iYres, &iBpp);

		iIconHeight = iYres * 2 / 10;
		iIconWidth  = iIconHeight * 2;

		iIconX = (iXres - iIconWidth)/2;
		iIconY = iYres / 10;

		/* ����Ҫ��ʾ��ͼ���С */
		tIconPixelDatas.iWidth  = iIconWidth;
		tIconPixelDatas.iHeight = iIconHeight;
		tIconPixelDatas.iBpp    = iBpp;
		tIconPixelDatas.iLineBytes = iIconWidth * iBpp / 8;
		tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;
		/* �������ź��ͼ���ŵĻ����� */
		tIconPixelDatas.aucPixelDatas = malloc(tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight);

		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
//			free(tIconPixelDatas.aucPixelDatas);
			return;
		}
		
		while (atLayout->strIconName)
		{
			atLayout->iTopLeftX  = iIconX;
			atLayout->iTopLeftY  = iIconY;
			atLayout->iBotRightX = iIconX + iIconWidth - 1;
			atLayout->iBotRightY = iIconY + iIconHeight - 1;

			iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
			if (iError)
			{
				printf("GetPixelDatasForIcon error!\n");
				return;
			}

			/* �������õ�ͼ���С������ͼƬ */
			if(PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas))
			{
				printf("PicZoom Error\n");
			}

			/* �����ź��ͼƬ�ϲ���APP�Դ��� */
			if(PicMerge(iIconX, iIconY, &tIconPixelDatas, &ptVideoMem->tPixelDatas))
//			if(PicMerge(iIconX, iIconY, &tIconPixelDatas, &tPixelDatasFB))
			{
				printf("PicMerge Error\n");
			}
//			PicMerge(0, 0, &ptVideoMem->tPixelDatas, &tPixelDatasFB);

			/* �ͷ���GetPixelDatasForIcon()��������ڴ� */
			FreePixelDatasForIcon(&tOriginIconPixelDatas);
			
			atLayout++;
			iIconY +=  iYres * 3 / 10;

			DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
		}

		/* �ͷ����ź��ͼ�껺���� */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);
	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
	
}
static int MainPageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	
	/* 1. ��ʾҳ�� */
	ShowMainPage(g_atMainPageLayout);

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = MainPageGetInputEvent(g_atMainPageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* ������ɿ� */
			if (bPressed)
			{
				DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
				/* �����а�ť������ */
				ReleaseButton(&g_atMainPageLayout[iIndexPressed]);
				bPressed = 0;
				iIndexPressed = -1;
			}
		}
		else
		{
			/* ����״̬ */
			if (iIndex != -1)
			{
				if (!bPressed)
				{
					DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
					/* δ�����°�ť */
					bPressed = 1;
					iIndexPressed = iIndex;
					PressButton(&g_atMainPageLayout[iIndexPressed]);
				}
			}
		}	
	}

	return 0;
}

static int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߳�����
	 * ���������̻߳�����ݺ󣬻��������
	 */
	iRet = GetInputEvent(&tInputEvent);
	if(iRet)
	{
		return -1;
	}
	if (tInputEvent.iType != INPUT_TYPE_TOUCHSCREEN)
	{
		return -1;
	}

	/* ����tInputEvent */
	*ptInputEvent = tInputEvent;
	
	/* �������� */
	/* ȷ������λ����һ����ť�� */
	while (atLayout[i].strIconName)
	{
		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
		{
			/* �ҵ��˱����еİ�ť */
			return i;
		}
		else
		{
			i++;
		}			
	}

	/* ����û�����ڰ�ť�� */
	return -1;
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

