#include <config.h>
#include <input_manager.h>
#include <stdlib.h>

#include <tslib.h>
#include "draw.h"

/* 参考tslib里的ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

static T_InputOpr g_tTouchScreenOpr;

/* 注意: 由于要用到LCD的分辨率, 此函数要在SelectAndInitDisplay之后调用 */
static int TouchScreenDevInit(void)
{
	char *pcTSName = NULL;

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
#if 0
		/* 以非阻塞方式打开 */
		g_tTSDev = ts_open(pcTSName, 1);
#endif
		/* 以阻塞方式打开 */
		g_tTSDev = ts_open(pcTSName, 0);
	}
	else
	{
#if 0
		/* 以非阻塞方式打开 */
		g_tTSDev = ts_open("/dev/event0", 1);
#endif

		/* 以阻塞方式打开 */
		g_tTSDev = ts_open(pcTSName, 0);
	}

	if (!g_tTSDev) {
		printf("ts_open error!\n");
		return -1;
	}

	if (ts_config(g_tTSDev)) {
		printf("ts_config error!\n");
		return -1;
	}

	if (GetDispResolution(&giXres, &giYres))
	{
		return -1;
	}

	return 0;
}

static int TouchScreenDevExit(void)
{
	return 0;
}


/* 按下-滑动-响应-松开，即每次按下只响应一次滑动事件  */
static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	struct ts_sample tSamp;
	struct ts_sample tSampPressed;
	struct ts_sample tSampReleased;
	int iRet;
	int bStart = 0;
	static int bHandled = 0;
	int iDelta;
	
	while (1)
	{
		/* 阻塞方式打开时，如果无数据则休眠 */
		iRet = ts_read(g_tTSDev, &tSamp, 1);
		if (iRet == 1)
		{
			if(tSamp.pressure <= 0)
			{
				tSampReleased = tSamp;
				bHandled = 0;
				return -1;
			}
					
			if ((tSamp.pressure > 0) && (bStart == 0) && bHandled == 0)
			{
				/* 刚按下 */
				/* 记录刚开始压下的点 */
				tSampPressed = tSamp;
				bStart = 1;
			}
			
			if (bStart != 0 && bHandled == 0)
			{
				/* 仍在按下状态 */
				if (tSamp.pressure > 0)
				{
					/* 处理数据 */
					iDelta = tSamp.y - tSampPressed.y;

					if(iDelta > giYres/5
						|| iDelta < 0 - giYres/5)
					{	
						ptInputEvent->tTime = tSamp.tv;
						ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;
						/* 滑动距离超过阈值 */

						if (iDelta > giYres/5)
						{
							/* 翻到上一页 */
							ptInputEvent->iVal = INPUT_VALUE_UP;
						}
						else if (iDelta < 0 - giYres/5)
						{
							/* 翻到下一页 */
							ptInputEvent->iVal = INPUT_VALUE_DOWN;				
						}

						bHandled = 1;
						return 0;
					}
				}
			}
		}
		else
		{
			return -1;
		}
	}
	

	return 0;
}


static T_InputOpr g_tTouchScreenOpr = {
	.name          = "touchscreen",
	.DeviceInit    = TouchScreenDevInit,
	.DeviceExit    = TouchScreenDevExit,
	.GetInputEvent = TouchScreenGetInputEvent,
};

int TouchScreenInit(void)
{
	return RegisterInputOpr(&g_tTouchScreenOpr);
}

