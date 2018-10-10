#include <unistd.h>
#include <stdio.h>

int main()
{
	uid_t uid = getuid();
	uid_t euid = geteuid();

	printf( "userid is %d , effective user is %d \n", uid, euid );

	
	pid_t pid = getpgid();	
	return 0;
}
