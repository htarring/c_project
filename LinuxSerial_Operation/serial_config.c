#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "serial_config.h"
#include "misc.h"




/*!
 * macro variable defined
 * explain :声明了俩个串口信息的结构体，并且是全局变量，便于其他模块调用
 * addtional :对外拥有 get method ，只有内部可设
 */
static struct termios *oldtio, *newtio;

/*!
 * addtional : 串口文件指针
 * */
static int *p_file_serial = 0;



/*!
 * function name :串口初始化
 * explain :暂时初始化俩个串口结构体的内存分配
 * addtional:@1:加这么一层封装的意义在于防止出现长度0的内存分配情况
 *           @2:使用内联关键字缩减程序大小
 */
inline int * safe_malloc(const int length)
{
	ErrorType errorNum = No_error;
	if(length<=0)
	 {
		 errorNum = error_malloc_length;
		 return errorNum;
	 }
	return (malloc(length));
}

inline ErrorType Safe_fClose(int *p){

	ErrorType errorNum = No_error;
	if( p == NULL )
	{
		errorNum = error_pfile;
		return errorNum;
	}
	close(*p);
	return errorNum;
}
/*!
 *分配内存
 * */
static int serial_init(void)
{
	ErrorType errorNum = defaulterror;
	newtio = safe_malloc(sizeof(struct termios));
	oldtio = safe_malloc(sizeof(struct termios));
	//p_file_serial = safe_malloc(sizeof (int));
}


/*!
 * function name :串口打开函数
 * input: serial_num name
 * output:open successful or error num
 * addtional: the port own read and write method ,and not block ctty.
 */
int open_serial(char *name,int *const p_file_serial)
{
	*p_file_serial = open(*name, O_RDWR| O_NOCTTY);//O_NDELAY
	if(*p_file_serial < 0)
	{
		perror(*name);
		exit(1);
	}
	printf("open serial: %s sucessful!\n",*name);

	return No_error;
}

int Close_Serial(char *name,int *const p_file_serial){
	
	ErrorType errorNum = No_error;
	errorNum = Safe_fClose(p_file_serial);
	printf("close serial: %s successful ! \n",*name);
	return errorNum;
}


inline static ErrorType Set_DataBit(int nBit){

	ErrorType errornum = No_error;
	
	switch (nBit)
	{
		case 7:
		    newtio.c_cflag |= CS7;  
			break;
		case 8:
		    newtio.c_cflag |= CS8;  
			break;
		case 9 :
		    newtio.c_cflag |= CS9;  
			break;
        // default : 8bit
		default:
		    newtio.c_cflag |= CS8;
			errornum = error_NostandardnBit;
			break;
	}
	return errornum;
}

inline static ErrorType Set_CheckMode(char check){

	ErrorType errornum = No_error;
	
	switch (check)
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
		// default :'N'	
		default:
		    newtio.c_cflag &= ~PARENB;
			errornum = error_Nostandardcheck;
			break;
	}
	return errornum;
}

inline static ErrorType Set_DataSpeed(int nSpeed){

	ErrorType errornum = No_error;

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
		case 57600:
	     	cfsetispeed(&newtio,B57600);  
			cfsetospeed(&newtio,B57600);
			break;
		case 115200:
		    cfsetispeed(&newtio,B115200);  
			cfsetospeed(&newtio,B115200);
			break;
			// default :115200
		default:  
			cfsetispeed(&newtio,B115200);  
			cfsetospeed(&newtio,B115200);  
            		break;
	}
	return errornum;
}

inline static ErrorType Set_StopBit(int nStop){

	ErrorType errornum = No_error;
	switch (nStop)
	{
		case 0:
		    newtio.c_cflag |= CSTOPB; 
			break;
		case 1:
		    newtio.c_cflag &= ~CSTOPB; 
			break;
		case 1_5:
		    //等待翻阅文档.....
			break;
			//默认一个停止位
		default:
            newtio.c_cflag &= ~CSTOPB;
		    break;
	}

	return errornum;
}

/*!
 *function name :
 */
//串口设置函数，check为大写“O,E,N”,nStop为1,0；
int set_serial(int fd, int nSpeed, int nBit, char check, int nStop, struct termios tio1,struct termios tio2)
{
	//struct termios oldtio, newtio;
	ErrorType errornum = No_error;
	if(tcgetattr(fd,&oldtio) != 0)  
    {  
        perror("Get Serial fail:");  
        exit(1);  
    }
	bzero(&newtio, sizeof(newtio));
	
	newtio.c_cflag |= CLOCAL| CREAD;
	newtio.c_cflag &= ~CSIZE;

	errornum = Set_DataBit(nBit);
	errornum = Set_CheckMode(check);
	errornum = Set_DataSpeed(nSpeed);

	//停止位1清除CSTOPB，0是激活
	errornum = Set_StopBit(nStop);

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