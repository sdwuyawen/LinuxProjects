#include <config.h>
#include <input_manager.h>
#include <stdlib.h>
#include <disp_manager.h>

#include <tslib.h>

/* 参考tslib里的ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;
static int giBpp;

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

	if (GetDispResolution(&giXres, &giYres, &giBpp))
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
	int iRet;
	
	while (1)
	{
		/* 阻塞方式打开时，如果无数据则休眠 */
		iRet = ts_read(g_tTSDev, &tSamp, 1);
		if (iRet == 1)
		{
			ptInputEvent->tTime     = tSamp.tv;
			ptInputEvent->iType     = INPUT_TYPE_TOUCHSCREEN;
			ptInputEvent->iX        = tSamp.x;
			ptInputEvent->iY        = tSamp.y;
			ptInputEvent->iPressure = tSamp.pressure;

			return 0;
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

