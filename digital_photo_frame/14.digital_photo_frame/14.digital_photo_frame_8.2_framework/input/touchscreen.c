#include <config.h>
#include <input_manager.h>
#include <stdlib.h>

#include <tslib.h>
#include "draw.h"

/* �ο�tslib���ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

static T_InputOpr g_tTouchScreenOpr;

/* ע��: ����Ҫ�õ�LCD�ķֱ���, �˺���Ҫ��SelectAndInitDisplay֮����� */
static int TouchScreenDevInit(void)
{
	char *pcTSName = NULL;

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
#if 0
		/* �Է�������ʽ�� */
		g_tTSDev = ts_open(pcTSName, 1);
#endif
		/* ��������ʽ�� */
		g_tTSDev = ts_open(pcTSName, 0);
	}
	else
	{
#if 0
		/* �Է�������ʽ�� */
		g_tTSDev = ts_open("/dev/event0", 1);
#endif

		/* ��������ʽ�� */
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


/* ����-����-��Ӧ-�ɿ�����ÿ�ΰ���ֻ��Ӧһ�λ����¼�  */
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
		/* ������ʽ��ʱ����������������� */
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
				/* �հ��� */
				/* ��¼�տ�ʼѹ�µĵ� */
				tSampPressed = tSamp;
				bStart = 1;
			}
			
			if (bStart != 0 && bHandled == 0)
			{
				/* ���ڰ���״̬ */
				if (tSamp.pressure > 0)
				{
					/* �������� */
					iDelta = tSamp.y - tSampPressed.y;

					if(iDelta > giYres/5
						|| iDelta < 0 - giYres/5)
					{	
						ptInputEvent->tTime = tSamp.tv;
						ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;
						/* �������볬����ֵ */

						if (iDelta > giYres/5)
						{
							/* ������һҳ */
							ptInputEvent->iVal = INPUT_VALUE_UP;
						}
						else if (iDelta < 0 - giYres/5)
						{
							/* ������һҳ */
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

