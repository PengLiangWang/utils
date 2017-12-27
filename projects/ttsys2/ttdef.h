
/**
 * @file ttdef.h
 * @brief TUOTUO 库基本类型定义
 * @author tuotuo
 * @version 1.0
 * @date 2008-3-1
 */

#ifndef _TTSYS_DEF_H_
#define _TTSYS_DEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/**
 * @defgroup TTSYS_DEF 基本类型及常量定义
 * @brief 基本类型及常量定义
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-1
 * @ingroup TTSYS
 * @{
 */

/**
 * @name 类型定义
 * @{
 */


/**
 *  C++宏定义
 */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#else
#define __BEGIN_DECLS
#endif

/**
 *  C++宏定义
 */
#ifdef __cplusplus
#define ___END_DECLS  }
#else
#define __END_DECLS
#endif

#include <errno.h>

__BEGIN_DECLS

/**
  * 无符号整形定义
  */
typedef unsigned int    uint32;

/**
  * 无符号字符定义
  */
typedef unsigned char   uchar;

/**
  * 无符号长整型定义
  */
typedef unsigned long   ulong32;


/** @} */

/**
 * @name 错误代码定义
 * @{
 */

enum  TTSYS_ERRNO
{
   TTS_MAX_SYS_ERRNO       = 130,       /**< 系统最大错误NO */
   TTS_SUCCESS             = 0,         /**< 成功SUCCESS*/
   TTS_OS_START_ERROR      = 60000,     /**< errno define start */
   TTS_ENOMEM              = 60001,     /**< 内存空间不足 */
   TTS_EGENERAL            = 60002,     /**< 存在故障，具体信息不详 */
   TTS_EINVAL              = 60003,     /**< 无效值*/
   TTS_EACCES              = 60004,     /**< 没有权限*/
   TTS_TIMEOUT             = 60005,     /**< 操作超时*/
   TTS_NOTFOUND            = 60006,     /**< "未找到目标对象" */
   TTS_EXIST               = 60007,     /**< 对象已经存在 */
   TTS_ENONE               = 60008,     /**< 对象不存在 */
   TTS_DLOPEN              = 60009,     /**< 打不开动态库(dlopen) */
   TTS_ENULL               = 60010,     /**< 对象为空 */
   TTS_EOF                 = 60011,     /**< 文件结束 */
   TTS_DATEBASE            = 60012,     /**< 数据库失败 */
   TTS_USER_START_ERROR    = 65000      /**< 用户自定义错误 */
};

/** @} */


/**
 * @name 数据类型定义
 * @{
 */

enum  TTSYS_STRING_TYPE
{
   STRING_TYPE_ASC         = 'A',       /**< ASCII码 */
   STRING_TYPE_LBCD        = 'L',       /**< 左BCD码 */
   STRING_TYPE_RBCD        = 'R',       /**< 右BCD码 */
   STRING_TYPE_HEX         = 'H',        /**< HEX码   */
   STRING_TYPE_ETLBCD      = 'X'        /**< HEX码   */
};

/** @} */

/**
  * 定义类型 格式化输出结构
  */
typedef struct  
{
  char *curpos;    /**< 当前指针位置*/
  char *endpos;    /**< 尾指针位置*/
}mvformatter_buff_t;

/**
 * @name 常用字符操作函数宏定义
 * @{
 */
#define m_isalnum(c) (isalnum(((unsigned char)(c))))       /**< 是否是...*/
#define m_isalpha(c) (isalpha(((unsigned char)(c))))       /**< 是否是...*/
#define m_iscntrl(c) (iscntrl(((unsigned char)(c))))       /**< 是否是...*/
#define m_isdigit(c) (isdigit(((unsigned char)(c))))       /**< 是否是数字(0-9)*/
#define m_isgraph(c) (isgraph(((unsigned char)(c))))       /**< 是否是...*/
#define m_islower(c) (islower(((unsigned char)(c))))       /**< 是否是...*/

/** @} */

/** GetStrError: 获取错误信息
 *  @param err 错误代码
 *  @return 成功返回错误信息。
 */
char *GetStrError(int err);

__END_DECLS


/** @} */


#endif
