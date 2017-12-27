#include "ttsys.h"
#include "sheet.h"


#define SHEETEX_ASSERT(exp,rv)       \
  {                                  \
    if ( (exp))                      \
    {                                \
      return rv;                     \
    }                                \
  }

typedef struct _sheet_entry_t_  TSheetEntry;

typedef struct _sheet_data_t_   TSheetData;

struct _sheet_data_t_
{
   int           type;
   int           len;
   char          data[512];
};

struct _sheet_entry_t_
{
   TSheetEntry    *next;
   uint32          hash;
   char            name[32];
   uint32          nlen;
   TSheetData     *data;
};

struct _sheet_index_t_
{
   TSheet        *sheet;        /**< 索引             */
   TSheetEntry   *this;         /**< 索引的当前节点   */
   TSheetEntry   *next;         /**< 索引的下一个节点 */
   uint32         index;        /**< 索引值           */
};


struct _sheet_t_
{
   TMemPool        *pool;
   TSheetEntry    **array;
   TSheetIndex      iterator;  
   uint32           count;
   uint32           max;
   TSheetEntry     *free;
  
};

#define SHEET_MAX_ITEMS    4096


static uint32 SheetFuncDefault33(const char *name,uint32 *klen)
{
   uint32         hash = 0;
   uchar         *key = (uchar *)name;
   const uchar   *p;
   uint32         i;

   if ( *klen == -1 )
   {
      for (p = key; *p; p++)
      {
         hash = hash * 33 + *p;
      }
      *klen = p - key;
   }
   else
   {
      for (p = key, i = *klen; i; i--, p++)
      {
         hash = hash * 33 + *p;
      }
   }
   
   return hash;
}

int SheetMake(TMemPool *pool, TSheet **sheet)
{
   TSheet        *new;

   new = (TSheet *)MemPoolAllocEx(pool, sizeof(TSheet));
   if ( new == NULL )
   {
      return TTS_ENOMEM;
   }
   new->pool = pool;

   new->array = (TSheetEntry **)MemPoolAllocEx(pool, sizeof(TSheetEntry *) * SHEET_MAX_ITEMS);
   if ( new->array == NULL )
   {
      return TTS_ENOMEM;
   }
   new->max = SHEET_MAX_ITEMS - 1;
   new->count = 0;
   new->free = NULL;
   *sheet = new;

   return TTS_SUCCESS;
}


static TSheetEntry **SheetFindEntry(TSheet *sheet, char *name, char *data)
{
   uint32           hash;
   uint32           nlen = -1;
   TSheetEntry  **hep,*he;

   hash = SheetFuncDefault33(name, &nlen);
   for (hep = &sheet->array[hash & sheet->max],he = *hep; he ; hep = &he->next,he = *hep)
   {
      if ( he->hash == hash && he->nlen == nlen && memcmp(he->name, name, nlen) == 0 )
      {
         break;
      }
   }
   
   if ( he || !data )
   {
      return hep;
   }

   if ((he = sheet->free) != NULL)
   {
      sheet->free = he->next;
   }
   else
   {
      he = (TSheetEntry *)MemPoolAllocEx(sheet->pool, sizeof(TSheetEntry));
   }
   if ( he == NULL )
   {
      *hep = NULL;
      return hep;
   }

   he->next = NULL;
   he->hash = hash;
   he->nlen = nlen;
   memcpy(he->name, name, nlen);
   *hep = he;
   sheet->count++;

   return hep;
}

int SheetPut(TSheet *sheet, char *name, void *data, int type, int len)
{
   TSheetEntry **hep;

   if ( len <= 0 )
   {
      return TTS_EINVAL;
   }
   else if ( len > 512 )
   {
      len = 512;
   }

   hep = SheetFindEntry(sheet, name, (char *)data);
   if ( *hep )
   {
      if ( data )
      {
         if ( (*hep)->data == NULL )
         {
            (*hep)->data = (TSheetData *)MemPoolAllocEx(sheet->pool, sizeof(TSheetData));
         }
         else
         {
            memset((*hep)->data, 0, sizeof(TSheetData));
         }
         (*hep)->data->type = type;
         (*hep)->data->len = len;
         memcpy((*hep)->data->data, data, len);
      }
      return TTS_SUCCESS;
   }

   return TTS_ENOMEM;
}

int SheetGet(TSheet *sheet, char *name, void *data, int type, int *len)
{
   TSheetEntry *he;

   he = *SheetFindEntry(sheet, name, (char *)data);
   if ( he && he->data )
   {
      SHEETEX_ASSERT( ( he->data->type != type ), TTS_EINVAL);
      memset(data, 0, *len);
      memcpy(data, he->data->data, he->data->len);
      *len = he->data->len;
      return TTS_SUCCESS;
   }

   return TTS_NOTFOUND;
}

int SheetGet2( TSheet *sheet,char *name,void **data,int *type, int *len)
{
   TSheetEntry *he;

   he = *SheetFindEntry(sheet, name, (char *)data);
   if ( he && he->data )
   {
      *data = he->data->data;
      *len = he->data->len;
      *type = he->data->type;
      return TTS_SUCCESS;
   }

   return TTS_NOTFOUND;
}


int SheetGet3(TSheet *sheet, char *name, void *data, int type, int len)
{
   TSheetEntry *he;

   he = *SheetFindEntry(sheet, name, (char *)data);
   if ( he && he->data )
   {
      SHEETEX_ASSERT( ( he->data->type != type ), TTS_EINVAL);
      memset(data, 0, len);
      if ( len < he->data->len || len > he->data->len)
      {
         len = he->data->len;
      }
      memcpy(data, he->data->data, len);
      return TTS_SUCCESS;
   }

   return TTS_NOTFOUND;
}

int SheetDel(TSheet *sheet, char *name )
{
   TSheetEntry **hep;

   hep = SheetFindEntry(sheet, name, (char *)NULL);
   if ( hep )
   {
      TSheetEntry  *old = *hep;
      *hep = (*hep)->next;
      old->next = sheet->free;
      sheet->free = old;
      --sheet->count;
      return TTS_SUCCESS;
   }

   return TTS_NOTFOUND;
}

uint32 SheetGetCount(TSheet *sheet)
{
  if (sheet)
  {
     return sheet->count;
  }

  return 0;
}

int SheetClear(TSheet *sheet)
{
   return TTS_SUCCESS;
}

TSheetIndex *SheetNext(TSheetIndex *hi)
{
  hi->this = hi->next;

  while ( !hi->this )
  {
     if ( hi->index > hi->sheet->max )
     {
        return NULL;
     }
     hi->this = hi->sheet->array[hi->index++];
     
  }
  hi->next = hi->this->next;
  return hi;
}

TSheetIndex  *SheetFirst( TSheet *sheet )
{
  TSheetIndex *hi;

  if ( sheet == NULL )
  {
    return NULL;
  }

  hi = (TSheetIndex *)&sheet->iterator;
  hi->sheet  = sheet;
  hi->index = 0;
  hi->this = NULL;
  hi->next = NULL;

  return SheetNext( hi );
}

void SheetThis( TSheetIndex *hi,char **name,void **data,char *type,int *len)
{
   if ( name ) 
   {
     *name  = hi->this->name;
   }
   if ( hi->this->data )
   {
      if ( data )
      {
         *data = hi->this->data->data;
      }
      if ( type )
      {
         *type = hi->this->data->type;
      }
      if ( len )
      {
         *len = hi->this->data->len;
      }
   }
}

int SheetForeach(const TSheet *sheet,TSheetCallbackFn *comp, void *rec)
{
  TSheetIndex    hix;
  TSheetIndex   *hi;
  int         rv = 0;

  hix.sheet = (TSheet *)sheet;
  hix.index = 0;
  hix.this = NULL;
  hix.next = NULL;

  if ( (hi = SheetNext(&hix)) )
  {
     do
     {
        if ( hi->this->data )
        {
           rv = (*comp)(rec, hi->this->name, hi->this->data->data, hi->this->data->type, hi->this->data->len);
        }
        else
        {
            rv = (*comp)(rec, hi->this->name, NULL, 0, 0);
        }
     }
     while (rv == 0 && (hi = SheetNext(hi)) );
  }

  return rv;
}


int SheetStorage(TMemPool *pool,TSheet *sheet,TSheetStore **store,int *bad)
{
   int             total,sum,i,j;
   TSheetEntry  *entry,*he;
   TSheetStore  *tlt;

   if ( sheet == NULL )
   {
      return TTS_EINVAL;
   }

   tlt = (TSheetStore *)MemPoolAllocEx(pool,sizeof(TSheetStore) * (sheet->max) );
   total = sum = 0;

   for ( i = 0 ; i < sheet->max ; i++ )
   {
      entry = (TSheetEntry *)sheet->array[i];

      for ( ; entry && entry->nlen > 0 ; )
      {
         sum++;
         entry = entry->next;
      }

      for ( j = 0 ; sum && j < total + 1; j++)
      {
         if ( tlt[j].bad == sum )
         {
            tlt[j].total += sum;
            sum = 0 ;
            break;
         }
      }

      if ( sum )
      {
         tlt[total].bad = sum;
         tlt[total].total = sum;
         total++;
         sum = 0;
      }
   }

   *bad = total;
   *store = tlt;

   return TTS_SUCCESS;
}

