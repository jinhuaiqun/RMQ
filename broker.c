#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define LISTEN_PORT 9527
#define MAXEVENTS 64

void make_nonblocking(int fd);

int main(int argc, char *argv[])
{
	int n = 0;
	int i = 0;;
	int listenfd = 0;
	int clifd = 0;
	int ret = 0;
	int epfd;
	struct epoll_event event;
	struct epoll_event *events;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t socklen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("socket error\n");
		return -1;
	}

	make_nonblocking(listenfd);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(LISTEN_PORT);

	ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0) {
		printf("bind error\n");	
		return -1;
	}

	ret = listen(listenfd, 2);
	if (ret < 0) {
		printf("listen error\n");	
		return -1;
	}


	epfd = epoll_create1(0);
	if (epfd < 0) {
		printf("epoll_create1 error\n");	
		return -1;
	}

	event.data.fd = listenfd;
	event.events = EPOLLIN;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);	
	if (ret < 0) {
		printf("epoll_ctl error\n");	
		return -1;
	}

	events = calloc(MAXEVENTS, sizeof(struct epoll_event));
	assert(events != NULL);

	while (1) {
		n = epoll_wait(epfd, events, MAXEVENTS, -1);
		if (n < 0) {
			printf("epoll_wait error\n");
			exit(-1);
		}

		for (i = 0; i < n; i++) {
			if ((events[i].events & EPOLLERR) 
			|| (events[i].events & EPOLLHUP)	
			|| !(events[i].events & EPOLLIN)) {
				printf("epoll error\n");	
				close(events[i].data.fd);
				continue;

			} else if (listenfd == events[i].data.fd) {
				int clifd = 0;
				struct sockaddr_in addr;
				socklen_t socklen = sizeof(addr);

				clifd = accept(listenfd, (struct sockaddr*)&addr, &socklen);
				if (clifd < 0) {
					printf("accept new fd error\n");	
					return -1;
				}

				printf("INFO: accept new client[%s:%d]\n", 
						inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

				make_nonblocking(clifd);
				event.data.fd = clifd;
				event.events = EPOLLIN;
				ret = epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &event);	
				if (ret < 0) {
					printf("epoll_ctl add conn fd error\n");	
					return -1;
				}
				continue;
			} else {
				int newfd = events[i].data.fd;
				while (1) {
					char buf[512] = {0};	
					if ((n = read(newfd, buf, sizeof(buf))) < 0) {
						if (errno != EAGAIN) {
							printf("read error\n");	
							return -1;
						}
						break;
					} else if (n == 0) {
						close(newfd);	
						break;
					} else {
						ret = write(newfd, buf, n);	
						if (ret < 0) {
							printf("write error\n");	
							return -1;
						}
					}
				}	
			}
		}
	}

	free(events);
	close(listenfd);
	return 0;
}

void make_nonblocking(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}
