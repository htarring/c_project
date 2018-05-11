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
    error_pfile         = -98,   //非法的文件指针
    error_NostandardnBit= -97,   //不是标注数据比特长度(7\8\9)
    error_Nostandardcheck = -96, //不是标准校验方式
    No_error            = 0,      //函数执行成功，无出错
}ErrorType;

#endif // !__MISC_H__
