#include <stdio.h>
#include<stdlib.h>
#include <string.h>
main()
{
    char *s1="Welcome To Beijing";
    char *s2="loc";
    char *p;
//    system("cls");

    /*Example 1*/
    p=strpbrk(s1,s2);
    if(p)
    { 
        printf("%s\n",p); /*Output "lcome To Beijing"*/
    }
    else
    {
        printf("Not Found!\n");
    }

    /*Example 2*/
    p=strpbrk(s1, "Da");
    if(p)
    {  
        printf("%s",p);
    }
    else
    {
        printf("Not Found!"); /*"Da" is not found*/
    }
    getchar();
    return 0;
}
