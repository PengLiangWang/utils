/**
 * @file exml.h
 * @brief EXML���ݴ��������
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
 * @defgroup TTSYS_EXML EXML�ӿڶ���
 * @ingroup TTSYS
 * @{
 */


/**
 * @name EXML���ݽṹ������
 * @{
 */


/**
 * EXML�ı��ṹ����
 */
typedef struct EXmlText EXmlText;

/**
 * EXML���ݽṹ����
 */
typedef struct EXmlTextHeader EXmlTextHeader;

/**
 * EXML�������Զ���
 */
typedef struct EXmlAttr EXmlAttr;

/**
 * EXML�������Զ���
 */
struct EXmlAttr
{
  const char *name;        /**< ����NAME */

  const char *value;       /**< ����ֵ */

  struct EXmlAttr *next;   /**< ��һ������ */
};

/**
 * EXMLԪ�ؽṹ����
 */
struct EXmlElement
{
  const char           *name;              /**< Ԫ������ */
  const char           *text;              /**< Ԫ������ */
  struct EXmlElement   *parent;            /**< Ԫ�صĸ��ڵ� */
  struct EXmlElement   *next;              /**< Ԫ�ص���һ���ڵ� */
  struct EXmlElement   *first_child;       /**< Ԫ�صĵ�һ���ӽڵ� */
  struct EXmlAttr      *attr;              /**< Ԫ�ص����� */
  struct EXmlElement   *last_child;        /**< Ԫ�ص����һ���ӽڵ� */
};

/**
 * �ж�Ԫ���Ƿ����ӽڵ������
 */
#define EXML_ELEM_IS_EMPTY(e) ((e)->first_child == NULL && \
                               (e)->text == NULL )

/**
 * EXMLԪ�ؽṹ����
 */
typedef struct EXmlElement EXmlElement;

/**
 * EXML�ĵ��ṹ����
 */
typedef struct EXmlDoc EXmlDoc;


/**
 * EXML�������ṹ����
 */
typedef struct EXmlParser EXmlParser;

/**
 * EXML�������ṹ����(��·��)
 */
typedef struct TEXml  TEXml;
/** @} */


/**
 * @name EXmlDoc��EXmlParser����������
 * @{
 */

/** �ӽ�������ȡXML�ĵ���
 *  @param parser ��������
 *  @return �ɹ�����XML�ĵ���ʧ�ܷ��ؿա�
 */
EXmlDoc *EXmlGetDocument(EXmlParser *parser);

/** ����XML��������
 *  @param pool �ڴ��
 *  @return �ɹ�����XML��������ʧ�ܷ��ؿա�
 */
EXmlParser * EXmlParserCreate(TMemPool *pool);

/** XML����������XML�ĵ���
 *  @param parser ������
 *  @param data ��Ҫ����������
 *  @param len  ���ݵĴ�С
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlParserFeed(EXmlParser *parser,const char *data,uint32 len);

/** ���XML�������Ľ���������
 *  @param parser ������
 *  @param pdoc XML�ĵ�
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlParserDone(EXmlParser *parser, EXmlDoc **pdoc);

/** ����XML�ļ���
 *  @param filename �ļ�����
 *  @param doc XML�ĵ�
 *  @param pool �ڴ��
 *  @param blen BUFFER��С(ÿ�ζ��ļ�BUFFER�Ĵ�С)
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlDocParserFile(const char *filename,EXmlDoc **doc,TMemPool *pool,uint32 blen);

/** @} */


/** ��EXML�ĵ�ת���ַ�����
 *  @param pool �ڴ��
 *  @param doc  EXML�ĵ�
 *  @param buffer �����ַ���������ָ��
 *  @param size �ַ��������Ĵ�С
 *  @return  �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlDocToText(TMemPool * pool, EXmlDoc *doc,char **buffer,uint32 *size);

/** ���ַ���ת��ΪEXml�ĵ�
 *  @param pool  �ڴ��
 *  @param doc   EXML�ĵ�
 *  @param buffer �ַ�������
 *  @param size �ַ��������Ĵ�С
 *  @return  �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */

int EXmlDocParseText(TMemPool *pool,EXmlDoc **doc,const char *buffer, uint32 size);


/** ���� EXML�ĵ�
 *  @param pool  �ڴ��
 *  @param name  EXML�ĵ��ĸ��������
 *  @param doc  EXML�ĵ�
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlDocCreate(TMemPool *pool,const char *name,EXmlDoc **doc);

/** ��EXML�ĵ�������Ԫ��
 *  @param pool �ڴ��
 *  @param doc EXML�ĵ�
 *  @param name �ڵ�����(Ŀǰ��֧��·������(root/server/name))
 *  @param data ��������
 *  @param new �µ�Ԫ��(**new����NULL)
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlDocAddElement(TMemPool *pool,EXmlDoc *doc,const char *name,const char *data,EXmlElement **new);

/** ����EXML�ĵ���ȡ�����
 *  @param doc EXML�ĵ�
 *  @return �ɹ�����Element��ʧ�ܷ���NULL��
 */
EXmlElement *EXmlDocGetRootElement(EXmlDoc *doc);

/** ��EXML�ĵ��и������ƻ�ȡXMLԪ�ص����е��ӽڵ㡣
 *  @param name ����
 *  @param doc XML�ĵ�
 *  @return �ɹ�����Element��ʧ�ܷ���NULL��
 */
EXmlElement *EXmlDocGetNodeLists(const char *name,EXmlDoc *doc);

/** �������ƻ�ȡXMLԪ�ص����е��ӽڵ㡣
 *  @param name ����
 *  @param elem �ýڵ��µ�ALLԪ��
 *  @return �ɹ�����Element��ʧ�ܷ���NULL��
 */
EXmlElement *EXmlElementGetChildNode(const char *name,EXmlElement *elem);

/** ��ȡԪ�ص�����
 *  @param pool �ڴ��
 *  @param elem �ڵ�
 *  @return �ɹ��������Ӧ�����ݣ�ʧ�ܷ���NULL��
 */
char *EXmlElementGetNodeData(TMemPool *pool,EXmlElement *elem);

/** ��ȡ��Ԫ�ص�����
 *  @param pool �ڴ��
 *  @param elem �ڵ�
 *  @param name �ӽڵ������
 *  @return �ɹ��������Ӧ�����ݣ�ʧ�ܷ���NULL��
 */
char *EXmlElementGetChildNodeData(TMemPool *pool,EXmlElement *elem,const char *name);

/** ��ȡԪ�ص�����ֵ
 *  @param name ��������
 *  @param elem �ڵ�
 *  @return �ɹ��������Ӧ�����ݣ�ʧ�ܷ���NULL��
 */
const char *EXmlElementGetAttr(const char *name,EXmlElement *elem);

/** �����µĽڵ�
 *  @param pool �ڴ��
 *  @param name �ڵ�����
 *  @param data �ڵ�����
 *  @return �ɹ��������Ӧ�����ݣ�ʧ�ܷ���NULL��
 */
EXmlElement * EXmlElementCreate(TMemPool *pool,const char *name,const char *data);

/** ����Ԫ������
 *  @param pool �ڴ��
 *  @param elem �ڵ�
 *  @param name �ڵ�����
 *  @param data ��������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlElementSetAttr(TMemPool *pool,EXmlElement *elem,const char *name,const char *data);

/** ���ӽڵ�
 *  @param parent ���ڵ�
 *  @param elem �ڵ�
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int EXmlElemenAddChildNode(EXmlElement *parent,EXmlElement *elem);

/** ��ȡXML�������Ĵ���
 *  @param parser XML������
 *  @param errbuf Buffer
 *  @param errbufsize Buffer ��С
 *  @return �ɹ����ش�����Ϣ��ʧ�ܷ���NULL��
 */
char * EXmlParserGeterror(EXmlParser *parser,char *errbuf,uint32 errbufsize);


#include <stdio.h>
/** ��ӡEXML�ļ�
 *  @param level �ڵ�ȼ�
 *  @param elem �ڵ�
 *  @param fp  ����ļ�
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




