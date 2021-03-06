#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<linux/usbdevice_fs.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>

#define SIZE sizeof(struct sockaddr_in)

int server_listen_accept()
{
    struct sockaddr_in server;
	int sockfd_connect;
	int sockfd_listen;
    
	if((sockfd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("fail to call socket()\n");
		exit(1);
	}
    
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(3700);
    
	if(bind(sockfd_listen, (struct sockaddr *)&server, SIZE) == -1)
	{
		printf("fail to call bind()\n");
		exit(1);
	}
	if(listen(sockfd_listen, 5) == -1)
	{
		printf("fail to call listen()\n");
		exit(1);
	}
	printf("sockfd_listen : %d\n", sockfd_listen);
    if((sockfd_connect = accept(sockfd_listen, NULL, NULL)) ==-1)
    {
		printf("fail to call accept()\n");
		exit(1);
	}
	return sockfd_connect;
}

int send_dev(int sockfd_connect, char* cmd, int cmd_size) {
    int ret, i;
	char cmd_tmp;
	for(i=0; i<cmd_size; i++) {
		cmd_tmp = cmd[i];
	    ret = send(sockfd_connect, &cmd_tmp, 1, 0);
	    if(ret<=0) {
			printf("send_dev() send error (%c) : %d/%d\n", cmd_tmp, i, cmd_size);
			return -1;
		}
	}
	return i;
}

int recv_dev(int sockfd_connect, struct usbdevfs_urb *urb) {
	int ret;
	ret = recv(sockfd_connect, urb, sizeof(struct usbdevfs_urb), 0);
	return ret;
}

int main() {
	//int sockfd=-1, sock_listen;
	char str[10];
	struct usbdevfs_urb urb;
	int sockfd_connect;
	int sockfd_listen;

	sockfd_connect = server_listen();
	printf("sock_listen : %d\n", sockfd_listen);
	printf("!!  !!\n");
	printf("accept : %d\n", sockfd_connect);
	strcpy(str, "1");
	printf("send : %d\n", send_dev(sockfd_connect, str, strlen(str)));
	//recv_dev(sockfd, &urb);
	close(sockfd_connect);
	close(sockfd_listen);
}
