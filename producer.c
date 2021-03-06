#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 9527

int main(void)
{
	int sockfd;
	int ret = 0;
	char buf[1024] = {0};
	char input[1024] = {0};
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("create socket error\n");	
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("connect error\n");	
		return -1;
	}

	strncpy(input, "putmsg#aoligei", 1024);
	write(sockfd, input, strlen(input));

	close(sockfd);

	return 0;
}

