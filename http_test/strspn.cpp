// 返回字符串s开头连续包含字符串accept内的字符数目

#include <string.h>
#include <stdio.h>

main()
{
    char *str="Linux was first developed for 386/486-based pcs.";
    printf("%d\n",strspn(str,"Linux"));
    printf("%d\n",strspn(str,"/-"));
    printf("%d\n",strspn(str,"linux"));
    printf("%d\n",strspn(str,"1234567890"));
}
