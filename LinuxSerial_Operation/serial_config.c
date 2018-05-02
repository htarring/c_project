#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "serial_config.h"
#include "misc.h"



/*!
 * function name :串口初始化
 * explain :暂时初始化俩个串口结构体的内存分配
 * 
 */
int * safe_malloc(int  const length)
{
	ErrorType errorNum = defaulterror;
	if(length<=0)
	 {
		 errorNum = error_malloc_length；
		 return 
	 }
	return (malloc());
}
int serial_init(void)
{
	ErrorType errorNum = defaulterror;
	newtio = malloc(sizeof(termios));
	
}


/*!
 * function name :串口打开函数
 * input: serial_num name
 * output:open successful or error num
 */
int open_serial(char *name)
{
	int fd;
	fd = open(name, O_RDWR| O_NOCTTY);//O_NDELAY
	if(fd < 0)
	{
		perror(name);
		exit(1);
	}
	printf("open serial: %s sucessful!\n",name);
	return fd;
}

/*!
 *function name :
 */
//串口设置函数，check为大写“O,E,N”,nStop为1,0；
int set_serial(int fd, int nSpeed, int nBit, char check, int nStop, struct termios tio1,struct termios tio2)
{
	struct termios oldtio, newtio;
	
	if(tcgetattr(fd,&oldtio) != 0)  
    {  
        perror("Get Serial fail:");  
        exit(1);  
    }
	bzero(&newtio, sizeof(newtio));
	
	newtio.c_cflag |= CLOCAL| CREAD;

	newtio.c_cflag &= ~CSIZE;
	switch(nBit)  //传输多少数据位
    {  
        case 7:  
			newtio.c_cflag |= CS7;  
			break;  
        case 8:  
			newtio.c_cflag |= CS8;  
			break;      
    }
	
	switch(check)  //大写字符
    {  
        case 'O': //奇校验 
           	newtio.c_cflag |= PARENB;  
       		newtio.c_cflag |= PARODD;  
       		newtio.c_iflag |= (INPCK| ISTRIP);  
            break;  
        case 'E': //偶校验
        	newtio.c_iflag |= (INPCK| ISTRIP);  
        	newtio.c_cflag |= PARENB;  
        	newtio.c_cflag &= ~PARODD;  
       		break;  
    	case 'N': //不进行 
        	newtio.c_cflag &= ~PARENB;  
        	break;  
    }
	
	switch(nSpeed)  //传输速度
    {  
        case 2400:  
			cfsetispeed(&newtio,B2400);  
			cfsetospeed(&newtio,B2400);  
			break;  
   	case 9600:  
			cfsetispeed(&newtio,B9600);  
			cfsetospeed(&newtio,B9600);  
			break;  
    	case 38400:  
			cfsetispeed(&newtio,B38400);  
			cfsetospeed(&newtio,B38400);  
			break;
	default:  
			cfsetispeed(&newtio,B9600);  
			cfsetospeed(&newtio,B9600);  
            		break;
	}
	
	//停止位1清除CSTOPB，0是激活
	if(nStop == 1){  
        newtio.c_cflag &= ~CSTOPB;  
    }  
    else if(nStop == 0){  
        newtio.c_cflag |= CSTOPB;  
    }
    
    //change output format
    newtio.c_iflag &= ~(ICANON| ECHO| ECHOE| ISIG);//original data input
    newtio.c_oflag &= ~OPOST;//original data output
	
    newtio.c_cc[VTIME] = MIN_WAIT_TIME; //读取字符的MIN等待时间 
    newtio.c_cc[VMIN] = MIN_READ_BIT;  //最少读取的字符? max read?
    

    //if bits to many, recive but not read!!! 
    tcflush(fd,TCIFLUSH);
	
    if((tcsetattr(fd,TCSANOW,&newtio)) != 0)  
    {  
        perror("Serial set error!:");  
        exit(1);  
    }  
    printf("Serial set done!\n");  
    
    tio1 = oldtio;
    tio2 = newtio;

    return 0;
}