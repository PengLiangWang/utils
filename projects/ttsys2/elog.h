
/**
 * @file elog.h
 * @brief  日志处理模块(支持远程)
 * @author tomszhou
 * @version 1.0
 * @date 2011-5-4
 */


#ifndef _TTSYS_ELOG_H_
#define _TTSYS_ELOG_H_

__BEGIN_DECLS

/**
 * @defgroup TTSYS_ELOG 日志处理模块(支持远程)
 * @ingroup TTSYS
 * @{
 */

/**
 * 日志常量定义
 */
enum
{
   LOG_LEVEL_ERROR   =   0x00,     /**< ERROR */
   LOG_LEVEL_WARN    =   0x01,     /**< WRAN  */
   LOG_LEVEL_INFO    =   0x02,     /**< INFO  */
   LOG_LEVEL_DEBUG   =   0x03,     /**< DEBUG */
   LOG_MASK_TAG      =   0x01,     //TAG
   LOG_FILE_MASK     =   0x01,     /**< file方式 */
   LOG_HOST_MASK     =   0x02      /**< server方式 */
};

/** 日志初始化
 *  @param pri     日志等级
 *  @param module  模块
 *  @param path    路径
 *  @param file    日志文件
 *  @param lmask   日志记录方式
 *  @param host    server (129.0.1.1:1234)
 *  @return 
 */
void eloginit(int pri,char *module,char *path,char *file,int lmask,char *host);

/** 设置日志等级
 *  @param pri     日志等级
 *  @return
 */
void eloglevel(int pri);

/** 设置日志记录方式
 *  @param mask   日志记录方式
 *  @return 
 */
void elogmask(int mask);

/** 获取是否是调试状态
 *  @return 1: 是 0:不是
 */
int elogisdebug();

/** 记录日志
 *  @param pri  日志等级
 *  @param function 函数(__FUNCITON__)
 *  @param file 程序所在的文件(__FILE__)
 *  @param line 文件的行数(__LINE__)
 *  @param fmt  格式
 *  @return
 */
void elog(int pri,const char *function, const char *file,const int line,char *fmt,...);


FILE *eloggetfilebyname(char *filename);
__END_DECLS


/**
 * @name 日志常用函数宏定义
 * @{
 */
#define PREFIXLOG(level)  LOG_LEVEL_##level,__FUNCTION__,__FILE__,__LINE__

#define ELOG(level, ...)  elog(PREFIXLOG(level), __VA_ARGS__)

#define ELOG_DEBUG()    elogisdebug()

/** @} */

/** @} */

#endif

