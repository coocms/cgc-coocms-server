#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

void *serv_client(void *arg)
{
	int clientfd = (int)arg;
	char buf[100] = {0};
	while(1)
	{
		memset(buf, 0, sizeof buf);
		int ret = read(clientfd, buf, 100);
		if(0 == ret)
			break;
		printf("recv: %s\n", buf);

		if(0 == strcmp("client", buf) )
		{
			unsigned long t = time(NULL);
			sprintf(buf, "%s", ctime(&t));
		}
		else
			strcpy(buf, "unknow.........");


		write(clientfd, buf, 100);
	}
}
void *hardware_client(void *arg)
{
	int clientfd = (int)arg;
	char buf[100] = {0};
	write(clientfd, "hello hardware", 14);
	
	while(1)
	{
		memset(buf, 0, sizeof buf);
		int ret = read(clientfd, buf, 100);
		if(0 == ret)
			break;
		printf("recv: %s\n", buf);

		if(0 == strcmp("hard", buf) )
		{
			unsigned long t = time(NULL);
			sprintf(buf, "%s", ctime(&t));
		}
		else
			strcpy(buf, "unknow.........");


		write(clientfd, buf, 100);
		
	}
	
	
}

int main()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
	char buf[100] = {0};
	if(-1 == fd)
	{
		perror("socket ");
		return -1;	
	}	

	//IP PORT ???? struct sockaddr
	struct sockaddr_in destaddr = {0};
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = htons(57008);
	destaddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("0.0.0.0");表明知道是（通过任意本地网卡）到达这台电脑的数据都不拒绝
	if(-1 == bind(fd, (struct sockaddr*)&destaddr, sizeof destaddr) )//向操作系统交代我对某个端口感兴趣
	{
		perror("bind");
		return -1;
	}
	listen(fd, 10);

	printf("waiting for client....................\n");

	struct sockaddr_in QTaddr = {0};
	struct sockaddr_in hardwareAddr = {0};
	
	
	int len = 0;
	
	while(1)
	{
			int hardwareFd = accept(fd, (struct sockaddr*)&hardwareAddr, &len);//等待底层连接
			printf("client: %s\n", inet_ntoa(hardwareAddr.sin_addr));
			memset(buf, 0, sizeof(buf));
			int ret = read(hardwareFd, buf, 100);
			printf("recv: %s\n", buf);
			
			if(strncmp("hardware", buf, 8) == 0)
			{
				pthread_t t;
				pthread_create(&t, NULL, hardware_client, (void*)hardwareFd);
				printf("硬件已连接\n");
				break;
			}
			strcpy(buf, "硬件未连接\n");
			write(hardwareFd, buf, 100);
		
	}


	
	
	
	
	
	
	while(1)
	{

		int clientfd = accept(fd, (struct sockaddr*)&QTaddr, &len);//接电话并生成新的用于真正通信的套接字（clientfd）

		//把客户端的IP转成人能识别的字符串样式,如: 0xff00ff00->"255.0.255.0"
		printf("client: %s\n", inet_ntoa(QTaddr.sin_addr));

		pthread_t t;
		pthread_create(&t, NULL, serv_client, (void*)clientfd);
	}
}
