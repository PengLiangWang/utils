#ifndef _SERVICE_BUF_INCLUDE_H_
#define _SERVICE_BUF_INCLUDE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FLDTYPE_INT           0x33     /* ����     */
#define FLDTYPE_DOUBLE        0x34     /* С��     */
#define FLDTYPE_STR           0x35     /* �ַ���   */
#define FLDTYPE_IMG           0x36     /* �ֽ����� */

#define SERVICE_NAMELEN       24       /* �������� */

#define SBUF_ERR_ERROR        -1100  /* SBUF����ʧ��   */
#define SBUF_ERR_BUFOVER      -1101  /* �������ĳ���   */
#define SBUF_ERR_TYPEERR      -1102  /* �����ʹ���     */
#define SBUF_ERR_BADDATA      -1103  /* ���ݸ�ʽ����   */ 
#define SBUF_ERR_NOFIELD      -1104  /* �����ڵ���     */
#define SBUF_ERR_VALBUFOVER   -1105  /* valLen���Ȳ��� */
#define SBUF_ERR_NOMEMORY     -1106  /* �ڴ治��       */
#define SBUF_ERR_DUPDATA      -1107  /* �����ظ�       */

typedef struct 
{
  char  *buff;
}SERVICEBUF;

typedef struct 
{
  char  name[SERVICE_NAMELEN + 1];
  int   type;
}SERVICEFIELD;


#define FIELDDEFINE(NAME,TYPE)   SERVICEFIELD NAME = {#NAME,TYPE}


int InitServiceBuf(SERVICEBUF *);
int FreeServiceBuf(SERVICEBUF *);

int AddServiceField(SERVICEBUF *,SERVICEFIELD *,char *val,int vallen);
int GetServiceField(SERVICEBUF *,SERVICEFIELD *,char *val,int *vallen,int maxsize);
int CheckServiceField(SERVICEBUF *sbuf,SERVICEFIELD *field);

int GetServiceFieldNum(SERVICEBUF *);

int GetServiceBufLen(SERVICEBUF *,int *size);

int ServiceBufToStream(SERVICEBUF *,int bufsize,int *bufuse,char *buf);
int StreamToServiceBuf(SERVICEBUF *,int bufsize,int *bufuse,char *buf);

int ServiceBufDebug(char *filename,char *title,SERVICEBUF *);


#ifdef __cplusplus
}
#endif

#endif

