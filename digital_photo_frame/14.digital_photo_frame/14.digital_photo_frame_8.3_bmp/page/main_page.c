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

static void MainPageRun(void)
{
	/* 1. 显示页面 */

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

