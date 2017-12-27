
/**
 * @file file.h
 * @brief  �ļ��������
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
 * @defgroup TTSYS_FILE �ļ��������
 * @ingroup TTSYS
 * @{
 */

/**
 * �ļ����ݴ洢�ṹ����
 */
typedef struct TFile  TFile;


/**
 * �ļ���ʶ����
 */
enum
{
  TFILE_CLEANUP    = 0x00800,        /**< �ر��ļ�ʱɾ���ļ� */
  TFILE_NOTCLEANUP = 0               /**< �ر��ļ�ʱ��ɾ���ļ� */
};

/** ���ļ�
 *  @param file �ļ����ݴ洢��
 *  @param name �ļ���
 *  @param flag ���ļ��ı�ʶ
 *  @param pool ���õ��ڴ��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileOpen(TFile **file,const char *name, int flag,TMemPool *pool);

/** ����������ʱ�ļ�
 *  @param file �ļ����ݴ洢��
 *  @param base �ļ�����ǰ׺
 *  @param path �ļ����·��
 *  @param flag ���ļ��ı�ʶ
 *  @param pool ���õ��ڴ��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int TmpFileOpen(TFile **file,const char *base,const char *path, int flag,TMemPool *pool);

/** �ر��ļ�
 *  @param file �ļ����ݴ洢��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileClose(TFile *file);

/** ɾ���ļ�
 *  @param path �ļ�·��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileRemove(const char *path);

/** �ļ���������
 *  @param from_path ԭ�ļ���
 *  @param to_path ���ļ���
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileRename(const char *from_path, const char *to_path);

/** ��ȡ�ļ�
 *  @param thefile �ļ����ݴ洢��
 *  @param buf ���ݴ洢��
 *  @param nbytes ��Ҫ��ȡ�����ݵĴ�С(��������ʱ��Ŷ�ȡ�����ݴ�С)
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileRead(TFile *thefile, void *buf, uint32 *nbytes);

/** д���ļ�
 *  @param thefile �ļ����ݴ洢��
 *  @param buf ���ݴ洢��
 *  @param nbytes ��Ҫд������ݵĴ�С(��������ʱ���д������ݴ�С)
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileWrite(TFile *thefile, const void *buf, uint32 *nbytes);

/** д���ļ�(����BUF�Ĵ�Сд���ļ�)
 *  @param thefile �ļ����ݴ洢��
 *  @param buf ���ݴ洢��
 *  @return �ɹ�����TTS_SUCCESS��
 */
int FileWriteEx(TFile *thefile, const char *buf);


/** @} */

__END_DECLS

#endif
