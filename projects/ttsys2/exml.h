/**
 * @file exml.h
 * @brief EXML数据处理组件。
 * @author tomszhou
 * @version 1.0
 * @date 2009-3-9
 */

#ifndef _TTSYS_EXML_H_
#define _TTSYS_EXML_H_

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_EXML EXML接口定义
 * @ingroup TTSYS
 * @{
 */


/**
 * @name EXML数据结构定义区
 * @{
 */


/**
 * EXML文本结构定义
 */
typedef struct EXmlText EXmlText;

/**
 * EXML内容结构定义
 */
typedef struct EXmlTextHeader EXmlTextHeader;

/**
 * EXML子项属性定义
 */
typedef struct EXmlAttr EXmlAttr;

/**
 * EXML子项属性定义
 */
struct EXmlAttr
{
  const char *name;        /**< 属性NAME */

  const char *value;       /**< 属性值 */

  struct EXmlAttr *next;   /**< 下一个属性 */
};

/**
 * EXML元素结构定义
 */
struct EXmlElement
{
  const char           *name;              /**< 元素名称 */
  const char           *text;              /**< 元素内容 */
  struct EXmlElement   *parent;            /**< 元素的父节点 */
  struct EXmlElement   *next;              /**< 元素的下一个节点 */
  struct EXmlElement   *first_child;       /**< 元素的第一个子节点 */
  struct EXmlAttr      *attr;              /**< 元素的属性 */
  struct EXmlElement   *last_child;        /**< 元素的最后一个子节点 */
};

/**
 * 判断元素是否含有子节点或数据
 */
#define EXML_ELEM_IS_EMPTY(e) ((e)->first_child == NULL && \
                               (e)->text == NULL )

/**
 * EXML元素结构定义
 */
typedef struct EXmlElement EXmlElement;

/**
 * EXML文档结构定义
 */
typedef struct EXmlDoc EXmlDoc;


/**
 * EXML解析器结构定义
 */
typedef struct EXmlParser EXmlParser;

/**
 * EXML解析器结构定义(带路径)
 */
typedef struct TEXml  TEXml;
/** @} */


/**
 * @name EXmlDoc、EXmlParser函数定义区
 * @{
 */

/** 从解析器获取XML文档。
 *  @param parser 解析器。
 *  @return 成功返回XML文档；失败返回空。
 */
EXmlDoc *EXmlGetDocument(EXmlParser *parser);

/** 创建XML解析器。
 *  @param pool 内存池
 *  @return 成功返回XML解析器；失败返回空。
 */
EXmlParser * EXmlParserCreate(TMemPool *pool);

/** XML解析器解析XML文档。
 *  @param parser 解析器
 *  @param data 需要解析的数据
 *  @param len  数据的大小
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlParserFeed(EXmlParser *parser,const char *data,uint32 len);

/** 完成XML解析器的解析动作。
 *  @param parser 解析器
 *  @param pdoc XML文档
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlParserDone(EXmlParser *parser, EXmlDoc **pdoc);

/** 解析XML文件。
 *  @param filename 文件名称
 *  @param doc XML文档
 *  @param pool 内存池
 *  @param blen BUFFER大小(每次读文件BUFFER的大小)
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlDocParserFile(const char *filename,EXmlDoc **doc,TMemPool *pool,uint32 blen);

/** @} */


/** 将EXML文档转成字符串。
 *  @param pool 内存池
 *  @param doc  EXML文档
 *  @param buffer 返回字符缓冲区的指针
 *  @param size 字符缓冲区的大小
 *  @return  成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlDocToText(TMemPool * pool, EXmlDoc *doc,char **buffer,uint32 *size);

/** 将字符串转化为EXml文档
 *  @param pool  内存池
 *  @param doc   EXML文档
 *  @param buffer 字符缓冲区
 *  @param size 字符缓冲区的大小
 *  @return  成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */

int EXmlDocParseText(TMemPool *pool,EXmlDoc **doc,const char *buffer, uint32 size);


/** 创建 EXML文档
 *  @param pool  内存池
 *  @param name  EXML文档的根结点名称
 *  @param doc  EXML文档
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlDocCreate(TMemPool *pool,const char *name,EXmlDoc **doc);

/** 在EXML文档中增加元素
 *  @param pool 内存池
 *  @param doc EXML文档
 *  @param name 节点名称(目前不支持路径名称(root/server/name))
 *  @param data 属性数据
 *  @param new 新的元素(**new可以NULL)
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlDocAddElement(TMemPool *pool,EXmlDoc *doc,const char *name,const char *data,EXmlElement **new);

/** 根据EXML文档获取根结点
 *  @param doc EXML文档
 *  @return 成功返回Element，失败返回NULL。
 */
EXmlElement *EXmlDocGetRootElement(EXmlDoc *doc);

/** 从EXML文档中根据名称获取XML元素的所有的子节点。
 *  @param name 名称
 *  @param doc XML文档
 *  @return 成功返回Element，失败返回NULL。
 */
EXmlElement *EXmlDocGetNodeLists(const char *name,EXmlDoc *doc);

/** 根据名称获取XML元素的所有的子节点。
 *  @param name 名称
 *  @param elem 该节点下的ALL元素
 *  @return 成功返回Element，失败返回NULL。
 */
EXmlElement *EXmlElementGetChildNode(const char *name,EXmlElement *elem);

/** 获取元素的数据
 *  @param pool 内存池
 *  @param elem 节点
 *  @return 成功返回相对应的数据，失败返回NULL。
 */
char *EXmlElementGetNodeData(TMemPool *pool,EXmlElement *elem);

/** 获取子元素的数据
 *  @param pool 内存池
 *  @param elem 节点
 *  @param name 子节点的名称
 *  @return 成功返回相对应的数据，失败返回NULL。
 */
char *EXmlElementGetChildNodeData(TMemPool *pool,EXmlElement *elem,const char *name);

/** 获取元素的属性值
 *  @param name 属性名称
 *  @param elem 节点
 *  @return 成功返回相对应的数据，失败返回NULL。
 */
const char *EXmlElementGetAttr(const char *name,EXmlElement *elem);

/** 创建新的节点
 *  @param pool 内存池
 *  @param name 节点名称
 *  @param data 节点数据
 *  @return 成功返回相对应的数据，失败返回NULL。
 */
EXmlElement * EXmlElementCreate(TMemPool *pool,const char *name,const char *data);

/** 设置元素属性
 *  @param pool 内存池
 *  @param elem 节点
 *  @param name 节点名称
 *  @param data 属性数据
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlElementSetAttr(TMemPool *pool,EXmlElement *elem,const char *name,const char *data);

/** 增加节点
 *  @param parent 父节点
 *  @param elem 节点
 *  @return 成功返回TTS_SUCCESS，失败返回相对应的错误代码。
 */
int EXmlElemenAddChildNode(EXmlElement *parent,EXmlElement *elem);

/** 获取XML解析器的错误
 *  @param parser XML解析器
 *  @param errbuf Buffer
 *  @param errbufsize Buffer 大小
 *  @return 成功返回错误信息，失败返回NULL。
 */
char * EXmlParserGeterror(EXmlParser *parser,char *errbuf,uint32 errbufsize);


#include <stdio.h>
/** 打印EXML文件
 *  @param level 节点等级
 *  @param elem 节点
 *  @param fp  输出文件
 */
void EXmlElementDebug(int level, const EXmlElement *elem,FILE *fp);


int EXmlMake(const char *RootName,TEXml **exml, TMemPool *MemPool);

int EXmlToText(TEXml *exml, char **buffer,uint32 *size, TMemPool *pool);

int TextToEXml(char *buffer, uint32 size, TEXml **exml, TMemPool *pool);

int EXmlGetValue(TEXml *exml, char *fieldPath, char *fieldName, const char **fieldValue);

int EXmlSetValue(TEXml *exml,char *fieldPath,char *fieldName,char *fieldValue);

int EXmlPrint(TEXml *exml, FILE *fp);

/** @} */

__END_DECLS


#endif




