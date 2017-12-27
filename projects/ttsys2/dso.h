
/**
 * @file dso.h
 * @brief ��̬��������,�Զ�̬��Ĳ�����װ��
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
 * @defgroup TTSYS_DSO ��̬��
 * @ingroup TTSYS
 * @{
 */


/**
 * ��̬��������
 */
typedef struct TDllHandle  TDllHandle;

/**
 * ��̬����Ŷ���
 */
typedef void *             TDllHandleSym;


/** װ�ض�̬��(.so)
 *  @param res_handle ��̬����Ϣ
 *  @param path  ·��
 *  @param pool �ڴ��
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int DllLoad(TDllHandle **res_handle,const char *path, TMemPool *pool);

/** ж�ض�̬��(.so)
 *  @param handle ��̬����Ϣ
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int DllUnload(TDllHandle *handle);

/** �������������Ӧ�Ķ���
 *  @param ressym ��Ŷ����ָ��
 *  @param handle  ��̬����Ϣ
 *  @param symname ��������
 *  @return �ɹ�����TTS_SUCCESS��ʧ�ܷ������Ӧ�Ĵ�����롣
 */
int DllSym(TDllHandleSym *ressym, TDllHandle *handle, const char *symname);

/** ��ȡ��̬��Ĵ�����Ϣ
 *  @param dll ��̬����Ϣ
 *  @param buffer  ������Ϣ
 *  @param buflen ��Ŵ�����ϢBUFFER�Ĵ�С
 *  @return �ɹ����ش�����Ϣ��ʧ�ܷ���"No Error"��
 */
const char * DllError(TDllHandle *dll, char *buffer, uint32 buflen);

/** @} */

__END_DECLS



#endif
