#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

	if( connect( sock, (struct sockaddr*)&address, sizeof(address) ) < 0 )
	{
		printf( "error is\n");
	}
	else 
	{
		const char* ob_data = "abc";
		const char* nr_data = "123";

		send( sock, ob_data, strlen( ob_data), 0);
		send( sock, nr_data, strlen( nr_data), MSG_OOB);
		send( sock, ob_data, strlen( ob_data), 0);
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
