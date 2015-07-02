#include "debug_manager.h"
#include "config.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h> 
#include <stdlib.h> 

#define SERVER_PORT		5678
#define PRINT_BUF_SIZE		(16*1024)

static int g_iSocketServer;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;
static int g_iHaveConnected = 0;
static char *g_pcNetPrintBuf = NULL;

/* ���λ�������дλ�� */
static int g_iReadPos  = 0;
static int g_iWritePos = 0;

static pthread_t g_tSendTreadID;
static pthread_t g_tRecvTreadID;

static pthread_mutex_t g_tNetDbgSendMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tNetDbgSendConVar = PTHREAD_COND_INITIALIZER;

int NetDbgInit(void);
int NetDbgExit(void);
int NetDbgPrint(char *strData);

static T_DebugOpr g_tNetDbgOpr = {
	.name       = "netprint",
	.isCanUse   = 1,
	.DebugInit  = NetDbgInit,
	.DebugExit  = NetDbgExit,
	.DebugPrint = NetDbgPrint,
};

static int isFull(void)
{
	return (((g_iWritePos + 1) % PRINT_BUF_SIZE) == g_iReadPos);
}

static int isEmpty(void)
{
	return (g_iWritePos == g_iReadPos);
}

static int PutData(char cVal)
{
	if (isFull())
		return -1;
	else
	{
		g_pcNetPrintBuf[g_iWritePos] = cVal;
		g_iWritePos = (g_iWritePos + 1) % PRINT_BUF_SIZE;
		return 0;
	}	
}

static int GetData(char *pcVal)
{
	if (isEmpty())
		return -1;
	else
	{
		*pcVal = g_pcNetPrintBuf[g_iReadPos];
		g_iReadPos = (g_iReadPos + 1) % PRINT_BUF_SIZE;
		return 0;
	}	
}

static void *NetDbgSendTreadFunction(void *pVoid)
{
	int iAddrLen;
	int iSendLen;
	char strTmpBuf[512];
	int i;
	char cVal;
	
	while(1)
	{
		/* ƽʱ���� */
		pthread_mutex_lock(&g_tNetDbgSendMutex);

		/* �ȴ��������������ߡ�
		 * �̱߳����������������ĵȴ�������ʱ�����ͷ�g_tMutex����ź��� 
		 */
		pthread_cond_wait(&g_tNetDbgSendConVar, &g_tNetDbgSendMutex);	

		pthread_mutex_unlock(&g_tNetDbgSendMutex);

		/* ���ͻ��������������ݵ��ͻ��� */
		while (g_iHaveConnected && !isEmpty())
		{
			i = 0;

			/* �ѻ��λ�����������ȡ����, ���ȡ512�ֽ� */
			while ((i < 512) && (0 == GetData(&cVal)))
			{
				strTmpBuf[i] = cVal;
				i++;
			}
			
			/* ִ�е�����, ��ʾ������ */
			/* ��sendto�������ʹ�ӡ��Ϣ���ͻ��� */
			iAddrLen = sizeof(struct sockaddr);
			iSendLen = sendto(g_iSocketServer, strTmpBuf, i, 0,
			                      (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);

		}
	}

	return NULL;
}

static void *NetDbgRecvTreadFunction(void *pVoid)
{
	socklen_t iAddrLen;
	int iRecvLen;
	char ucRecvBuf[1000];
	struct sockaddr_in tSocketClientAddr;
	
	while(1)
	{
		iAddrLen = sizeof(struct sockaddr);

		DBG_PRINTF("in NetDbgRecvTreadFunction\n");
		
		/* tSocketClientAddr�ǽ��յ������ݵĵ�ַ����recvfrom�������� */
		iRecvLen = recvfrom(g_iSocketServer, ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (iRecvLen > 0)
		{
			ucRecvBuf[iRecvLen] = '\0';
			DBG_PRINTF("Get Msg From %s : %s\n", inet_ntoa(tSocketClientAddr.sin_addr), ucRecvBuf);

			/* ��������:
			 * setclient            : ���ý��մ�ӡ��Ϣ�Ŀͻ���
			 * resetclient          : �رս��մ�ӡ��Ϣ�Ŀͻ���
			 * dbglevel=0,1,2...    : �޸Ĵ�ӡ����
			 * stdout=0             : �ر�stdout��ӡ
			 * stdout=1             : ��stdout��ӡ
			 * netprint=0           : �ر�netprint��ӡ
			 * netprint=1           : ��netprint��ӡ
			 */
			if (strcmp(ucRecvBuf, "setclient")  == 0)
			{
				g_tSocketClientAddr = tSocketClientAddr;
				g_iHaveConnected = 1;
				printf("setclient %s:%d\n", inet_ntoa(tSocketClientAddr.sin_addr), ntohs(tSocketClientAddr.sin_port));
				/* ����������ڵ����ݵ��ͻ��� */
				NetDbgFlush();
			}
			else if (strcmp(ucRecvBuf, "resetclient")  == 0)
			{
				memset(&g_tSocketClientAddr, 0x00, sizeof(g_tSocketClientAddr));
				g_iHaveConnected = 0;
			}
			else if (strncmp(ucRecvBuf, "dbglevel=", 9) == 0)
			{
				SetDbgLevel(ucRecvBuf);
			}
			else if ((strncmp(ucRecvBuf, "stdout=", 7) == 0)
					|| (strncmp(ucRecvBuf, "netprint=", 9) == 0))
			{
				SetDbgChannel(ucRecvBuf);
			}
		}
	}

	return NULL;
}

int NetDbgInit(void)
{
	/* �����̵ĳ�ʼ�� */
	int iRet;
	
	/* tcp��SOCK_STREAM��udp��SOCK_DGRAM */
	g_iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == g_iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	g_tSocketServerAddr.sin_family			= AF_INET;
	g_tSocketServerAddr.sin_port			= htons(SERVER_PORT);  	/* host to net, short */
 	g_tSocketServerAddr.sin_addr.s_addr	= INADDR_ANY;			/* ��������IP */
	memset(g_tSocketServerAddr.sin_zero, 0, 8);

	/* ��socket�Ķ˿ں�IP */
	iRet =  bind(g_iSocketServer, (const struct sockaddr *)&g_tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	/* ���뻺���� */
	g_pcNetPrintBuf = malloc(PRINT_BUF_SIZE);
	if (NULL == g_pcNetPrintBuf)
	{
		close(g_iSocketServer);
		return -1;
	}

	/* ����netprint�����߳�: ���������ʹ�ӡ��Ϣ���ͻ��� */
	pthread_create(&g_tSendTreadID, NULL, NetDbgSendTreadFunction, NULL);			
	
	/* ����netprint�����߳�: �������տ�����Ϣ,�����޸Ĵ�ӡ����,��/�رմ�ӡ */
	pthread_create(&g_tRecvTreadID, NULL, NetDbgRecvTreadFunction, NULL);	

	return 0;
}

int NetDbgExit(void)
{
	/* �ر�socket,... */
	close(g_iSocketServer);

	/* �ͷŻ����� */
	if(g_pcNetPrintBuf != NULL)
	{
		free(g_pcNetPrintBuf);
	}

	return 0;
}
int NetDbgPrint(char *strData)
{
	/* �����ݷ��뻷�λ����� */
	int i;
	int strLen;

	strLen = strlen(strData);
	
	for (i = 0; i < strLen; i++)
	{
		if (0 != PutData(strData[i]))
			break;
	}

	/* ����Ѿ��пͻ���������, �Ͱ�����ͨ�����緢�͸��ͻ��� */
	/* ����netprint�ķ����߳� */
	pthread_mutex_lock(&g_tNetDbgSendMutex);
	pthread_cond_signal(&g_tNetDbgSendConVar);
	pthread_mutex_unlock(&g_tNetDbgSendMutex);

	return i;
}

/* ˢ�»����������ݵ��ͻ��� */
int NetDbgFlush(void)
{
	return NetDbgPrint("");
}
	
int NetPrintInit(void)
{
	return RegisterDebugOpr(&g_tNetDbgOpr);
}
