
/**
 * @file elog.h
 * @brief  ��־����ģ��(֧��Զ��)
 * @author tomszhou
 * @version 1.0
 * @date 2011-5-4
 */


#ifndef _TTSYS_ELOG_H_
#define _TTSYS_ELOG_H_

__BEGIN_DECLS

/**
 * @defgroup TTSYS_ELOG ��־����ģ��(֧��Զ��)
 * @ingroup TTSYS
 * @{
 */

/**
 * ��־��������
 */
enum
{
   LOG_LEVEL_ERROR   =   0x00,     /**< ERROR */
   LOG_LEVEL_WARN    =   0x01,     /**< WRAN  */
   LOG_LEVEL_INFO    =   0x02,     /**< INFO  */
   LOG_LEVEL_DEBUG   =   0x03,     /**< DEBUG */
   LOG_MASK_TAG      =   0x01,     //TAG
   LOG_FILE_MASK     =   0x01,     /**< file��ʽ */
   LOG_HOST_MASK     =   0x02      /**< server��ʽ */
};

/** ��־��ʼ��
 *  @param pri     ��־�ȼ�
 *  @param module  ģ��
 *  @param path    ·��
 *  @param file    ��־�ļ�
 *  @param lmask   ��־��¼��ʽ
 *  @param host    server (129.0.1.1:1234)
 *  @return 
 */
void eloginit(int pri,char *module,char *path,char *file,int lmask,char *host);

/** ������־�ȼ�
 *  @param pri     ��־�ȼ�
 *  @return
 */
void eloglevel(int pri);

/** ������־��¼��ʽ
 *  @param mask   ��־��¼��ʽ
 *  @return 
 */
void elogmask(int mask);

/** ��ȡ�Ƿ��ǵ���״̬
 *  @return 1: �� 0:����
 */
int elogisdebug();

/** ��¼��־
 *  @param pri  ��־�ȼ�
 *  @param function ����(__FUNCITON__)
 *  @param file �������ڵ��ļ�(__FILE__)
 *  @param line �ļ�������(__LINE__)
 *  @param fmt  ��ʽ
 *  @return
 */
void elog(int pri,const char *function, const char *file,const int line,char *fmt,...);


FILE *eloggetfilebyname(char *filename);
__END_DECLS


/**
 * @name ��־���ú����궨��
 * @{
 */
#define PREFIXLOG(level)  LOG_LEVEL_##level,__FUNCTION__,__FILE__,__LINE__

#define ELOG(level, ...)  elog(PREFIXLOG(level), __VA_ARGS__)

#define ELOG_DEBUG()    elogisdebug()

/** @} */

/** @} */

#endif

