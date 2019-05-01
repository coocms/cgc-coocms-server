#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char *argv[])
{
	if(3 != argc)
	{
		printf("Usage: %s <IP> <PROT>\n", argv[0]);
		return -1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
	if(-1 == fd)
	{
		perror("socket ");
		return -1;	
	}	

	//描述目标地址
	struct sockaddr_in destaddr = {0};
	destaddr.sin_family = AF_INET;
	destaddr.sin_port = htons(atoi(argv[2]));
	destaddr.sin_addr.s_addr = inet_addr(argv[1]);
	if(-1 == connect(fd, (struct sockaddr*)&destaddr, sizeof destaddr) )//与对方握手，判断是否连通
	{
		perror("connect");
		return -1;
	}
	while(1)
	{
		char buf[100] = {"time"};
		printf("input> ");fflush(stdout);
		gets(buf);
		write(fd, buf, sizeof buf);//发数据给对方
		read(fd, buf, sizeof buf); //接受反馈
		printf("recv: %s\n", buf);
		
	}
}
