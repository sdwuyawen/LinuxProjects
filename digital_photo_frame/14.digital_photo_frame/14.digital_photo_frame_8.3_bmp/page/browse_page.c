static T_PageAction g_tBrowsePageAction = {
	.name          = "browse",
	.Run           = BrowsePageRun,
	.GetInputEvent = BrowsePageGetInputEvent,
	.Prepare       = BrowsePagePrepare;
};

static void BrowsePageRun(void)
{
	/* 1. 显示页面 */

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		InputEvent = BrowsePageGetInputEvent();
		switch (InputEvent)	
		{
			case "返回":
			{
				return 0;
			}
			case "缩小":
			{
				/* 显示缩小的页面 */
				break;
			}
			case "放大":
			{
				/* 显示放大的页面 */
				break;
			}		

			case "上幅":
			{
				/* 显示上一幅图片 */
				break;
			}		
			case "下幅":
			{
				/* 显示下一幅图片 */
				break;
			}		
			
			case "连播":
			{
				Page("auto")->Run();
				break;
			}		

			case "按住不放":
			{
				/* 显示挪动的图片 */
				break;
			}	
		}
	}
}


int BrowsePageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}