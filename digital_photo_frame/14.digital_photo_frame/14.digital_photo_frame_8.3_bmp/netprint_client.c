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

#define SERVER_PORT 5678


/*
 * ./netprint_client <server_ip> dbglevel=<0-9>
 * ./netprint_client <server_ip> stdout=0|1
 * ./netprint_client <server_ip> netprint=0|1
 * ./netprint_client <server_ip> show // setclient,并且接收打印信息
 */
int main(int argc, char *argv[])
{
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

	int iSocketClient;
	struct sockaddr_in tSocketServerAddr;
	struct sockaddr_in tSocketClientAddr;
	
	int iRet;

	int iSendLen;
	unsigned char ucSendBuf[1000];
	int iRecvLen;
	unsigned char ucRecvBuf[1000];
	int iAddrLen;


	if (argc != 3)
	{
		printf("Usage:\n");
		printf("%s <server_ip> dbglevel=<0-9>\n", argv[0]);
		printf("%s <server_ip> stdout=0|1\n", argv[0]);
		printf("%s <server_ip> netprint=0|1\n", argv[0]);
		printf("%s <server_ip> show\n", argv[0]);
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

	if (strcmp(argv[2], "show") == 0)
	{
		/* 向socket发送数据 */
		iSendLen = sendto(iSocketClient, "setclient", 9, 0,
							(const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
		if (iSendLen <= 0)
		{
			close(iSocketClient);
			return -1;
		}

		while(1)
		{
			iAddrLen = sizeof(struct sockaddr);
			/* tSocketClientAddr是接收到的数据的地址，由recvfrom函数返回 */
			iRecvLen = recvfrom(iSocketClient, ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
			if (iRecvLen > 0)
			{
				ucRecvBuf[iRecvLen] = '\0';
				printf("%s", ucRecvBuf);
			}
		}
	}
	else
	{
		/* 向socket发送数据 */
		iSendLen = sendto(iSocketClient, argv[2], strlen(argv[2]), 0,
							(const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));

		if (iSendLen <= 0)
		{
			close(iSocketClient);
			return -1;
		}
	}
	
	return 0;
}

