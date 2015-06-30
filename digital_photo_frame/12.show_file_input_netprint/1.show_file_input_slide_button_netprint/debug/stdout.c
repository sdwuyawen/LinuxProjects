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
	/* ֱ�Ӱ������Ϣ��printf��ӡ���� */
	printf("%s", strData);
	return strlen(strData);	
}

int StdoutInit(void)
{
	return RegisterDebugOpr(&g_tStdoutDbgOpr);
}