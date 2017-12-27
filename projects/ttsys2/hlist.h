
/**
 * @file hlist.h
 * @brief ��ϣ���������㷨�������ϵ����ݽ��������û��Զ��塣
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-18
 */


#ifndef _TTSYS_HASHLIST_H_
#define _TTSYS_HASHLIST_H_

#include "ttdef.h"
#include "list.h"

__BEGIN_DECLS

/**
 * @defgroup TTSYS_HLIST ��ϣ����
 * @brief ��ϣ���������㷨�������ϵ����ݽ��������û��Զ��塣
 * @author tomszhou
 * @version 1.0
 * @date 2008-3-18
 * @ingroup TTSYS 
 * @{
 */

/**
 * @name ��ϣ�����ûص���������
 * @{
 */

typedef int (*GetHashValueFunc)(void *userData,int hasMaxVal);
typedef int (*CompressFunc)(void *data1,void *data2);

/** @} */

/**
 * HList ��ϣ���������
 */

typedef struct HList HList;

/**
 * HList ��ϣ�������ݽṹ����
 */
struct HList
{
  GetHashValueFunc   GetHashValue;   /**< ��ȡ��ϣֵ���� */
  CompressFunc       compress;       /**< �ȽϺ��� */
  int                maxHashValue;   /**< ��ϣֵ */
  TList             *itemList;       /**< ������ */
};


/** \fn int HListNew(HList *hList,GetHashValueFunc GetHashValue,CompressFunc compress,int  maxHashValue)
 *  \brief ������ϣ����
 *  @param hList ��Ҫ�����Ĺ�ϣ����
 *  @param GetHashValue ��ȡ��ϣֵ�Ļص�����
 *  @param compress ��ϣ������Ԫ�رȽϵĻص�����
 *  @param maxHashValue ���Ĺ�ϣֵ(�Ǹ����������� 2^n - 1)
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListNew(HList *hList,GetHashValueFunc GetHashValue,CompressFunc compress,int  maxHashValue);

/** HListResize: ���ݹ�ϣֵ���¹����ϣ����
 *  @param hList ��Ҫ���¹���Ĺ�ϣ����
 *  @param maxHashValue ���Ĺ�ϣֵ(�Ǹ����������� 2^n - 1)
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListResize(HList *hList, int maxHashValue);

/** HListAdd: ����Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param userData �����ӵ�Ԫ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListAdd(HList *hList,void *userData);

/** HListDelete: ɾ����ϣ�����е�Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param userData ��ɾ����Ԫ��
 *  @param delFunc ɾ��Ԫ��ʱ�Ļص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListDelete(HList *hList,void *userData,int (*delFunc)(void *userData));

/** HListFind: ���ҹ�ϣ�����е�Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param indexData Ԫ�ر�ʶ
 *  @param userDataP �ҵ������Ӧ��Ԫ��
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListFind(HList *hList,void *indexData,void **userDataP);

/** HListFind2: ���ҹ�ϣ�����е�Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param indexData Ԫ�ر�ʶ
 *  @param userDataP �ҵ������Ӧ��Ԫ��
 *  @param compress ��ϣ������Ԫ�رȽϵĻص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListFind2(HList *hList,void *indexData,void **userDataP,CompressFunc compress);

/** HListClean: ��չ�ϣ�����е�Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param delFunc ɾ��Ԫ��ʱ�Ļص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListClean(HList *hList,int (*delFunc)(void *userData));

/** HListFree: �ͷŹ�ϣ�����е�Ԫ��
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param delFunc ɾ��Ԫ��ʱ�Ļص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListFree(HList *hList,int (*delFunc)(void *userData));

/** HListForeach: ������ϣ�����е�����
 *  @param hList ���Ӧ�Ĺ�ϣ����
 *  @param foreachFunc ����ʱ�Ļص�����
 *  @return ����TTS_SUCCESS�ɹ�
 */
int HListForeach(HList *hList,int (*foreachFunc)(void *userdata));

/** defaultGetHashValue: Ĭ�ϵĹ�ϣֵ���㺯��
 *  @param key ��Ӧ�ļ�ֵ
 *  @param length ��ֵ�ĳ���
 *  @return ����TTS_SUCCESS�ɹ�
 */
uint32 defaultGetHashValue(uchar *key,int length);


/** @} */

__END_DECLS

#endif



