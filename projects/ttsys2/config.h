
/**
 * @file config.h
 * @brief  配置文件处理组件
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-9
 */


#ifndef _TTSYS_CONFIG_H_
#define _TTSYS_CONFIG_H_

#include "ttdef.h"
#include "memory.h"
#include "hlist.h"

__BEGIN_DECLS



/**
 * @defgroup TTSYS_CONFIG 配置文件处理组件
 * @ingroup TTSYS
 * @{
 */

/**
 * 配置文件数据存储结构定义
 */
typedef struct TConfig TConfig;

/**
 * 属性项定义
 */
typedef struct TProperty
{
  char   *key;    /**< 键 */
  char   *value;  /**< 值 */
}TProperty;


/**
 *  函数宏定义
 */
#define   ConfigFreeData      ConfigFree

/** 装载配置文件
 *  @param filename 配置文件名称
 *  @param config  配置文件数据存储区
 *  @return 成功返回TTS_SUCCESS。
 */
int  ConfigLoad(const char *filename,TConfig **config);

/** 关闭配置文件
 *  @param config  配置文件数据存储区
 *  @return 成功返回TTS_SUCCESS。
 */
int  ConfigClose(TConfig **config);

/** 从配置文件中获取属性值
 *  @param config 配置文件数据存储区
 *  @param section  所属组
 *  @param key 键
 *  @param data 值
 *  @return 成功返回TTS_SUCCESS，返回TTS_ENULL表示没有此键。
 */
int  ConfigGetProperty(TConfig *config,char *section,char *key,char **data);

/** 释放存储区的数据
 *  @param data 值
 *  @return 成功返回TTS_SUCCESS，返回TTS_ENULL表示没有此键。
 */
int  ConfigFree(char **data);

/** 从配置文件中获取Section列表
 *  @param config 配置文件数据存储区
 *  @param sections  所Section列表
 *  @param nums     Seciont的个数
 *  @return 成功返回TTS_SUCCESS，返回TTS_NOTFOUND表示没有此键。
 */

int  ConfigGetSections(TConfig *config,char ***sections,int *nums);

/** 释放存储区的数据
 *  @param sections 值
 *  @return 成功返回TTS_SUCCESS，返回TTS_ENULL表示没有此键。
 */
int  ConfigFreeSections(char ***sections);

/** 从配置文件中根据Section获取KEY列表
 *  @param config 配置文件数据存储区
 *  @param section section名称
 *  @param pros  Property
 *  @param nums  Property的个数
 *  @return 成功返回TTS_SUCCESS，返回TTS_ENULL表示没有此键。
 */
int  ConfigGetPropertys(TConfig *config,char *section,TProperty **pros,int *nums);

/** 释放存储区的数据
 *  @param pros 值
 *  @return 成功返回TTS_SUCCESS，返回TTS_ENULL表示没有此键。
 */
int  ConfigFreePropertys(TProperty **pros);


/** 从配置文件中获取属性子项
 *  @param config 配置文件数据存储区
 *  @param section  所属组
 *  @param key 键
 *  @param subkey 子键
 *  @param data 数据存储区
 *  @return 成功返回TTS_SUCCESS，返回TTS_NOTFOUND表示没有此键。
 */
int  ConfigGetSubProperty(TConfig *config,char *section,char *key,char *subkey,char **data);

/** @} */

__END_DECLS

#endif
