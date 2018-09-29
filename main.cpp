#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool stop = false;

static void handle_term( int sig )
{
	stop = true;
}

int main( int argc, char* argv[] )
{

	signal( SIGTERM, handle_term );
	
	if (argc <= 3)
	{
		printf( "wrong!!!!! \n" );
		return 1;
	}

	const char* ip = argv[1];
	int port = atoi( argv[2] );
	int backlog = atoi( argv[3] );

	printf( "ip:%s \n", ip );
	printf( "port:%d \n", port );
	printf( "backlog:%d \n", backlog );

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

	ret = listen( sock, backlog );
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
		char buffer[1024];
		memset( buffer, '\0', 1024 );
		ret = recv( connfd, buffer, 1023, 0 );
		printf( "get %d bytes of normal data %s \n", ret, buffer );
		
		memset( buffer, '\0', 1024 ); 
		ret = recv( connfd, buffer, 1023, MSG_OOB );
		printf( "get %d bytes of normal data %s \n", ret, buffer );
		
		memset( buffer, '\0', 1024 ); 
		ret = recv( connfd, buffer, 1023, 0 );
		printf( "get %d bytes of normal data %s \n", ret, buffer );

		close( connfd );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
