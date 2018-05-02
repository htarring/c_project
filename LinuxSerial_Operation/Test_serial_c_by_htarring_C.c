//串口接受端
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

//hong dingyi
//nbit = 8 ; check为大写“O,E,N” ; nStop为1,0；
//#define MODEMDEVICE "/dev/ttyUSB6"
#define SETSPEED 115200
#define SETNBIT 8
#define SETCHECK 'N'
#define SETSTOP 1
#define MIN_WAIT_TIME 1
#define MIN_READ_BIT 100
#define MAXREAD 100
#define LOG_ERROR "/home/pi/log/log_error.txt"
#define SERVER_ID  "0XAAAAAAAAAA"


typedef struct GPS
{
	char id[10];
	char jingdu[20];
	char weidu[20];
	
	
}gps;

typedef struct Temperature
{
	unsigned char MHDR;
	unsigned char DevAddr[6];
	unsigned char Date[10];
}temperature;

typedef struct RH
{
	unsigned char MHDR;
	unsigned char DevAddr[6];
	unsigned char Date[10];
}rh;

typedef struct GAS
{
	unsigned char MHDR;
	unsigned char DevAddr[6];
	unsigned char Date[10];
}gas;

//int  url_code(char *url, char *id, char *jing, char *wei);
int url_code(char *url, char *Server_id, unsigned char *sn, unsigned char *date);
int  url_open(const char *url);
int  dealGpsData(char *s,char *id, char *jing, char *wei);
int  write_log(char *id, char *serial_id, char *jing, char *wei,char type,char *type_sum);
int  write_error_log(char *buffer_serial,char *serial_name);
int  open_serial(char *name);
int  set_serial(int fd, int nSpeed, int nBit, char check, int nStop, struct termios tio1,struct termios tio2);
int  read_serial(int fd, char *buffer_serial, int maxnum);
int  write_log_zj(char *id,char *serial_name);
bool buf_cmp(char std,char *s1);
char MHDR_Selected(char *date);
int Deal_Temperature_Date(char *serial_date,unsigned char *T_Addr,unsigned char *T_Date);
void Deal_Humidity_Date(char *serial_date,unsigned char *H_Date);
void Deal_Gas_logic_Date(char *serial_date,unsigned char *Gas_Date);
int Deal_GAS_Date(char *serial_date,unsigned char *A_Addr,unsigned char *A_Date);

char std_error[]={"30.303003"};
//int check_serial_data(char *buffer_serial);

int main(int argc, char* argv[])
{       
        int  State=0;
	char serial_name[20] = {"/dev/ttyUSB0"};
	if(argc > 1)
	{
		memset(serial_name,0,sizeof(serial_name));
		int x = sprintf(serial_name,"/dev/ttyUSB%s",argv[1]);
		serial_name[x] = '\0';
	}
	
	int fd_serial;
	int flag_error;
	struct termios oldtio, newtio;
	
	unsigned char  buffer_serial[150];
	char  buffer_url[100];
	//char  test_1=0x0A;
	int   test_i=0;
	//char  test_2=0x01;
	printf("start...\n");
	//Serial init
	printf("open Serial...\n");
	fd_serial = open_serial(serial_name);
	printf("next set Serial...\n");
	set_serial(fd_serial, SETSPEED, SETNBIT, SETCHECK, SETSTOP,oldtio,newtio);

	flag_error = 0;

	temperature Tempera;
	rh          Humidity;
	gas         GAS_Logic;
	gas         Fire;

	Tempera.MHDR = 0X28;
	Humidity.MHDR = 0X48;
	GAS_Logic.MHDR = 0X68;

	while(1)
	{
		memset(buffer_serial,0,sizeof(buffer_serial));
		int m = read_serial(fd_serial, buffer_serial, MAXREAD);	
		//serial error!
			
		if(m <= 0)
		{
			flag_error++;
			if(flag_error >= 10)
			{
				printf("serial lost connection!!!\n");
				break;
			}
			printf("read serial error %d!!!\n",flag_error);
			continue;
		}
		//read error!

	   //if(buffer_serial[0]==00)
	   {
		 /*  struct tm *timeptr;
		   time_t now_time;
		   time(&now_time);
		   timeptr = localtime(&now_time);
		   //int a = 0;

		   char buffer_time[50];
		   char buffer_log[200];
		   memset(buffer_log, 0, sizeof(buffer_log));//清0
		   memset(buffer_time, 0, sizeof(buffer_time));//清0
		   strcpy(buffer_time, asctime(timeptr));*/
		   //buffer_time[30]='\n';
		//int n = sprintf(buffer_log, "%s\n[0X%s]\n", buffer_time,buffer_serial);//将相关信息写入数组
		//write_log_zj(buffer_serial,serial_name);
		char file_name[50];
		//sprintf(file_name,"/home/pi/log/log_error_rate%d.txt",buffer_serial[0]);
		sprintf(file_name,"/home/pi/log/log_error_IWDG%d.txt",1);
		FILE *fp=fopen(file_name,"a+");
		int NUM_I=0;
		//fwrite(buffer_serial, m, 1, fp);
		//fprintf(fp,"\n");
		//fprintf(fp,"%c\r\n",buffer_serial[NUM_I]);
		for(NUM_I=0;NUM_I<m;NUM_I++)
		{
	        fprintf(fp,"%c",buffer_serial[NUM_I]);
           // fwrite(buffer_serial, 1, 19, fp);
		    //printf("%c\r\n",buffer_serial[NUM_I]);
			if(buffer_serial[NUM_I+1] == 'i'  || buffer_serial[NUM_I+1] == 'o')
			  fprintf(fp,"\n");
		  
		}
		
		
		//fprintf(fp,"\n");
		fclose(fp);
	   }//记录中继开始时间

		//data check
		//check_serial_data(buffer_serial);
	
		//deal with data 
		gps g;
		//memset(g,0,sizeof(g));
		/*if (MHDR_Selected(buffer_serial) == 'T')
		{
			State=Deal_Temperature_Date(buffer_serial, Tempera.DevAddr, Tempera.Date);
			url_code(buffer_url, SERVER_ID,Tempera.DevAddr,Tempera.Date);
		}
		else if(MHDR_Selected(buffer_serial)=='A')
		{
			State=Deal_GAS_Date(buffer_serial,GAS_Logic.DevAddr,GAS_Logic.Date);
			url_code(buffer_url, SERVER_ID,GAS_Logic.DevAddr,GAS_Logic.Date);
		}
		else if(MHDR_Selected(buffer_serial)=='F')
		{
		 	State=Deal_GAS_Date(buffer_serial,Fire.DevAddr,Fire.Date);
			url_code(buffer_url, SERVER_ID,Fire.DevAddr,Fire.Date);
		}*/
         if(State>=1)
		{
			
			//判断是CC还是3C，执行的时候直接写入对应的标号
		  //  dealGpsData(buffer_serial,g.id, g.jingdu, g.weidu);
		//url code
		    memset(buffer_url,0,sizeof(buffer_url));
		   //if(buf_cmp(test_1,buffer_serial))		
		   // url_code(buffer_url, SERVER_ID,Tempera.DevAddr,Tempera.Date);
		//url open
		//url_open(buffer_url);
		//write log

		if(State==1)
                {
	    //	write_log(Tempera.MHDR, serial_name, Tempera.DevAddr, Tempera.Date,'3', buffer_serial);
		}
		else if(State==2)
		{
		//write_log(g.id, serial_name, g.jingdu, g.weidu,'C', buffer_serial);
		}
		//State=0;
		for(test_i=0;test_i<sizeof(buffer_serial)-1;test_i++)
		printf("%d ",(int)buffer_serial[test_i]);
		for(State=0;State<sizeof(buffer_serial)-1;State++)
		  buffer_serial[State]=0;
		State=0;
		}
	}
	printf("close serial...\n");
	close(fd_serial);
	printf("close serial successful!!!\n");
	tcsetattr(fd_serial, TCSANOW, &oldtio);
	return 0;
}

/*
int check_serial_data(char *buffer_serial)
{
	
}
*/

char  MHDR_Selected(char *date)//接受串口过来的数据判断数据类型
{
	switch (date[0])
	{
	case 0x28: return  'T';break;
	case 0x88: return 'F';break;
	case 0x68:return 'A';break;
	default:return 'E';
		break;
	}
}

int Deal_Temperature_Date(char *serial_date, unsigned char *T_Addr, unsigned char *T_Date)//
{
	int i = 0;
	//for ( i = 1;i < 6;i++)
	//	T_Addr[i - 1] = serial_date[i];//记录devaddr
	//T_Addr[i] = '\0';
	//i=sprintf(T_Addr,"%c%c%c%c%c",serial_date[1],serial_date[2],serial_date[3],serial_date[4],serial_date[5]);
	//T_Addr[i]='\0';
	char temp3=serial_date[1]+0x30;
	char temp4=serial_date[2]+0x30;
	char temp5=serial_date[3]+0x30;
	char temp6=serial_date[4]+0x30;
	char temp7=serial_date[5]+0x30;
	i=sprintf(T_Addr,"%c%c%c%c%c",temp3,temp4,temp5,temp6,temp7);
	T_Addr[i]='\0';
	
	int temp_1 = (int)serial_date[6];
	int temp_2 = (int)serial_date[7];
	int temp_3 = (int)serial_date[8];
	int temp_4 = (int)serial_date[9];
	int end=sprintf(T_Date, "%2d.%02d-%2d.%02d", temp_1, temp_2,temp_3,temp_4);
	T_Date[end] = '\0';

	return 1;
}

int Deal_GAS_Date(char *serial_date,unsigned char *A_Addr,unsigned char *A_Date)
{
	char temp1=serial_date[1]+0x30;
	char temp2=serial_date[2]+0x30;
	char temp3=serial_date[3]+0x30;
	char temp4=serial_date[4]+0x30;
	char temp5=serial_date[5]+0x30;
	int i=sprintf(A_Addr,"%c%c%c%c%c",temp1,temp2,temp3,temp4,temp5);
	A_Addr[i]='\0';
	
	int temp_1=(int)serial_date[6];
	int end=sprintf(A_Date,"%1d",temp_1);
	A_Date[end]='\0';
}

int write_log(char *id, char *serial_name, char *jing, char *wei,char type,char *type_sum)
{
	struct tm *timeptr;
 	time_t lt;  
 	time(&lt);  
 	timeptr=localtime(&lt);
	//int a = 0;

 	char buffer_time[50];
 	char buffer_log[150];
 	//char jingdu[10] = "120";
 	//char weidu[10] = "30";
	memset(buffer_log, 0, sizeof(buffer_log));//清0
	memset(buffer_time,0,sizeof(buffer_time));//清零
 	strcpy(buffer_time,asctime(timeptr));
 	
 	int n = sprintf(buffer_log, "%s[ type=%c ZJ_Num=%d  USB id=%s GPS id=%s jingdu=%s weidu=%s ]\n", buffer_time, type,(int)(*(type_sum+10)),serial_name, id, jing, wei);//将相关信息写入数组
 	buffer_log[n] = '\0';
	printf("\nLog length =%d\n", n); 
	
	//write log file
	char file_name[50];
	int m = sprintf(file_name,"/home/pi/log/log_temp%s.txt",id);
	file_name[m] = '\0';
	FILE *fp = fopen(file_name,"a+");
 	if(NULL == fp) 
 	{ 
    		printf("File:\t %sCan Not Open To Write\n",file_name); 
    		exit(1); 
 	}
 	//int length = sizeof(bufftime);
 	int l = fwrite(buffer_log, 1, n, fp);
 	printf("Log File:\t %s \nWrite......\n%swrite %ddata Successful!!!\n\n", file_name, buffer_log, l);
 	fclose(fp); 
 
  	return 0;
}

int write_log_zj(char *id,char *serial_name)
{
	struct tm *timeptr;
 	time_t lt;  
 	time(&lt);  
 	timeptr=localtime(&lt);
	//int a = 0;

 	char buffer_time[50];
 	char buffer_log[100];
 	//char jingdu[10] = "120";
 	//char weidu[10] = "30";
	memset(buffer_log, 0, sizeof(buffer_log));
	memset(buffer_time,0,sizeof(buffer_time));
 	strcpy(buffer_time,asctime(timeptr));
	  
    int n = sprintf(buffer_log, "%s\n  ", buffer_time);
 	buffer_log[n] = '\0';
	printf("\nLog length =%d\n", n); 
	
	//write log file
	char file_name[50];
	int m = sprintf(file_name,"/home/pi/log/log_temp%d.txt",(int)(*(id+1)));
	file_name[m] = '\0';
	FILE *fp = fopen(file_name,"a+");
 	if(NULL == fp) 
 	{ 
    		printf("File:\t %sCan Not Open To Write\n",file_name); 
    		exit(1); 
 	}
 	//int length = sizeof(bufftime);
 	int l = fwrite(buffer_log, 1, n, fp);
//	int j = fprintf(fp,"0x%x\n",id);
//	printf("Log File:\t %s \nWrite......\n%swrite %ddata Successful!!!\n\n", file_name, buffer_log, l+j);
 	fclose(fp); 
 
  	return 0;


 

}

int write_error_log(char *buffer_serial,char *serial_name)
{
	struct tm *timeptr;
 	time_t lt;  
 	time(&lt);  
 	timeptr=localtime(&lt);

 	char buffer_time[50];
 	char buffer_log[100];
	memset(buffer_log, 0, sizeof(buffer_log));
	memset(buffer_time,0,sizeof(buffer_time));
 	strcpy(buffer_time,asctime(timeptr));
 	
 	int n = sprintf(buffer_log, "%s[ USB id=%s error data :%s ]\n", buffer_time, serial_name, buffer_serial);
 	buffer_log[n] = '\0';
	printf("\nLog length =%d\n", n); 
	/*
	//write log file
	FILE *fp = fopen(LOG_ERROR,"a");
	*/
	char file_name[100];
	int m  = sprintf(file_name,"/home/pi/log%s.txt",serial_name);
	file_name[m] = '\0';
	FILE *fp = fopen(file_name,"a");
 	if(NULL == fp) 
 	{ 
    		printf("File:\t %sCan Not Open To Write\n",file_name); 
    		exit(1); 
 	}
 	//int length = sizeof(bufftime);
 	int l = fwrite(buffer_log, 1, n, fp);
 	printf("Log File:\t %s \nWrite......\n%swrite %ddata Successful!!!\n\n", file_name, buffer_log,l);
 	fclose(fp); 
 
  	return 0;
}

int read_serial(int fd, char *buffer_serial, int maxnum)
{
	printf("reading serial data...\n");
	int m = read(fd, buffer_serial, maxnum);
	buffer_serial[m] = '\0';
	
	printf("serial data res=%d\nvuf=%s\n", m, buffer_serial);
	
	return m;
}

//lat = jingdu, lng = weidu
int url_code(char *url, char *Server_id, unsigned char *sn, unsigned char *date)
{
	memset(url,0,sizeof(url));
	printf("start url code...\n");
	int m = sprintf(url, "curl http://smarthome.lpwa.cloud/wsn?gate=%s\\&sn=%s\\&data=%s",SERVER_ID,sn,date);
	printf("url = %s\n",url);

	printf("url open....\n");
	system(url);

	return m;
}
//no function
int url_open(const char *url)
{
	printf("url open....\n");
	int m = system(url);
	return m;
}

int dealGpsData(char *s, char *id, char *jing, char *wei)
{
	printf("gps deal data start....\n");
	//gps g1;
	//memset(g1.id,0,sizeof(g1.id));
	//memset(g1.weidu,0,sizeof(g1.weidu));
	//memset(g1.jingdu,0,sizeof(g1.jingdu));
	//strncpy(g1.id,s,9);
	//char *ptr1 = strchr(s,',');
	//char *ptr2 = strchr(ptr1+1,',');
	//char *ptr3 = strrchr(s,',');

	int c1 = (int)s[1];
	//b save weidu
	int b1 = (int)s[2];
	int b2 = (int)s[3]*10000;
	int b3 = (int)s[4]*100;
	int b4 = (int)s[5];
	int b5 = b2+b3+b4;
	//a save jingdu
	int a1 = (int)s[6];
	int a2 = (int)s[7]*10000;
	int a3 = (int)s[8]*100;
	int a4 = (int)s[9];
	int a5 = a2+a3+a4;

	int l = sprintf(id, "%d",c1);
	id[l] = '\0';
	printf("id=%s!!!\n",id);
	
	int m = 0;
	if(a5<100000)
		m = sprintf(jing, "%d.0%d",a1,a5);
	else
		m = sprintf(jing, "%d.%d",a1,a5);
	jing[m] = '\0';
	printf("jing=%s, a1=%d,a5=%d!!!\n",jing,a1,a5);
	

	int n = 0;
	if(b5<100000)
		n = sprintf(wei, "%d.0%d",b1,b5);
	else
		n = sprintf(wei, "%d.%d",b1,b5);
	wei[n] = '\0';
	printf("wei=%s, b1=%d,b5=%d!!!\n",wei,b1,b5);
	
	return 0;	
}

//串口打开函数
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
		case 115200:
			cfsetispeed(&newtio,B115200);  
			cfsetospeed(&newtio,B115200);  
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

bool buf_cmp(char std,char *s1)
{
        char  std_2=std-0x09;//0x01
	int   num_1=0;
	int   num_2=0;
	bool  state=false;
	for(num_1=2;num_1<10;num_1++)
	{
 	 if(num_1==5||num_1==9)
	 {
	  if(s1[num_1]==std_2)
	   num_2++;
	 }
	 if(s1[num_1]==std)
	  num_2++;
	}
	if(num_2>5||s1[2]==std||s1[6]==std||s1[2]>37||s1[6]>126||s1[2]<20||s1[6]<20)
	state=false;
	else
	state=true;

	return state;

}
