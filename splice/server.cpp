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
		int pipefd[2];
		assert( ret != -1);
		ret = pipe( pipefd );

		/* 将connfd 上流入的客户数据定向到管道中 */
		ret = splice( connfd, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE );

		assert( ret != -1 );
		
		/* 将管道输出的定向到 connfd 客户连接文件描述符 */
		ret = splice( pipefd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE );
		assert( ret != -1 );
	
		close( connfd );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
