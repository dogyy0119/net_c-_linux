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

/* 为了简化问题，外面没有给客户端发送一个完整的HTTP 应答报文，而只是根据服务器的处理结果发送如下成功或者失败通知 */
static const char* szret[] = { "I get a correct result \n", "Something wrong \n " };

/* 从状态机，用于解析出一行内容 */
LINE_STATUS parse_line( char* buffer, int& cheched_index, int& read_index )
{
	char temp;
	/* */
	for ( ; checked_index < read_index; ++checked_index )
	{
		/* 获得当前要分析的字节 */
		temp = buffer[ chacked_index ];
		
		/* 如果当前的字节是 "\r"，则说明可能读取到了一个完整的行 */
		if ( temp == '\r' ) 
		{
			/* 如果 "\r" 字符碰巧是目前 buffer 中的最后一个被读入的客户端数据，那么这次分析没有读到一个完整的行，返回 LINE_OPEN 
			   表示还要继续读取客户端数据才能进一步分析 */
			if ( ( checked_inde + 1 ) == read_index )
			{
				return LINE_OPEN;
			}
			/**/
			else if ( buffer [ check_index + 1 ] == '\n')
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
	printf( "The request URL is: %d\n", utl );
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




