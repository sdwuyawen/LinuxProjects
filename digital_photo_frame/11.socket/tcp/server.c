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
#define BACKLOG		10			/* 请求连接的队列最大长度 */

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

	/* 子进程退出时会向父进程发送SIGCHLD信号，父进程 */
	signal(SIGCHLD,SIG_IGN);

	iSocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family			= AF_INET;
	tSocketServerAddr.sin_port			= htons(SERVER_PORT);  	/* host to net, short */
 	tSocketServerAddr.sin_addr.s_addr	= INADDR_ANY;			/* 本机所有IP */
	memset(tSocketServerAddr.sin_zero, 0, 8);

	/* 绑定socket的端口和IP */
	iRet =  bind(iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	/* 开始监听端口 */
	iRet = listen(iSocketServer, BACKLOG);
	if (-1 == iRet)
	{
		printf("listen error!\n");
		return -1;
	}

	while(1)
	{
		iAddrLen = sizeof(struct sockaddr);

		/* 接受了一个客户端发起的连接 */
		iSocketClient = accept(iSocketServer, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (-1 != iSocketClient)
		{
			iClientNum++;
			printf("Get connect from client %d : %s\n",  iClientNum, inet_ntoa(tSocketClientAddr.sin_addr));

			/* 创建子进程 */
			if (!fork())
			{
				/* 子进程的源码 */
				while (1)
				{
					/* 接收客户端发来的数据并显示出来 */
					iRecvLen = recv(iSocketClient, ucRecvBuf, 999, 0);
					if (iRecvLen <= 0)
					{
						/* 关闭socket */
						close(iSocketClient);

						/* 子进程退出 */
						return -1;
					}
					else
					{
						ucRecvBuf[iRecvLen] = '\0';
						printf("Get Msg From Client %d: %s\n", iClientNum, ucRecvBuf);
					}
				}				
			}
			/* 父进程执行 */
			else
			{
				
			}
		}
	}

	/* 关闭服务器的socket */
	close(iSocketServer);
	
	return 0;
}
