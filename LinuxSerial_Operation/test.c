#include "stdio.h"
#include "stdlib.h"

/*
 *purpose :验证优先级，单目运算符，结合律从右往左....
 *time    :2018.4
 *user    :htarring
 * */
int main()
{
     char *test ,*test1;
     test = malloc(1);
    printf("0x%x\r\n",test);
    *test = 1 ;
    test1 = test;
    printf("%d  %d\r\n",*test,*test1);
    //*test--;
    *test -=1;
    printf("0x%x %d   %d\r\n",test,*test,*test1);
}