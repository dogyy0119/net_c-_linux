#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

bool daemonize()
{
	/* 创建子进程，关闭父进程，这样可以使程序再后台运行 */
	pid_t pid = fork();
	if ( pid < 0 )
	{
		return false;
	}
	else if ( pid > 0 )
	{
		exit(0);
	}
	/* 设置文件权限掩码，当进程创建新文件（使用 open() 系统调用）时，文件权限将是 mode & 0777 */
	umask( 0 );
	/* 创建新的会话，设置本进程为进程组的首领 */
	pid_t sid = setsid();
	if ( sid < 0 )
	{
		return false;
	}
	
	/* 切换工作目录 */
	if ( ( chdir ( "/" ) ) )
	{
		return false;
	}
	
	/* 关闭标准输入设备，标准输出设备和标准错误输出设备 */
	close( STDIN_FILENO );
	close( STDOUT_FILENO );
	close( STDERR_FILENO );

	/* 关闭其他已经打开的文件描述符，代码省略*/
	/* 将标准输入、输出和标准版错误输出定向到 /dev/null 文件*/
	open( "/dev/null", O_RDONLY );
	open( "/dev/null", O_RDWR );
	open( "/dev/null", O_RDWR );
	
	return true;
}

static bool switch_to_user( uid_t user_id, gid_t gp_id)
{
	/* 先确保输入用户不是root */
	if ( ( user_id == 0 ) && ( gp_id == 0 ) )
	{
		return false;
	}

	/* 先确保当前用户是合法用户： root 或者 目标用户 */
	gid_t gid = getgid();
	uid_t uid = getuid();
	if ( ( ( gid != 0 ) || ( uid != 0 ) ) && ( ( gid != gp_id ) || ( uid != user_id ) ) )
	{
		return false;
	}

	/* 如果部署root, 则已经是目标用户 */
	if ( uid != 0 )
	{
		return true;
	}

	/* 切换到目标用户 */
	if ( ( setgid( gp_id) < 0 ) || ( setuid( user_id ) < 0 ) )
	{
		return false;
	}

	return true;
}

int main()
{
	uid_t uid = getuid();
	uid_t euid = geteuid();

	printf( "userid is %d , effective user is %d \n", uid, euid );	

	bool ret = switch_to_user( 1000, 1000 );
	printf( "转换是否成功 ：%d \n", ret );

	ret = daemonize();
	printf( "设置是否成功 ：%d \n", ret );

	cout << "safas" <<endl;
	return 0;
}
