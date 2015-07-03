static T_PageAction g_tExplorePageAction = {
	.name          = "explore",
	.Run           = ExplorePageRun,
	.GetInputEvent = ExplorePageGetInputEvent,
	.Prepare       = ExplorePagePrepare;
};

static void ExplorePageRun(void)
{
	/* 1. 显示页面 */

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		InputEvent = ExplorePageGetInputEvent();
		switch (InputEvent)	
		{
			case "向上":
			{
				/* 判断是否已经是顶层 */
				if (isTopLevel)
					return 0;
				else
				{
					/* 显示上一个目录的页面 */
				}
				break;
			}
			case "选择":
			{
				if (isSelectDir)
				{
					/* 显示下一级目录 */
				}
				else
				{
					/* 保存当前页面 */
					StorePage();
					Page("browse")->Run();
					/* 恢复之前的页面 */
					RestorePage();
				}
				break;
			}
			case "下页":
			{
				/* 显示下一页 */
				break;
			}		

			case "上页":
			{
				/* 显示上一页 */
				break;
			}		
		}
	}
}


int ExplorePageInit(void)
{
	return RegisterPageAction(&g_tExplorePageAction);
}