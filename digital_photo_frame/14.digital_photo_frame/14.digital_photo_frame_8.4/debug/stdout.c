#include "debug_manager.h"
#include <config.h>
#include <stdio.h>
#include <string.h>

static int StdoutDebugPrint(char *strData);

static T_DebugOpr g_tStdoutDbgOpr = {
	.name = "stdout",
	.isCanUse = 1,
//	.DebugInit = StdoutDbgInit,
//	.DebugExit = StdoutDbgExit,
	.DebugPrint = StdoutDebugPrint,	
};


static int StdoutDebugPrint(char *strData)
{
	/* 直接把输出信息用printf打印出来 */
	printf("%s", strData);
	return strlen(strData);	
}

int StdoutInit(void)
{
	return RegisterDebugOpr(&g_tStdoutDbgOpr);
}