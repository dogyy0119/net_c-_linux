#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define BUFFER_SIZE 1024


int main( int argc, char* argv[] )
{
	if (argc <= 3)
	{
		printf( "wrong!!!!! \n" );
		return 1;
	}

	const char* ip = argv[1];
	int port = atoi( argv[2] );
	const char* file_name =  argv[3];

	printf( "ip:%s \n", ip );
	printf( "port:%d \n", port );
	printf( "file_name:%s \n", file_name );

	int filefd = open( file_name, O_RDONLY );
	assert( filefd > 0 );
	struct stat stat_buf;
	fstat( filefd, &stat_buf );

	int sock = socket( PF_INET, SOCK_STREAM, 0);
	assert( sock >= 0 );
	
	/*创建一个IPCV4 socket 连接*/
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );
	assert( ret != 1 );

	ret = listen( sock, 5 );
	assert( ret != -1 );
	printf( "listen \n" );
	

	struct sockaddr_in client;
	socklen_t client_addrlength = sizeof( client );
	int connfd = accept( sock, ( struct sockaddr* )&client, &client_addrlength );

	printf( "accept \n" );
	if( connfd < 0 )
	{
		printf( "error is\n");
	}
	else 
	{
		sendfile( connfd, filefd, NULL, stat_buf.st_size );
		close( connfd );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
