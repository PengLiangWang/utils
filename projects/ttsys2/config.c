#include <string.h>
#include <stdio.h>
#include "config.h"
#include "hlist.h"

typedef struct
{
  char   *section;
  char   *key;
  char   *value;
}TConfigProperty;

typedef struct 
{
  char  *section;
  TList  list;
}TSection;

struct TConfig
{
  char        *file;
  HList        list;
  TList        sections;
};

static int GConfigHashTableSize = 101;

static int ConfigHashValueFunc(void *a,int maxHashValue)
{
  static char  Key[256];
  unsigned int hashValue;
  TConfigProperty *p = (TConfigProperty *)a;

  memset(Key,0,sizeof(Key));
  sprintf(Key,"%s%s",p->section,p->key);
  hashValue =  defaultGetHashValue((uchar *)Key,strlen(Key)) ;

  return (int) (hashValue % maxHashValue);
}

static int ConfigCompressFunc(void *b1,void *b2)
{
  TConfigProperty *p1 = (TConfigProperty *)b1 ,*p2 = (TConfigProperty *)b2;

  if ( b1 != NULL && b2 != NULL)
  {
    if ( strcmp(p1->section,p2->section) == 0 && strcmp(p1->key,p2->key) == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  return -1;
}

static TConfigProperty *ConfigPropertyAlloc(const char *section,const char *key,const char *value)
{
  TConfigProperty  *property;

  property = (TConfigProperty *)malloc(sizeof(TConfigProperty));
  memset(property,0,sizeof(TConfigProperty));
  property->section = strdup(section);
  property->key = strdup(key);
  property->value = strdup(value);

  return property;
}

static int  ConfigPropertyFree(void  *data)
{
  TConfigProperty  *property = (TConfigProperty  *)data;

  if ( property != NULL )
  {
    free(property->section);
    free(property->key);
    free(property->value);
    free(property);
  }
  property = NULL;

  return TTS_SUCCESS;
}

static TProperty *ConfigKeyAlloc(char *key,char *value)
{
  TProperty  *pro;

  pro = (TProperty *)malloc(sizeof(TProperty));
  memset(pro,0,sizeof(TProperty));
  pro->key = strdup(key);
  pro->value = strdup(value);

  return pro;
}

static int  ConfigKeyFree(void *data)
{
  TProperty  *pro = (TProperty *)data;
  free(pro->key);
  free(pro->value);
  free(pro);
  return TTS_SUCCESS;

}

static TSection * ConfigSectionAlloc(char *section,TList **list)
{
  TSection *sec = malloc(sizeof(TSection));
  memset(sec,0,sizeof(TSection));
  sec->section = strdup(section);
  *list = ListNew(&(sec->list));

  return sec;
}

static int  ConfigSectionFree(void *data)
{
  TSection  *sec = (TSection  *)data;
  
  free(sec->section);
  ListClear(&(sec->list),ConfigKeyFree);
  free(sec);
  return TTS_SUCCESS;
}


int  ConfigClose(TConfig **config)
{
  if ( *config == NULL )
  {
    return TTS_ENULL;
  }

  HListFree(&((*config)->list),ConfigPropertyFree);
  ListClear(&((*config)->sections),ConfigSectionFree);
  free((*config)->file);
  free((*config));
  *config = NULL;

  return TTS_SUCCESS;
}


static char  GSection[256];
static char  GKey[256];
static char  GValue[256];

static int  ConfigParserSection(const char *line)
{
  char  *ss,*se,*te;
  
  ss = strstr(line,"[");
  se = strstr(line,"]");
  te = strstr(line,"=");

  if (ss == NULL || se == NULL)
  {
    return TTS_ENULL;
  }

  if (te != NULL)
  {
    return TTS_ENULL;
  }
  
  memset(GSection,0,sizeof(GSection));

  strncpy(GSection,ss + 1,se - ss - 1);

  return TTS_SUCCESS;
}



static int  ConfigParserParameter(const char *line)
{
  char       *ss;

  ss = strstr(line,"=");
  if (ss == NULL)
  {
    return TTS_ENULL;
  }
  memset(GKey,0,sizeof(GKey));
  memset(GValue,0,sizeof(GValue));

  strncpy(GKey,line,ss - line);
  strncpy(GValue,ss + 1,strlen(line) - (ss - line));

  m_trim(GKey);
  m_trim(GValue);
  
  return TTS_SUCCESS;
}

int  ConfigLoad(const char *filename,TConfig **config)
{
  TConfig          *new ;
  FILE             *fp;
  char              line[256];
  char             *ptr;
  int               rv;
  int               flag = 0;
  TConfigProperty  *property;
  TList            *sList,*kList;
  TProperty        *KeyListItem;

  new = (TConfig *)malloc(sizeof(TConfig));
  if ( new == NULL )
  {
    return TTS_ENULL;
  }

  memset(new,0,sizeof(TConfig));

  new->file = strdup(filename);

  HListNew(&(new->list),ConfigHashValueFunc,ConfigCompressFunc,GConfigHashTableSize);

  sList = ListNew(&(new->sections));

  *config = new;

  fp = fopen(new->file,"r");
  if (fp == NULL)
  {
    return TTS_ENONE;
  }

  while(!feof(fp))
  {
    memset(line,0,sizeof(line));
 
    if ( fgets(line,255,fp) == NULL )
    {
      break;
    }
    if (isspaceline(line) || line[0]==';' || line[0] == '#')
    {
      continue;
    }
    rv = ConfigParserSection((const char *)line);
    if (rv == TTS_SUCCESS)
    {
      TSection *section;

      flag = 1;
      section = ConfigSectionAlloc(GSection,&kList);
      if ( section == NULL )
      {
        ConfigClose(&new);
        return TTS_ENOMEM;
      }
      ListAdd(sList,section,NULL);
      continue;
    }
    else if (rv != TTS_SUCCESS && flag == 0)
    {
      continue;
    }
    
    rv = ConfigParserParameter((const char *)line);
    if (rv != TTS_SUCCESS)
    {
      continue;
    }
    
    property = ConfigPropertyAlloc(GSection,GKey,GValue);
    
    if ( property == NULL )
    {
      ConfigClose(&new);
      return TTS_ENOMEM;
    }

    KeyListItem = ConfigKeyAlloc(GKey,GValue);
    if ( KeyListItem == NULL )
    {
      ConfigClose(&new);
      return TTS_ENOMEM;
    }

    ListAdd(kList,KeyListItem,NULL);

    rv = HListAdd(&(new->list),(void *)property);
    if ( rv != TTS_SUCCESS )
    {
      ConfigClose(&new);
      return rv;
    }
  }

  fclose(fp);

  *config = new;

  return TTS_SUCCESS;
}

int  ConfigGetProperty(TConfig *config,char *section,char *key,char **data)
{
  TConfigProperty  *property,*found;
  int               rv;

  if ( config == NULL )
  {
    return TTS_EINVAL;
  }

  property = ConfigPropertyAlloc(section,key," ");
  if ( property == NULL )
  {
    return TTS_ENOMEM;
  }

  rv = HListFind(&(config->list),(void *)property,(void **)&found);
  if ( rv != TTS_SUCCESS )
  {
    ConfigPropertyFree((void *)property);
    return rv;
  }
  
  ConfigPropertyFree((void *)property);

  *data = strdup(found->value);

  return TTS_SUCCESS;
}

int  ConfigFree(char **data)
{
  if ( *data )
  {
    free(*data);
    *data = NULL;
  }

  return TTS_SUCCESS;
}

int  ConfigFreeSections(char ***sections)
{
  char  **ks;

  ks = *sections;
  free(ks);

  *sections = NULL;


  return TTS_SUCCESS;
}

int  ConfigFreePropertys(TProperty **pros)
{
  TProperty  *ks;

  ks = *pros;
  free(ks);

  *pros = NULL;

  return TTS_SUCCESS;
}

int  ConfigGetSections(TConfig *config,char ***sections,int *nums)
{
  int           rv;
  int           count;
  TList        *list;
  char         **ks;
  TListIndex   *index;
  TSection     *data;

  if ( config == NULL )
  {
    return TTS_EINVAL;
  }

  list = &(config->sections);

  count = ListGetCount(list);

  if ( count == 0 )
  {
    *nums = 0;
    return TTS_SUCCESS;
  }
  
  *nums = count;

  ks = malloc(sizeof(char *) * count);
  
  memset(ks,0,sizeof(char *) * count);

  count = 0;

  for (index = ListFirst(list) ; index != NULL ; index = ListNext(index))
  {
    ListThis(index,(void **)&data);
    if ( data != NULL )
    {
      ks[count++] = data->section;
    }
  }

  *sections = ks ;

  return TTS_SUCCESS;
}

int  ConfigGetPropertys(TConfig *config,char *section,TProperty **pros,int *nums)
{
  int       rv;
  int           count;
  TList        *list,*kList = NULL;
  TProperty    *property,*pdata;
  TListIndex   *index;
  TSection     *data;
  
  if ( config == NULL )
  {
    return TTS_EINVAL;
  }

  list = &(config->sections);

  for (index = ListFirst(list) ; index != NULL ; index = ListNext(index))
  {
    ListThis(index,(void **)&data);
    if ( data != NULL )
    {
      if ( strcmp(data->section,section) == 0)
      {
        kList = &(data->list);
        break;
      }
    }
  }

  if ( kList == NULL )
  {
    *nums = 0;
    return TTS_SUCCESS;
  }

  count = ListGetCount(kList);
  if ( count == 0 )
  {
    *nums = 0;
    return TTS_SUCCESS;
  }

  *nums = count;

  property = (TProperty *)malloc(sizeof(TProperty) * count);

  memset(property,0,sizeof(TProperty) * count);

  count = 0;

  for (index = ListFirst(kList) ; index != NULL ; index = ListNext(index))
  {
    ListThis(index,(void **)&pdata);
    if ( pdata != NULL )
    {
      memcpy(&property[count++],pdata,sizeof(TProperty));
    }
  }

  *pros = property;

  return TTS_SUCCESS;
}


int  ConfigGetSubProperty(TConfig *config,char *section,char *key,char *subkey,char **data)
{
  int        rv ;
  char      *keydata;
  char      *hptr,*eptr,*kptr,*vptr,*tptr;
  char     **arr;
  int        i = 0,item_num = 0,item_alloc_num = 1,found = 0;

  rv = ConfigGetProperty(config,section,key,&keydata);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }
  
  hptr = strchr(keydata,'"');
  eptr = strrchr(keydata,'"');

  *eptr = 0;
  hptr++;
  
  arr = (char **)NULL;

  if ( ( tptr = strtok(hptr, ";") ) )
  {
    do
    {
      arr = (char **)realloc(arr,sizeof(char *) * (item_alloc_num + 1));
      arr[item_num] = strdup(tptr);
      item_num++;
      item_alloc_num++;
    }
    while ( ( tptr = strtok(NULL, ";") ) );
  }

  if ( item_num == 0 )
  {
    ConfigFree((char **)&keydata);
    return TTS_NOTFOUND;
  }

  for ( i = 0 ; i < item_num ; i++)
  {
    kptr = strtok(arr[i],"=");
    if ( kptr == NULL)
    {
      for ( i = 0; i < item_num; i++ )
      {
        free(arr[i]);
      }
      free(arr);
      ConfigFree((char **)&keydata);
      return TTS_NOTFOUND;
    }
    kptr = (char *)m_trim(kptr);
    vptr = strtok(NULL,"=");

    if ( strcmp(subkey,kptr) == 0)
    {
      if ( vptr == NULL )
      {
        *data = NULL;
        rv = TTS_ENULL;
      }
      else
      {
        vptr = (char *)m_trim(vptr);
        rv = TTS_SUCCESS;
        *data = strdup(vptr);
      }

      found  = 1;
      break;
    }
  }
  
  if ( found == 0)
  {
    *data = NULL;
    rv = TTS_NOTFOUND;
  }

  for ( i = 0; i < item_num; i++ )
  {
    free(arr[i]);
  }

  free(arr);
  ConfigFree((char **)&keydata);

  return rv;
}


