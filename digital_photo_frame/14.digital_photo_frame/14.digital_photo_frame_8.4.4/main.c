#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <debug_manager.h>
#include <pic_operation.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "render.h"




/* digitpic <bmp_file> */
int main(int argc, char **argv)
{
	int iError;

	if (argc != 2)
	{
		printf("%s <bmp_file>\n", argv[0]);
		return -1;
	}
	
	/* 一系列的初始化 */
	
	/* 在DebugInit()之前，调用DebugPrint()不会产生任何输出，因为g_ptDebugOprHead链表是空的 */
	iError = DebugInit();
	if (iError)
	{
		printf("DebugInit error!\n");
		return -1;
	}

	InitDebugChannels();

	iError = DisplayInit();
	if (iError)
	{
		printf("DisplayInit error!\n");
		return -1;
	}
	SelectAndInitDefaultDispDev("fb");

	AllocVideoMem(5);

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);

	InputInit();
	AllInputDevicesInit();

	PagesInit();

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	
	Page("main")->Run();

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	
	return 0;	
}

