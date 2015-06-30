#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/* int socket(int domain, int type, int protocol); */

/*
 * socket()
 * bind()
 * listen()
 * accept()
 * send()
 * recv()
 */

#define SERVER_PORT	8888
#define BACKLOG		10			/* �������ӵĶ�����󳤶� */

int main(int argc, char *argv[])
{
	int iSocketServer;
	int iSocketClient;

#if 0
	struct sockaddr_in 
	{
		short int sin_family;
		unsigned short int sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};

	/* Internet address. */
	struct in_addr 
	{
		uint32_t       s_addr;     /* address in network byte order */
	};
#endif
	
	struct sockaddr_in tSocketServerAddr;
	struct sockaddr_in tSocketClientAddr;
	
	int iRet;
	int iAddrLen;

	int iRecvLen;
	unsigned char ucRecvBuf[1000];

	int iClientNum = -1;

	/* �ӽ����˳�ʱ���򸸽��̷���SIGCHLD�źţ������� */
	signal(SIGCHLD,SIG_IGN);

	iSocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family			= AF_INET;
	tSocketServerAddr.sin_port			= htons(SERVER_PORT);  	/* host to net, short */
 	tSocketServerAddr.sin_addr.s_addr	= INADDR_ANY;			/* ��������IP */
	memset(tSocketServerAddr.sin_zero, 0, 8);

	/* ��socket�Ķ˿ں�IP */
	iRet =  bind(iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	/* ��ʼ�����˿� */
	iRet = listen(iSocketServer, BACKLOG);
	if (-1 == iRet)
	{
		printf("listen error!\n");
		return -1;
	}

	while(1)
	{
		iAddrLen = sizeof(struct sockaddr);

		/* ������һ���ͻ��˷�������� */
		iSocketClient = accept(iSocketServer, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (-1 != iSocketClient)
		{
			iClientNum++;
			printf("Get connect from client %d : %s\n",  iClientNum, inet_ntoa(tSocketClientAddr.sin_addr));

			/* �����ӽ��� */
			if (!fork())
			{
				/* �ӽ��̵�Դ�� */
				while (1)
				{
					/* ���տͻ��˷��������ݲ���ʾ���� */
					iRecvLen = recv(iSocketClient, ucRecvBuf, 999, 0);
					if (iRecvLen <= 0)
					{
						/* �ر�socket */
						close(iSocketClient);

						/* �ӽ����˳� */
						return -1;
					}
					else
					{
						ucRecvBuf[iRecvLen] = '\0';
						printf("Get Msg From Client %d: %s\n", iClientNum, ucRecvBuf);
					}
				}				
			}
			/* ������ִ�� */
			else
			{
				
			}
		}
	}

	/* �رշ�������socket */
	close(iSocketServer);
	
	return 0;
}
