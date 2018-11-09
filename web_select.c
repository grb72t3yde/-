#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

char webpage[] =
"HTTP/1.1 200 OK \r\n"
"ContexT-Type: text/html;charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>ShellWaveX</title>\r\n"
"<style>body {background-color: #FFFF00 }</style></head>\r\n"
"<body><center><h1>Hello World!</h1><br>\r\n"
"<img src=\"try.jpg\"></center></body></html>\r\n";


int main(int argc,char* argv[]){
	struct sockaddr_in server_addr,client_addr;
	socklen_t sin_len = sizeof(client_addr);
	int fd_server,fd_client;

	fd_set active_fd_set, read_fd_set;
	char buf[2048];
	int fdimg;
	int i=0;
	int on = 1;
	
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_server < 0){
		perror("socket");
		exit(1);
	}

	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr))== -1){
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server, 10)== -1){
		perror("bind");
		close(fd_server);
		exit(1);
	}

	FD_ZERO(&active_fd_set);
	FD_SET(fd_server, &active_fd_set);

	while(1){
		read_fd_set = active_fd_set;
		if(select(FD_SETSIZE, &read_fd_set, NULL, NULL,NULL) < 0){
			perror("select");
			exit(EXIT_FAILURE);
		}


		for(i=0;i<FD_SETSIZE;i++)
			if(FD_ISSET(i,&read_fd_set)){
				if(i == fd_server){   //connetion request on listening server
					int new;
					sin_len = sizeof(client_addr);
					new = accept(fd_server, (struct sockaddr*) &client_addr, &sin_len);

					if(new == -1){
						perror("Connection failed ...\n");
						exit(EXIT_FAILURE);
					}

					printf("Got client connection ...\n");
					FD_SET(new, &active_fd_set);
				}
				else{	
		//			close(fd_server);
					memset(buf,0, 2048);
					read(i,buf,1047);

					printf("%s\n",buf);

					if(!strncmp(buf, "GET /try.jpg", 12)){
						fdimg = open("try.jpg", O_RDONLY);
						sendfile(i, fdimg,NULL,500000);
						close(fdimg);
					}
					else
						write(i, webpage, sizeof(webpage)-1);

					close(i);
					FD_CLR(i, &active_fd_set);
				}
			}
	}
}
				



