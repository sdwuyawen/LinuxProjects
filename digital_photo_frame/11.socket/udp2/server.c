#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

/* int socket(int domain, int type, int protocol); */

/*
 * socket()
 * bind()
 * sendto()
 * recvfrom()
 */

#define SERVER_PORT	8888

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

	/* tcp用SOCK_STREAM，udp用SOCK_DGRAM */
	iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
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

	while(1)
	{
		iAddrLen = sizeof(struct sockaddr);

		/* tSocketClientAddr是接收到的数据的地址，由recvfrom函数返回 */
		iRecvLen = recvfrom(iSocketServer, ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (iRecvLen > 0)
		{
			ucRecvBuf[iRecvLen] = '\0';
			printf("Get Msg From %s : %s\n", inet_ntoa(tSocketClientAddr.sin_addr), ucRecvBuf);
		}
	}

	/* 关闭服务器的socket */
	close(iSocketServer);
	
	return 0;
}
