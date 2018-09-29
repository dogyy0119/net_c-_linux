#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 512

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

	printf( "ip:%s \n", ip );
	printf( "port:%d \n", port );

	int sock = socket( PF_INET, SOCK_STREAM, 0);
	assert( sock >= 0 );
	
	/*创建一个IPCV4 socket 连接*/
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int recvbuf = atoi( argv[3] );
	int len = sizeof( recvbuf );

	/* 先设置TCP接收缓冲区大小，然后立即读取之*/
	setsockopt( sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof( recvbuf ) ); 
	getsockopt( sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, ( socklen_t* )&len );
	printf( "the tcp recive buffer size after setting is %d\n", recvbuf );

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
		char buffer[BUFFER_SIZE];
		
		memset( buffer, '\0', BUFFER_SIZE ); 
		while( recv( connfd, buffer, BUFFER_SIZE - 1, 0 ) >0 )
		{
			printf("recv \n");
			printf( " recv data %s \n",  buffer ); 
		}

		printf( "get %d bytes of normal data %s \n", ret, buffer );

		close( connfd );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
