#ifndef __MISC_H__
#define __MISC_H__

/*!
 * file state:部分公共参数定义
 */
/*!
 * 
 */

/*!
 * error num 说明
 */
typedef  enum ErrorType{
    defaulterror        = -100,  //函数初始化时出错代码
    error_malloc_length = -99,   //内存初始化操作传入长度非正整数
    No_error            = 0,      //函数执行成功，无出错
}ErrorType;

#endif // !__MISC_H__
