#ifndef _SERVICE_BUF_INCLUDE_H_
#define _SERVICE_BUF_INCLUDE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FLDTYPE_INT           0x33     /* 整数     */
#define FLDTYPE_DOUBLE        0x34     /* 小数     */
#define FLDTYPE_STR           0x35     /* 字符串   */
#define FLDTYPE_IMG           0x36     /* 字节数组 */

#define SERVICE_NAMELEN       24       /* 域名长度 */

#define SBUF_ERR_ERROR        -1100  /* SBUF操作失败   */
#define SBUF_ERR_BUFOVER      -1101  /* 超出报文长度   */
#define SBUF_ERR_TYPEERR      -1102  /* 域类型错误     */
#define SBUF_ERR_BADDATA      -1103  /* 数据格式错误   */ 
#define SBUF_ERR_NOFIELD      -1104  /* 不存在的域     */
#define SBUF_ERR_VALBUFOVER   -1105  /* valLen长度不足 */
#define SBUF_ERR_NOMEMORY     -1106  /* 内存不足       */
#define SBUF_ERR_DUPDATA      -1107  /* 数据重复       */

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

