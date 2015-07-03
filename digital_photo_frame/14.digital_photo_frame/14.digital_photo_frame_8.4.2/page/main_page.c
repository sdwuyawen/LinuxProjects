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

	/* 1. 获得显存 */
	/* 参数2设置为1，表示本线程要立刻使用该显存 */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		printf("can't get video mem for main page!\n");
		return;
	}

	/* 2. 描画数据 */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		GetDispResolution(&iXres, &iYres, &iBpp);

		iIconHeight = iYres * 2 / 10;
		iIconWidth  = iIconHeight * 2;

		iIconX = (iXres - iIconWidth)/2;
		iIconY = iYres / 10;

		/* 设置要显示的图标大小 */
		tIconPixelDatas.iWidth  = iIconWidth;
		tIconPixelDatas.iHeight = iIconHeight;
		tIconPixelDatas.iBpp    = iBpp;
		tIconPixelDatas.iLineBytes = iIconWidth * iBpp / 8;
		tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;
		/* 分配缩放后的图标存放的缓冲区 */
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

			/* 根据设置的图标大小，缩放图片 */
			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

			/* 把缩放后的图片合并到APP显存里 */
			PicMerge(iIconX, iIconY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

			/* 释放在GetPixelDatasForIcon()中申请的内存 */
			FreePixelDatasForIcon(&tOriginIconPixelDatas);
			
			atLayout++:
			iIconY +=  iYres * 3 / 10;
		}

		/* 释放缩放后的图标缓冲区 */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
	
}
static void MainPageRun(void)
{
	/* 1. 显示页面 */
	ShowMainPage(g_atMainPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		InputEvent = MainPageGetInputEvent();
		switch (InputEvent)	
		{
			case "浏览模式":
			{
				/* 保存当前页面 */
				StorePage();
				
				Page("explore")->Run();
				
				/* 恢复之前的页面 */
				RestorePage();
				break;
			}
			case "连播模式":
			{
				/* 保存当前页面 */
				StorePage();

				Page("auto")->Run();

				/* 恢复之前的页面 */
				RestorePage();
				
				break;
			}
			case "设置":
			{
				/* 保存当前页面 */
				StorePage();
				
				Page("setting")->Run();
				
				/* 恢复之前的页面 */
				RestorePage();
				break;
			}		
		}
	}
}

static int MainPageGetInputEvent(...)
{
	/* 获得原始的触摸屏数据 
	 * 它是调用input_manager.c的函数，此函数会让当前线程休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
	 */
	GetInputEvent();
	
	/* 处理数据 */
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

