#include <sys/types.h>//对于此程序而言此头文件types.h用不到
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 
int main( int argc, char *argv[] )/*整数类型主函数*/
{
	pid_t pid = fork();/*传递参数*/
	if( pid < 0 )/*如果(进程标记<0)*/
	{
		fprintf( stderr, "错误！\n" );
	}
	else if( pid == 0 )/*否则如果(进程标记==0)*/
	{
		printf( "百度百科：这是子进程！\n " );
		exit( 0 );
	}
	else/*否则*/{
		printf( "百度百科：这是父进程！子进程的进程标记为=%d \n ", pid );
	}
	//可能需要时候wait或waitpid函数等待子进程的结束并获取结束状态
	exit( 0 );
}
