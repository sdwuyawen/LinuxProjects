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
 * connect()
 * send()
 * recv()
 */

#define SERVER_PORT 8888

int main(int argc, char *argv[])
{
	int iSocketClient;
	struct sockaddr_in tSocketServerAddr;

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
	
	int iRet;

	int iSendLen;
	unsigned char ucSendBuf[1000];

	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}

	/* tcp用SOCK_STREAM，udp用SOCK_DGRAM */
	iSocketClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == iSocketClient)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family			= AF_INET;
	tSocketServerAddr.sin_port			= htons(SERVER_PORT);  	/* host to net, short */
 //	tSocketServerAddr.sin_addr.s_addr	= INADDR_ANY;					/* 本机所有IP */
	if (0 == inet_aton(argv[1], &tSocketServerAddr.sin_addr))		/* 服务器IP */
 	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tSocketServerAddr.sin_zero, 0, 8);

	/* 连接到tSocketServerAddr指定IP和端口的服务器 */
	iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));	
	if (-1 == iRet)
	{
		printf("connect error!\n");
		return -1;
	}

	while (1)
	{
		if (fgets(ucSendBuf, 999, stdin))
		{
			/* 向socket发送数据 */
			iSendLen = send(iSocketClient, ucSendBuf, strlen(ucSendBuf), 0);
			if (iSendLen <= 0)
			{
				close(iSocketClient);
				return -1;
			}
		}
	}
	
	return 0;
}
