
/**
 * @file strings.h
 * @brief 常用字符串处理
 * @author tomszhou
 * @version 1.1
 * @date 2008-3-9
 * @date 2011-6-15
 */


#ifndef _TTSYS_STRINGS_H_
#define _TTSYS_STRINGS_H_

#include "ttdef.h"
#include "memory.h"

__BEGIN_DECLS


/**
 * @defgroup TTSYS_STRING 常用字符串处理
 * @ingroup TTSYS
 * @{
 */


/** 判断字符串中是否含有'\\r' '\\n' '#' ' ' '\\t' 等字符
 *  @param line  字符串
 *  @return 返回0表示不含，其他值含有以上字符串
 */

int isspaceline(char *line);


/** 拷贝字符串。
 *  @param pool 所需的内存池。
 *  @param s 源字符串
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_strdup(TMemPool *pool, const char *s);

/** 根据长度拷贝字符串。
 *  @param pool 所需的内存池。
 *  @param s 源字符串
 *  @param n 拷贝长度
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_strndup(TMemPool *pool, const char *s, int n);

/** 拼拷字符串。
 *  @param pool 所需的内存池。
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_strcat( TMemPool *pool,...);


/** 将整型数据转换成字符串。
 *  @param pool 所需的内存池。
 *  @param n 整型数据
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_itoa(TMemPool *pool, int n);

/** 将长整型数据转换成字符串。
 *  @param pool 所需的内存池。
 *  @param n 整型数据
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_ltoa(TMemPool *pool, long n);


/** 格式化输入字符串
 *  @param pool 内存池
 *  @param format 字符串格式
 *  @return 成功返回字符串的地址；失败返回空
 */
char * m_sprintf(TMemPool *pool, const char *format, ...);

/** 根据目标大小拷贝字符串
 *  @param dst 目标字符串
 *  @param src 源字符串
 *  @param dst_size 目标字符串大小
 *  @return 成功返回字符串的地址； 失败返回空 。
 */
char * m_cpystrn(char *dst, const char *src, uint32 dst_size);

/** 比较字符串
 *  @param a 字符串a
 *  @param b 字符串b
 *  @return 两字符串相同时返回0，否则返回非0值
 */
int m_strcmp(char *a,char *b);

/** 根据特定的字符串分割字符串
 *  @param str 源字符串
 *  @param sep 分割字符串
 *  @param last 存放分割后的字符
 *  @return 返回分割后的字符
 */
char *m_strtok(char *str, const char *sep, char **last);


/** 去除数据两边的空格及其他字符('\\r','\\n',' ','\\t')
 *  @param src 目标字符串
 *  @return 返回处理后的字符串
 */
char * m_trim(char* src);

/** 将字符串转换成16进制字符串
 *  @param a  所属的内存池
 *  @param bin 字符串
 *  @param len 字符串长度
 *  @return 返回16进制字符串
 */

char *Bin2Str( TMemPool * a , const char *bin , int len );

/** 将16进制字符串转换成字符串
 *  @param a  所属的内存池
 *  @param str 16进制字符串
 *  @param len 字符串长度
 *  @return 返回字符串
 */
char *Str2Bin( TMemPool * a , const char *str , int len );

/** 将二进制数据转换成16进制字符串
 *  @param bin 二进制数据
 *  @param str 16进制数据
 *  @param len 二进制数据长度
 */
void binToStr(const char *bin,char *str,int len);

/** 将16进制字数据转换成二进制串
 *  @param str 16进制数据
 *  @param bin 二进制数据
 *  @param len 二进制数据长度
 */
void strToBin( const char *str , char *bin,int len );

/** 将16进制字数据转换成ASC码
 *  @param Hex 16进制数据
 *  @param Asc ASC数据
 *  @param AscLen 16进制数据长度/2
 */
int HexToAsc(char *Hex, char *Asc, int AscLen);

/** 将ASC数据转换成16进制字数据
 *  @param Asc ASC数据
 *  @param Hex 16进制数据
 *  @param AscLen ASC数据长度
 */
int AscToHex(char *Asc, char *Hex, int AscLen);

/** 左BCD转换成ASC
 *  @param Bcd BCD数据
 *  @param Asc Asc数据
 *  @param BcdLen BCD数据长度
 */
int BcdToAscL(char *Bcd, char *Asc, int BcdLen);

/** 右BCD转换成ASC
 *  @param Bcd BCD数据
 *  @param Asc Asc数据
 *  @param BcdLen BCD数据长度
 */
int BcdToAscR(char *Bcd, char *Asc, int BcdLen);

/** ASC转换成左BCD
 *  @param Asc Asc数据
 *  @param Bcd BCD数据
 *  @param AscLen Asc数据长度
 */
int AscToBcdL(char *Asc, char *Bcd, int AscLen);

/** ASC转换成右BCD
 *  @param Asc Asc数据
 *  @param Bcd BCD数据
 *  @param AscLen Asc数据长度
 */
int AscToBcdR(char *Asc, char *Bcd, int AscLen);

/** ASC转换成各种格式数据
 *  @param Asc Asc数据
 *  @param Nnn 格式数据数据
 *  @param AscLen Asc数据长度
 *  @param Type 数据类型(A,L,R,H)
 */
int AscToNnn(char *Asc,char **Nnn,int AscLen,char Type);

/** 各种格式数据转换成ASC
 *  @param Nnn 格式数据数据
 *  @param Asc Asc数据
 *  @param AscLen Asc数据长度
 *  @param Type 数据类型(A,L,R,H)
 */
int NnnToAsc(char **Nnn,char *Asc,int AscLen,char Type);

/** @} */

__END_DECLS

#endif

