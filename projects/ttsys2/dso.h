
/**
 * @file dso.h
 * @brief 动态库操作组件,对动态库的操作封装。
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_DSO_H_
#define _TTSYS_DSO_H_

#include "ttdef.h"
#include "memory.h"


__BEGIN_DECLS


/**
 * @defgroup TTSYS_DSO 动态库
 * @ingroup TTSYS
 * @{
 */


/**
 * 动态库句柄定义
 */
typedef struct TDllHandle  TDllHandle;

/**
 * 动态库符号定义
 */
typedef void *             TDllHandleSym;


/** 装载动态库(.so)
 *  @param res_handle 动态库信息
 *  @param path  路径
 *  @param pool 内存池
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int DllLoad(TDllHandle **res_handle,const char *path, TMemPool *pool);

/** 卸载动态库(.so)
 *  @param handle 动态库信息
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int DllUnload(TDllHandle *handle);

/** 根据名称找相对应的对象
 *  @param ressym 存放对象的指针
 *  @param handle  动态库信息
 *  @param symname 对象名称
 *  @return 成功返回TTS_SUCCESS；失败返回相对应的错误代码。
 */
int DllSym(TDllHandleSym *ressym, TDllHandle *handle, const char *symname);

/** 获取动态库的错误信息
 *  @param dll 动态库信息
 *  @param buffer  错误信息
 *  @param buflen 存放错误信息BUFFER的大小
 *  @return 成功返回错误信息；失败返回"No Error"。
 */
const char * DllError(TDllHandle *dll, char *buffer, uint32 buflen);

/** @} */

__END_DECLS



#endif
