
/**
 * @file file.h
 * @brief  文件处理组件
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_FILE_H_
#define _TTSYS_FILE_H_
#include <fcntl.h>

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_FILE 文件处理组件
 * @ingroup TTSYS
 * @{
 */

/**
 * 文件数据存储结构定义
 */
typedef struct TFile  TFile;


/**
 * 文件标识定义
 */
enum
{
  TFILE_CLEANUP    = 0x00800,        /**< 关闭文件时删除文件 */
  TFILE_NOTCLEANUP = 0               /**< 关闭文件时不删除文件 */
};

/** 打开文件
 *  @param file 文件数据存储区
 *  @param name 文件名
 *  @param flag 打开文件的标识
 *  @param pool 所用的内存池
 *  @return 成功返回TTS_SUCCESS。
 */
int FileOpen(TFile **file,const char *name, int flag,TMemPool *pool);

/** 创建并打开临时文件
 *  @param file 文件数据存储区
 *  @param base 文件名的前缀
 *  @param path 文件存放路径
 *  @param flag 打开文件的标识
 *  @param pool 所用的内存池
 *  @return 成功返回TTS_SUCCESS。
 */
int TmpFileOpen(TFile **file,const char *base,const char *path, int flag,TMemPool *pool);

/** 关闭文件
 *  @param file 文件数据存储区
 *  @return 成功返回TTS_SUCCESS。
 */
int FileClose(TFile *file);

/** 删除文件
 *  @param path 文件路径
 *  @return 成功返回TTS_SUCCESS。
 */
int FileRemove(const char *path);

/** 文件重新命名
 *  @param from_path 原文件名
 *  @param to_path 新文件名
 *  @return 成功返回TTS_SUCCESS。
 */
int FileRename(const char *from_path, const char *to_path);

/** 读取文件
 *  @param thefile 文件数据存储区
 *  @param buf 数据存储区
 *  @param nbytes 需要读取的数据的大小(函数返回时存放读取的数据大小)
 *  @return 成功返回TTS_SUCCESS。
 */
int FileRead(TFile *thefile, void *buf, uint32 *nbytes);

/** 写入文件
 *  @param thefile 文件数据存储区
 *  @param buf 数据存储区
 *  @param nbytes 需要写入的数据的大小(函数返回时存放写入的数据大小)
 *  @return 成功返回TTS_SUCCESS。
 */
int FileWrite(TFile *thefile, const void *buf, uint32 *nbytes);

/** 写入文件(根据BUF的大小写入文件)
 *  @param thefile 文件数据存储区
 *  @param buf 数据存储区
 *  @return 成功返回TTS_SUCCESS。
 */
int FileWriteEx(TFile *thefile, const char *buf);


/** @} */

__END_DECLS

#endif
