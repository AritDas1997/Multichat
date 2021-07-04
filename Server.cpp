#include<fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<time.h>

#define PORT 4950
#define BUFSIZE 1024

using namespace std ;
//int p ;
ofstream q ;
char a[20] ;


//time_t finish = time(NULL) ;
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			char buf[1000] ;
			memset(buf,0,sizeof(buf)) ;
			sprintf(buf, "Socket %d ",i) ;
		//	strcat(buf,":") ;
			char *st=buf ;
			if (send(j, st,sizeof(st), 0) == -1){
				perror("send") ;
			}
			if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
				perror("send");
			}
	         
		}
	}	
}
		
void send_recv(int i, fd_set *master, int sockfd, int fdmax,struct sockaddr_in *client_addr)
{
	int nbytes_recvd, j;
	char recv_buf[BUFSIZE], buf[BUFSIZE];
	memset(recv_buf,0,sizeof(recv_buf)) ;	
	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			q.open("Logout.txt", ios::app) ;
			time_t finish = time(NULL) ;
			q<<"Log out time of socket "<<i<<": "<<ctime(&finish)<<"\n" ;
			q.close() ;
			printf("socket %d left the group\n",i);
		}else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, master);
	}else { 
		printf("Socket %d: %s\n",i,recv_buf);
		if(a[i]==0)
		{
		  q.open("Logout.txt", ios::app) ;	
		  time_t start = time(NULL) ;
	          q<<"Log in time of socket "<<i<<": "<<ctime(&start)<<"\n" ;
		  q.close() ;
		  a[i]=1 ;

		}	
		for(j = 0; j <= fdmax; j++){
			send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master );
		}
	}	
}
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;
		
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	//	time_t start = time(NULL) ;
	//	q.open("Logout.txt", ios::app) ;
	//	q<<"Login time of Socket "<<ntohs(client_addr->sin_port)<<":"<<ctime(&start)<<"\n" ;
	//	q.close() ;
	}
}
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr, long int po)
{
	int yes = 1;
			
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
			
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(po) ;
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, ' ', sizeof my_addr->sin_zero);
			
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
			
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCPServer Waiting for client\n");
	fflush(stdout);
}
int main(int argc, char *argv[])
{
	long int y=atoi(argv[1]) ;
	for(int k=0;k<=15;k++) 
		a[k]=0 ;
	q.open("Logout.txt", ios::trunc) ;
	q.close() ;
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;
//	printf("\nEnter port number:") ;
//	scanf("%d",&p) ; 
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr,y);
	FD_SET(sockfd, &master);
		
	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
				
		for(i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd){
					connection_accept(&master, &fdmax, sockfd, &client_addr);
				}
				else
					send_recv(i, &master, sockfd, fdmax, &client_addr);
			}
		}
	}
	return 0;
}
