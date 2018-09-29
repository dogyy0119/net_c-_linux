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

int main( int argc, char* argv[] )
{

	
	if (argc <= 2)
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

	int sendbuf = atoi( argv[3] );
	int len = sizeof( sendbuf );

	/* 先设置TCP发送缓冲区的大小，然后立即读取之 */
	setsockopt( sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof( sendbuf ) );
	getsockopt( sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, ( socklen_t* )&len );
	printf( "the tcp send buffer size after setting is %d \n", sendbuf );

	if( connect( sock, (struct sockaddr*)&address, sizeof(address) ) < 0 )
	{
		printf( "error is\n");
	}
	else 
	{
		char buffer[BUFFER_SIZE];
		memset( buffer, 'a', BUFFER_SIZE );
		send( sock, buffer, BUFFER_SIZE, 0 );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
