#ifndef __SERIAL_CONFIG_H__
#define __SERIAL_CONFIG_H__


/*
  serial parameters
*/
// Version : 0.1   confirm it worked, then rebuilt again  ,now time is 2018.3.21 14:43
#define SETSPEED       9600   //band rate
#define SETNBIT        8      //bit length
#define SETCHECK      'N'     //cheak mode
#define SETSTOP        1      //stop bit
#define MIN_WAIT_TIME  1      //ms? not confirmed
#define MIN_READ_BIT   50     //byte not confirmed
#define MAXREAD        50     //byte not confirmed

/*!
 * macro variable defined
 * explain :声明了俩个串口信息的结构体，并且是全局变量，便于其他模块调用
 */
struct termios *oldtio, *newtio;



#endif