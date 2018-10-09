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

#define BUFFER_SIZE 1024

static bool stop = false;

static const char* status_line[2] = { "200 OK", "500 Internal server error"};

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
	const char* file_name =  argv[3];

	printf( "ip:%s \n", ip );
	printf( "port:%d \n", port );
	printf( "file_name:%s \n", file_name );

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
		char header_buf[ BUFFER_SIZE ];
		memset( header_buf, '\0', BUFFER_SIZE );
			
		/* 用于存放目标文件内容的应用程序缓存 */	
		char* file_buf;
		/* 用于存放目标文件的属性，比如是否为目录，文件大小等 */
		struct stat file_stat;
		/* 记录目标文件是否是有效文件 */
		bool valid = true;
		/* 缓存区 header_buf 目前已经使用了多少字节空间 */
		int len = 0;
		
		if( stat( file_name, &file_stat ) < 0 ) /* 文件不存在 */
		{
			valid = false;
		}				
		else 
		{
			if( S_ISDIR( file_stat.st_mode ) ) /* 目标文件是一个目录*/
			{
				valid = false;	
			}
			else if( file_stat.st_mode & S_IROTH ) /* 当前用户有读取目标文件的权限 */
			{
				/* 动态分配缓冲区 file_buf, 并指定其大小为目标文件的大小 file_stat.st_size 加1，
				   然后将目标文件读入缓存区 file_buf 中 */
				int fd = open( file_name, O_RDONLY );
				file_buf = new char[ file_stat.st_size + 1];
				memset( file_buf, '\0', file_stat.st_size +1 );
				if( read( fd, file_buf, file_stat.st_size) < 0 )
				{	
					valid = false;
				}
			}
			else 
			{
				valid = false;
			}
			
			if( valid = true ) 
			{
				/* 下面这部分内容将HTTP应答的状态行、"Content-Length" 头部字段和一个空行依次加入 
				   header_buf 中*/
				ret = snprintf( header_buf, BUFFER_SIZE - 1,"%s %s\r\n",
						"HTTP/1.1", status_line[0] );
				len += ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE - 1 - len,
                                                "Content-Length: %d\r\n", file_stat.st_size );
				len += ret;
				ret = snprintf( header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n" );
				
				/* 使用writev 将head_buf 和 file_buf 一并发出 */
				struct iovec iv[2];
				iv[ 0 ].iov_base = header_buf;
				iv[ 0 ].iov_len  = strlen( header_buf );
				iv[ 1 ].iov_base = file_buf;
                                iv[ 1 ].iov_len  = strlen( file_buf );
				ret = writev( connfd, iv , 2 );	
				
				printf( " send file over!!!! \n" );
			}
			else
			{
				ret = snprintf( header_buf, BUFFER_SIZE - 1,"%s %s\r\n",
                                                "HTTP/1.1", status_line[1] );
				ret = snprintf( header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n" );
				send( connfd, header_buf, strlen( header_buf ), 0 );
				
				printf( " send file false!!!! \n" );
			}
			
			delete [] file_buf;			
		}
		
		close( connfd );
	}

	/* 关闭 socket*/
	close( sock );
	return 0;
}
