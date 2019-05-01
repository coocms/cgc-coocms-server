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
	
	while(1)
	{
		char buf[100] = {0};
		int ret = read(clientfd, buf, 100);
		if(0 == ret)
			break;
		printf("recv: %s\n", buf);

		if(0 == strcmp("time", buf) )
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

	struct sockaddr_in clientaddr = {0};
	int len = sizeof clientaddr;

	while(1)
	{

		int clientfd = accept(fd, (struct sockaddr*)&clientaddr, &len);//接电话并生成新的用于真正通信的套接字（clientfd）

		//把客户端的IP转成人能识别的字符串样式,如: 0xff00ff00->"255.0.255.0"
		printf("client: %s\n", inet_ntoa(clientaddr.sin_addr));

		pthread_t t;
		pthread_create(&t, NULL, serv_client, (void*)clientfd);
	}
}
