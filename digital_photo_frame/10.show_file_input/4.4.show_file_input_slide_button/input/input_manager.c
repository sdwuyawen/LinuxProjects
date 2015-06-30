
#include <config.h>
#include <input_manager.h>
#include <string.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent g_tInputEvent;
/* 互斥信号量，用于多线程访问同一全局变量的保护 */
static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
/* 条件变量 */
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

/* 线程主函数，每个线程的主函数都是这个函数，但是传入的参数不同 */
static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* 定义函数指针 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while(1)
	{
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* 唤醒主线程 */
			/* 访问临界资源前，先获得互斥量 */
			pthread_mutex_lock(&g_tMutex);

			/* 把tInputEvent的值赋给一个全局变量,以让主线程读取 */
			g_tInputEvent = tInputEvent;

			/*  唤醒等待g_tConVar的主线程 */
			pthread_cond_signal(&g_tConVar);

			/* 释放互斥量 */
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
			/* 对于每个输入设备，创建子线程
			 * 第4个参数是传递给线程调用函数的参数
			 */
			pthread_create(&ptTmp->tThreadID, NULL, InputEventThreadFunction, ptTmp->GetInputEvent);	
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}

	return iError;
}

/* 主函数循环调用 */
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 多线程的主线程 */
	/* 休眠 */
	pthread_mutex_lock(&g_tMutex);

	/* 等待条件变量，休眠。
	 * 线程被放置在条件变量的等待队列中时，会释放g_tMutex这个信号量 
	 */
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* 被唤醒后,返回数据 */
	*ptInputEvent = g_tInputEvent;
	
	pthread_mutex_unlock(&g_tMutex);

	return 0;

#if 0
	/* select方式 */
	/* 用select函数监测stdin,touchscreen,
	   有数据时再调用它们的GetInputEvent或获得具体事件
	 */
	PT_InputOpr ptTmp = g_ptInputOprHead;
	fd_set tRFds;
	int iRet;

	/* 获取要监视的文件句柄 */
	tRFds = g_tRFds;

	iRet = select(g_iMaxFd, &tRFds, NULL, NULL, NULL);
	/* iRet > 0表明有文件可读 */
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
	/* 轮询方式 */
	/* 把链表中的InputOpr的GetInputEvent都调用一次,一旦有数据即返回 */
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


