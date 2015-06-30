
#include <config.h>
#include <input_manager.h>
#include <string.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent g_tInputEvent;
/* �����ź��������ڶ��̷߳���ͬһȫ�ֱ����ı��� */
static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
/* �������� */
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptInputOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}

	return 0;
}


void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/* �߳���������ÿ���̵߳�����������������������Ǵ���Ĳ�����ͬ */
static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* ���庯��ָ�� */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while(1)
	{
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* �������߳� */
			/* �����ٽ���Դǰ���Ȼ�û����� */
			pthread_mutex_lock(&g_tMutex);

			/* ��tInputEvent��ֵ����һ��ȫ�ֱ���,�������̶߳�ȡ */
			g_tInputEvent = tInputEvent;

			/*  ���ѵȴ�g_tConVar�����߳� */
			pthread_cond_signal(&g_tConVar);

			/* �ͷŻ����� */
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}
int AllInputDevicesInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if(0 == ptTmp->DeviceInit())
		{
			/* ����ÿ�������豸���������߳�
			 * ��4�������Ǵ��ݸ��̵߳��ú����Ĳ���
			 */
			pthread_create(&ptTmp->tThreadID, NULL, InputEventThreadFunction, ptTmp->GetInputEvent);	
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}

	return iError;
}

/* ������ѭ������ */
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* ���̵߳����߳� */
	/* ���� */
	pthread_mutex_lock(&g_tMutex);

	/* �ȴ��������������ߡ�
	 * �̱߳����������������ĵȴ�������ʱ�����ͷ�g_tMutex����ź��� 
	 */
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* �����Ѻ�,�������� */
	*ptInputEvent = g_tInputEvent;
	
	pthread_mutex_unlock(&g_tMutex);

	return 0;

#if 0
	/* select��ʽ */
	/* ��select�������stdin,touchscreen,
	   ������ʱ�ٵ������ǵ�GetInputEvent���þ����¼�
	 */
	PT_InputOpr ptTmp = g_ptInputOprHead;
	fd_set tRFds;
	int iRet;

	/* ��ȡҪ���ӵ��ļ���� */
	tRFds = g_tRFds;

	iRet = select(g_iMaxFd, &tRFds, NULL, NULL, NULL);
	/* iRet > 0�������ļ��ɶ� */
	if (iRet > 0)
	{
		while (ptTmp)
		{
			if (FD_ISSET(ptTmp->iFd, &tRFds))
			{
				if(0 == ptTmp->GetInputEvent(ptInputEvent))
				{
					return 0;
				}
			}
			ptTmp = ptTmp->ptNext;
		}
	}
	return -1;
#endif

#if 0
	/* ��ѯ��ʽ */
	/* �������е�InputOpr��GetInputEvent������һ��,һ�������ݼ����� */
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		if (0 == ptTmp->GetInputEvent(ptInputEvent))
		{
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
#endif
}

int InputInit(void)
{
	int iError;
	
	iError = StdinInit();
	iError += TouchScreenInit();
	iError += ButtonInit();

	return iError;
}


