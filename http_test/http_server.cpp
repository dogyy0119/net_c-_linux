#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

/* 主机状态的可能两种状态，分别表示：当前正在分析请求行，当前正在分析的头部字段 */
enum CHECK_STATE 
{
	CHECK_STATE_REQUESTLINE = 0,
	CHECK_STATE_HEADER
};

/* 从状态机的三种可能状态，即行的读取状态，分别表示：读取到一个完整的行，行出错和行数据尚且不完整 */
enum LINE_STATUS
{
	LINE_OK,
	LINE_BAD,
	LINE_OPEN
};

enum HTTP_CODE{
	NO_REQUEST,
	GET_REQUEST,
	BAD_REQUEST,
	FORBIDDEN_REQUEST,
	INTERNAL_ERROR,
	CLOSED_CONNECTION
};

/* 为了简化问题，外面没有给客户端发送一个完整的HTTP 应答报文，而只是根据服务器的处理结果发送如下成功或者失败通知 */
static const char* szret[] = { "I get a correct result \n", "Something wrong \n " };

/* 从状态机，用于解析出一行内容 */
LINE_STATUS parse_line( char* buffer, int& checked_index, int& read_index )
{
	char temp;
	/* */
	for ( ; checked_index < read_index; ++checked_index )
	{
		/* 获得当前要分析的字节 */
		temp = buffer[ checked_index ];
		
		/* 如果当前的字节是 "\r"，则说明可能读取到了一个完整的行 */
		if ( temp == '\r' ) 
		{
			/* 如果 "\r" 字符碰巧是目前 buffer 中的最后一个被读入的客户端数据，那么这次分析没有读到一个完整的行，返回 LINE_OPEN 
			   表示还要继续读取客户端数据才能进一步分析 */
			if ( ( checked_index + 1 ) == read_index )
			{
				return LINE_OPEN;
			}
			/**/
			else if ( buffer [ checked_index + 1 ] == '\n')
			{
				buffer[ checked_index++ ] = '\0';
				buffer[ checked_index++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if ( temp == '\n' )
		{
			if ( ( checked_index > 1 ) && ( buffer[ checked_index -1] == '\r' ) )
			{
				buffer[ checked_index-1 ] = '\0';
				buffer[ checked_index++ ] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
	return LINE_OPEN;
}

/* 分析请求行 */
HTTP_CODE parse_requestline( char* temp, CHECK_STATE& checkstate )
{
	char* url = strpbrk( temp, "\r" );
	if ( !url )
	{
		return BAD_REQUEST;
	}
	*url++ = '\0';

	char* method = temp;
	if ( strcasecmp( method, "GET" ) == 0 ) /* 仅支持 GET 方法 */
	{
		printf( "The request method is GET \n" );
	}
	else 
	{
		return BAD_REQUEST;
	}
	
	url += strspn( url, " \t");
	char* version = strpbrk( url, " \t");
	if ( !version )
	{
		return BAD_REQUEST;
	}
	*version++ = '\0';
	version += strspn( version, " \t" );
	/* 仅支持 HTTP/1.1 */
	if ( strcasecmp( version, "HTTP/1.1") != 0 )
	{
		return BAD_REQUEST;
	}
	/* 检查 URL 是否合法 */
	if ( strncasecmp( url, "http://", 7 ) == 0 )
	{
		url += 7;
		url = strchr( url, '/' );
	}
	
	if ( !url || url[ 0 ] != '/' )
	{
		return BAD_REQUEST;
	}
	printf( "The request URL is: %s \n", url );
	checkstate = CHECK_STATE_HEADER;
	return NO_REQUEST;
}	

/* 分析头部字段 */
HTTP_CODE parse_headers( char* temp )
{
	/* 遇到一个空行，说明我们得到了一个正确的 HTTP 请求 */
	if ( temp[ 0 ] == '\0' )
	{
		return GET_REQUEST;
	}
	else if ( strncasecmp( temp, "Host:", 5) == 0 ) /* 处理 "HOST" 头部字段 */
	{
		temp += 5;
		temp += strspn( temp, "\t" );
		printf( "the request host is: %s\n ", temp );
	}
	else  /* 其他头部字段都不处理 */
	{
		printf( "I can not handle this header \n" );
	}
	return NO_REQUEST;
}

HTTP_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line )
{
	LINE_STATUS linestatus = LINE_OK;
	HTTP_CODE retcode = NO_REQUEST;
	/*  主状态机，用与从 buffer 中取出所有的完整的行 */
	while ( ( linestatus = parse_line( buffer, checked_index, read_index ) ) == LINE_OK ) 
	{
		char* temp = buffer + start_line;
		start_line = checked_index;
		/* checkestatus 记录主状态机当前的状态 */
		switch ( checkstate) 
		{
			case CHECK_STATE_REQUESTLINE:
			{	
				retcode = parse_requestline( temp, checkstate );
				if ( retcode == BAD_REQUEST )
				{
					return BAD_REQUEST;
				}
				break;
			}
			case CHECK_STATE_HEADER:
			{
				retcode = parse_headers( temp );
				if ( retcode == BAD_REQUEST )
				{
					return BAD_REQUEST;
				}
				else if ( retcode == GET_REQUEST )
				{
					return GET_REQUEST;
				}
				break;
			}
			default:
			{
				return INTERNAL_ERROR;
			}
		}

	}

	/* 若没有读取到一个完整的行，则表示还需要继续读取客户端数据才能进一步分析 */
	if ( linestatus == LINE_OPEN )
	{
		return NO_REQUEST;
	}
	else 
	{
		return BAD_REQUEST;
	}
}

int main( int argc, char* argv[] )
{
	if( argc <= 2 )
	{
		printf( "wrong!!!\n " );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2]);
	
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd != -1 );
	int ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
	assert( ret != -1 );
	ret = listen( listenfd, 5 );
	assert( ret != -1 );
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof( client_address );
	int fd = accept( listenfd, (struct sockaddr* )&client_address, &client_addrlength);
	if ( fd < 0 )
	{
		printf( "error is: %d \n", errno );
	}
	else 
	{
		char buffer[ BUFFER_SIZE ]; /* 读缓冲区 */
		memset( buffer, '\0', BUFFER_SIZE );
		int data_read = 0;
		int read_index = 0;
		int checked_index = 0;
		int start_line = 0;

		CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;
		while(1)
		{
			data_read = recv( fd, buffer + read_index, BUFFER_SIZE - read_index, 0 );
			if ( data_read == -1 )
			{
				printf( "reading failed!!! \n" );
			}
			else if ( data_read == 0 )
			{
				printf( "remote client has closed the connection!!! \n" );
			}
			read_index += data_read;
			/* 分析目前已经获得的所有客户端数据 */
			HTTP_CODE result = parse_content( buffer, checked_index, checkstate, read_index, start_line );
			if ( result == NO_REQUEST ) /* 尚未得到一个完整的 HTTP 请求 */
			{
				continue;
			}
			else if ( result == GET_REQUEST ) /* 得到一个完整的、正确的 HTTP 请求*/
			{
				send( fd, szret[0], strlen( szret[0] ), 0 );
				break;
			}
			else
			{
				send( fd, szret[1], strlen( szret[1] ), 0 );
				break;
			}	
		}
		close( fd );
	}
	close( listenfd );
	return 0;
}
