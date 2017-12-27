#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ServiceBuf.h"
#include "d_malloc.h"
#include "dlist.h"

typedef DList  Sheet;

#define SERVICEITEMHEADSIZE  SERVICE_NAMELEN + sizeof(int ) * 3

typedef struct
{
  char  ID[SERVICE_NAMELEN + 4];
  int   type;
  int   size;
  char  data[1];
} DataItem;

Sheet *Sheet_new(void)
{
  Sheet *sheet;
 
  sheet = (Sheet *)d_malloc( sizeof(Sheet) );
  if (sheet == NULL)
  {
    return NULL;
  }

  memset(sheet,0,sizeof(Sheet));

  return sheet ;
}

int InitServiceBuf(SERVICEBUF *sbuf)
{
  Sheet *sheet;
  
  if (sbuf == NULL || sbuf->buff != NULL)
  {
    return -1; 
  }

  sheet = (Sheet *)d_malloc( sizeof(Sheet) );
  if (sheet == NULL)
  {
    return -1;
  }

  memset(sheet,0,sizeof(Sheet));

  sbuf->buff = (char*)sheet;

  return 0 ;
}

static int freeF(void *data)
{
  d_free(data);
  return 0;
}

int Sheet_clear(Sheet *sheet)
{
  DList_clear(sheet,freeF);
  return 0;
}


int Sheet_free(Sheet *sheet)
{
  Sheet_clear(sheet);
  d_free(sheet);
  return 0;
}

int FreeServiceBuf(SERVICEBUF *sbuf)
{
  
  Sheet_clear((Sheet *)sbuf->buff);
  
  d_free((Sheet *)sbuf->buff);

  sbuf->buff = NULL;

  return 0;
}

static int Sheet_find(Sheet *sheet,char *ID,DataItem **p)
{
  int             rv;
  DList_stepInfo  sinfo;
  DataItem        *item;

  rv = DList_openStep(sheet,&sinfo);
  if (rv != 0)
  {
    return SBUF_ERR_ERROR;
  }

  while ( DList_fetchStep(sheet,&sinfo,(void **)&item) == 0 )
  {
    if ( strcmp(ID,item->ID) == 0 )
    {
      DList_closeStep(sheet,&sinfo);
      *p = item;
      return 0;
    }
  }

  DList_closeStep(sheet,&sinfo);

  return SBUF_ERR_NOFIELD;
}

int Sheet_del(Sheet *sheet,char *ID)
{
  int       rv;
  DataItem  *p;

  rv = Sheet_find(sheet,ID,&p);
  if (rv == 0)
  {
    DList_del(sheet,p,freeF);
  }

  return 0;
}

int Sheet_add(Sheet *sheet,char* ID,int type,int size,void *data)
{
  DataItem  *item;
  int       rv;

  rv = Sheet_find(sheet,ID,&item);
  if (rv != SBUF_ERR_NOFIELD && rv != 0)
  {
    return SBUF_ERR_ERROR;
  }
  else if (rv == 0)
  {
    return SBUF_ERR_NOFIELD;
  }
  
  item = (DataItem *)d_malloc(SERVICEITEMHEADSIZE + size + 1);
  if ( item == NULL )
  {
    return SBUF_ERR_NOMEMORY;
  }

  memset(item,0,SERVICEITEMHEADSIZE + size + 1);

  strcpy(item->ID,ID);
  item->type = type;
  item->size = size;
  memcpy(item->data,data,size);
  
  rv = DList_add(sheet,item,NULL);
  if (rv != 0)
  {
    d_free(item);
    return SBUF_ERR_ERROR;
  }
  
  return 0;
}

int CheckServiceField(SERVICEBUF *sbuf,SERVICEFIELD *field)
{
  Sheet    *sheet;
  DataItem *item;
  int       rv;

  sheet = (Sheet *)sbuf->buff;

  rv = Sheet_find(sheet,field->name,&item);
  if( rv != SBUF_ERR_NOFIELD && rv != 0 )
  {
    return SBUF_ERR_ERROR;
  }

  return rv;
}

int AddServiceField(SERVICEBUF *sbuf,SERVICEFIELD *field,char *val,int  vallen)
{
  Sheet     *sheet;
  DataItem  *item;
  int        rv;
 
  sheet = (Sheet *)sbuf->buff; 

  rv = Sheet_find(sheet,field->name,&item);
  if (rv != SBUF_ERR_NOFIELD && rv != 0)
  {
    return SBUF_ERR_ERROR;
  }
  else if (rv == 0)
  {
    return SBUF_ERR_DUPDATA;
  }
  
  item = (DataItem *)d_malloc(SERVICEITEMHEADSIZE + vallen + 1);
  if ( item == NULL )
  {
    return SBUF_ERR_NOMEMORY;
  }

  memset(item,0,SERVICEITEMHEADSIZE + vallen + 1);

  strcpy(item->ID,field->name);
  item->type = field->type;
  item->size = vallen;
  memcpy(item->data,val,vallen);

  rv = DList_add(sheet,item,NULL);
  if (rv != 0)
  {
    d_free(item);
    return SBUF_ERR_ERROR;
  }
  
  return 0;
}


int Sheet_get(Sheet *sheet,char *ID,int maxsize,int *type,int *size, void *data)
{
  DataItem  *item;
  int        rv;

  rv = Sheet_find(sheet,ID,&item);
  if (rv != 0)
  {
    return (rv == SBUF_ERR_NOFIELD) ? SBUF_ERR_NOFIELD : SBUF_ERR_ERROR;
  }

  if (item->size > maxsize)
  {
    return SBUF_ERR_BUFOVER;
  }

  *type = item->type;
  *size = item->size;

  if (item->size == 0)
  {
    return 0;
  }

  memcpy(data,item->data,item->size);
 
  return 0;
}

int GetServiceField(SERVICEBUF *sbuf,SERVICEFIELD *field,char *val,int *vallen,int maxsize)
{
  Sheet     *sheet;
  DataItem  *item;
  int        rv;


  memset(val,0,maxsize);

  sheet = (Sheet *)sbuf->buff; 

  rv = Sheet_find(sheet,field->name,&item);
  if (rv != 0)
  {
    return (rv == SBUF_ERR_NOFIELD) ? SBUF_ERR_NOFIELD : SBUF_ERR_ERROR;
  }

  if (item->size > maxsize)
  {
    return SBUF_ERR_VALBUFOVER;
  }
  
  *vallen = item->size;

  if (item->type != field->type)
  {
    return SBUF_ERR_TYPEERR;
  }

  if (item->size == 0)
  {
    return 0;
  }

  memcpy(val,item->data,item->size);
 
  return 0;
}


int Sheet_add_int(Sheet *sheet,char *ID,int data)
{
  return Sheet_add(sheet,ID,FLDTYPE_INT,sizeof(int),&data);
}

int Sheet_add_double(Sheet *sheet,char *ID,double data)
{
  return Sheet_add(sheet,ID,FLDTYPE_DOUBLE,sizeof(double),&data);
}

int Sheet_add_str(Sheet *sheet,char *ID,char *str)
{
  if (str == NULL)
  {
    return SBUF_ERR_BADDATA;
  }

  return Sheet_add(sheet,ID,FLDTYPE_STR,strlen(str),str);

}

int Sheet_add_img(Sheet *sheet,char *ID,void *img,int size)
{
  if (img == NULL)
  {
    return SBUF_ERR_BADDATA;
  }

  return Sheet_add(sheet,ID,FLDTYPE_IMG,size,img);
}

int Sheet_get_int(Sheet *sheet,char *ID,int *data)
{
  int size,type;
  int rv;

  rv = Sheet_get(sheet,ID,sizeof(int),&type,&size,data);
  if (rv != 0)
  {
    return rv;
  }

  if (type != FLDTYPE_INT)
  {
    return SBUF_ERR_TYPEERR ;
  }

  return 0 ;
}

int Sheet_get_double(Sheet *sheet,char *ID,double *data)
{
  int size,type;
  int rv;

  rv = Sheet_get(sheet,ID,sizeof(double),&type,&size,data);
  if (rv != 0)
  {
    return rv;
  }

  if (type != FLDTYPE_DOUBLE)
  {
    return SBUF_ERR_TYPEERR;
  }

  return 0 ;
}

int Sheet_get_str(Sheet *sheet,char *ID,int maxsize,char *str)
{
  int size,type;
  int rv;

  rv = Sheet_get(sheet,ID,maxsize,&type,&size,str);
  if (rv != 0)
  {
    return rv;
  }

  if ( type != FLDTYPE_STR )
  {
    return SBUF_ERR_TYPEERR;
  }

  return 0 ;
}


int Sheet_get_img(Sheet *sheet,char *ID,int maxsize,void *img)
{
  int size,type;
  int rv;

  rv = Sheet_get(sheet,ID,maxsize,&type,&size,img);
  if (rv != 0)
  {
    return rv;
  }

  if ( type != FLDTYPE_IMG )
  {
    return SBUF_ERR_TYPEERR;
  }

  return 0 ;
}

int GetServiceFieldNum(SERVICEBUF *sbuf)
{
  Sheet *sheet = (Sheet *)sbuf->buff;

  return sheet->cnt;
}

int GetServiceBufLen(SERVICEBUF *sbuf,int *size)
{
  Sheet          *sheet;
  DList_stepInfo  sinfo;
  int             rv;
  DataItem        *item;
  
  sheet = (Sheet *)sbuf->buff;

  *size = 0;

  rv = DList_openStep(sheet,&sinfo);
  if (rv != 0 )
  {
    return SBUF_ERR_ERROR;
  }

  *size += 3;

  while( DList_fetchStep(sheet,&sinfo,(void *)&item) == 0 )
  {
    *size += SERVICE_NAMELEN;
    switch(item->type)
    {
      case FLDTYPE_INT:
        *size += 17;
        break;
      case FLDTYPE_DOUBLE:
        *size += 21;
        break;
      case FLDTYPE_STR:
      case FLDTYPE_IMG:
        *size += 5 + item->size;
        break;
      default :
        DList_closeStep(sheet,&sinfo);
        return SBUF_ERR_ERROR;
    }
  }

  DList_closeStep(sheet,&sinfo);
  
  return 0;
}

int ServiceBufToStream(SERVICEBUF *sbuf,int bufsize,int *bufuse,char *buf)
{
  DList_stepInfo  sinfo;
  int             rv;
  DataItem        *item;
  int             value_i;
  double          value_d;
  Sheet          *sheet;

  sheet = (Sheet *)sbuf->buff;

  sprintf(buf,"%03d",sheet->cnt);
  *bufuse = 3;

  rv = DList_openStep(sheet,&sinfo);
  if (rv != 0 )
  {
    return SBUF_ERR_ERROR;
  }

  while( DList_fetchStep(sheet,&sinfo,(void *)&item) == 0 )
  {
    memcpy(buf + *bufuse,item->ID,SERVICE_NAMELEN);
    *bufuse += SERVICE_NAMELEN;

    switch(item->type)
    {
      case FLDTYPE_INT: 
        if (bufsize < *bufuse + 12 + 1)
        {
          DList_closeStep(sheet,&sinfo);
          return SBUF_ERR_ERROR;
        }
        memcpy(&value_i,item->data,sizeof(value_i));
        buf[(*bufuse)++] = item->type;
        sprintf(buf + *bufuse,"%012d",value_i);
        *bufuse += 12;
        break;
      case FLDTYPE_DOUBLE:
        if (bufsize < *bufuse + 16 + 1)
        {
          DList_closeStep(sheet,&sinfo);
          return SBUF_ERR_ERROR;
        }
        memcpy(&value_d,item->data,sizeof(double));
        buf[(*bufuse)++] = item->type;
        sprintf(buf + *bufuse,"%016.3f",value_d);
        *bufuse += 16;
        break;
      case FLDTYPE_STR:
      case FLDTYPE_IMG:
        if (bufsize < *bufuse + item->size + 5)
        {
          DList_closeStep(sheet,&sinfo);
          return SBUF_ERR_ERROR;
        }
        buf[(*bufuse)++] = item->type;
        sprintf(buf + *bufuse,"%04d",item->size);
        *bufuse += 4;
        memcpy(buf+*bufuse,item->data,item->size);
        *bufuse += item->size;
        break;
      default:
        DList_closeStep(sheet,&sinfo);
        return SBUF_ERR_ERROR;
    }
  }
  
  DList_closeStep(sheet,&sinfo);
  
  return 0;
}

int StreamToServiceBuf(SERVICEBUF *sbuf,int bufsize,int *bufuse,char *buf)
{
  int        rv;
  DataItem   item;
  int        value_i;
  double     value_d;
  char      *data;
  char       tmpbuf[17];
  Sheet     *sheet;

  sheet = (Sheet *)sbuf->buff;

  *bufuse = 3;
  while( ( bufsize - *bufuse ) > 0)
  {
    memset(&item,0,sizeof(DataItem));
    memcpy(item.ID,buf + *bufuse,SERVICE_NAMELEN);
    *bufuse += SERVICE_NAMELEN;
    item.type = buf[(*bufuse)++];
    memset(tmpbuf,0,sizeof(tmpbuf));

    switch(item.type)
    {
      case FLDTYPE_INT:
        memcpy(tmpbuf,buf + *bufuse,12);
        value_i = atoi(tmpbuf);
        data = (char *)&value_i;
        item.size = sizeof(int);
        item.type = FLDTYPE_INT;
        (*bufuse) += 12;
        break;
      case FLDTYPE_DOUBLE:
        memcpy(tmpbuf,buf + *bufuse,16);
        sscanf(tmpbuf,"%lf",&value_d);
        data=(char *)&value_d;
        item.size = sizeof(double);
        item.type = FLDTYPE_DOUBLE;
        (*bufuse) += 16;
        break;
      case FLDTYPE_STR:
        memcpy(tmpbuf,buf + *bufuse,4);
        item.size = atoi(tmpbuf);
        (*bufuse) += 4;
        data = buf + *bufuse;
        (*bufuse) += item.size;
        break;
      case FLDTYPE_IMG:
        memcpy(tmpbuf,buf + *bufuse,4);
        item.size = atoi(tmpbuf);
        (*bufuse) += 4;
        data = buf + *bufuse;
        (*bufuse) += item.size;
        break;
      default:
        return -1;
    }

    rv = Sheet_add(sheet,item.ID,item.type,item.size,data);
    if ( rv != 0)
    {
      return rv;
    }
    
  }
  if ( bufsize != *bufuse )
  {
    return SBUF_ERR_BUFOVER;
  }

  return 0;
}

int ServiceBufDebug(char *filename,char *title,SERVICEBUF *sbuf)
{
  int              rv,i;
  DList_stepInfo   sinfo;
  DataItem        *item;
  double           value_d;
  int              count = 1;
  FILE            *out;
  Sheet           *sheet;

  sheet = (Sheet *)sbuf->buff;

  out = fopen((char*)GetLogFileName(filename),"a");
  if (out == NULL)
  {
    return -1;
  }

  rv = DList_openStep(sheet,&sinfo);
  if (rv != 0)
  {
    fclose(out);
    return rv;
  }

  fprintf(out,"%s(nums=%d):\n",title,sheet->cnt);

  while ( DList_fetchStep(sheet,&sinfo,(void **)&item) == 0 )
  {
    switch(item->type)
    {
      case FLDTYPE_INT:
        fprintf(out,"%d: %-16s  INT: %d\n",count++,item->ID,*(int *)item->data);
        break;
      case FLDTYPE_DOUBLE:
        memcpy(&value_d,item->data,sizeof(double));
        fprintf(out,"%d: %-16s  DOUBLE: %lf\n",count++,item->ID,value_d);
        break;
      case FLDTYPE_STR:
        fprintf(out,"%d: %-16s  STR(len=%d): %s\n",count++,item->ID,item->size,item->data);
        break;
      case FLDTYPE_IMG:
        fprintf(out,"%d: %-16s  IMG(len=%d): ",count++,item->ID,item->size);
        for(i = 0 ;i < item->size - 1;i++)
          fprintf(out,"%02x ",item->data[i] & 0xff);
        fprintf(out,"%02x\n",item->data[item->size - 1] & 0xff);
        break;
      default:
        fprintf(out,"TYPE ERROR !! %d ID=%s type=%d\n",count++,item->ID,item->type);
        break;
    }
  }

  DList_closeStep(sheet,&sinfo);

  fclose(out);
  
  return 0;
}

