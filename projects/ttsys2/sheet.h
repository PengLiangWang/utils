
/**
 * @file sheet.h
 * @brief ��������
 * @author tomszhou
 * @version 1.0
 * @date 2011-2-17
 */


#ifndef _TTSYS_SHEET_H_
#define _TTSYS_SHEET_H_


#include "ttdef.h"
#include "memory.h"



__BEGIN_DECLS

/**
 * @defgroup TTSYS_SHEETX λͼ���ú���
 * @ingroup TTSYS
 * @{
 */

typedef struct _sheet_t_ TSheet;
typedef struct _sheet_index_t_ TSheetIndex;

enum SHEET_CONST
{
   SHEET_SIZE       =  0x100000 /* �������Ĵ�С */
};

typedef int (TSheetCallbackFn)(void *rec, const char *name, const void *value, const int type, const int len);

int SheetMake(TMemPool *pool, TSheet **sheet);

int SheetPut(TSheet *sheet, char *name, void *data, int type, int len);

int SheetGet(TSheet *sheet, char *name, void *data, int type, int *len);

int SheetGet2( TSheet *sheet,char *name,void **data,int *type, int *len);

int SheetGet3(TSheet *sheet, char *name, void *data, int type, int len);

int SheetDel(TSheet *sheet, char *name );

uint32 SheetGetCount(TSheet *sheet);

int SheetClear(TSheet *sheet);

TSheetIndex *SheetNext(TSheetIndex *hi);

TSheetIndex  *SheetFirst( TSheet *sheet );

void SheetThis( TSheetIndex *hi,char **name,void **data,char *type,int *len);

int SheetForeach(const TSheet *sheet,TSheetCallbackFn *comp, void *rec);

typedef struct
{
  int     bad;
  int     total;
}TSheetStore;

int SheetStorage(TMemPool *pool,TSheet *sheet,TSheetStore **store,int *bad);


/** @} */

#ifdef __cplusplus
}
#endif


#endif


