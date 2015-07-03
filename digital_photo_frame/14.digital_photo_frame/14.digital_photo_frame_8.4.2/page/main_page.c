#include "page_manager.h"
#include "input_manager.h"
#include "config.h"

static void MainPageRun(void);
static int MainPageGetInputEvent(...);

static T_PageAction g_tMainPageAction = {
	.name = "main",
	.Run = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	.Prepare = MainPagePrepare,
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

	/* 2. �軭���� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
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
			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

			/* �����ź��ͼƬ�ϲ���APP�Դ��� */
			PicMerge(iIconX, iIconY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

			/* �ͷ���GetPixelDatasForIcon()��������ڴ� */
			FreePixelDatasForIcon(&tOriginIconPixelDatas);
			
			atLayout++:
			iIconY +=  iYres * 3 / 10;
		}

		/* �ͷ����ź��ͼ�껺���� */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
	
}
static void MainPageRun(void)
{
	/* 1. ��ʾҳ�� */
	ShowMainPage(g_atMainPageLayout);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		InputEvent = MainPageGetInputEvent();
		switch (InputEvent)	
		{
			case "���ģʽ":
			{
				/* ���浱ǰҳ�� */
				StorePage();
				
				Page("explore")->Run();
				
				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
				break;
			}
			case "����ģʽ":
			{
				/* ���浱ǰҳ�� */
				StorePage();

				Page("auto")->Run();

				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
				
				break;
			}
			case "����":
			{
				/* ���浱ǰҳ�� */
				StorePage();
				
				Page("setting")->Run();
				
				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
				break;
			}		
		}
	}
}

static int MainPageGetInputEvent(...)
{
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߳�����
	 * ���������̻߳�����ݺ󣬻��������
	 */
	GetInputEvent();
	
	/* �������� */
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}
