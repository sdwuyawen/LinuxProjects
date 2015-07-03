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
}

static void ShowMainPage(PT_Layout atLayout)
{
	PT_VideoMem ptVideoMem;

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

