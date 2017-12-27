#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"
#include "note.h"
#include "note_queue.h"
#include "note_error.h"
#include "hlist.h"
#include "d_malloc.h"
#include "note_server.h"

typedef struct _note_tables_
{
  char   *index_str;
  char   *value_str;
  int    value_size;
}NoteTables;

static int HASHLISTSIZE = 4497;

static HList  kList;

static char NoteCtrlFileName[128] = {'I','B','N','O','T','E','\0'};

int NoteTablesCmp(void *b1,void *b2)
{
  if(b1 == NULL & &b2 !=NULL) 
  {
    return -1;
  }

  if(b1 != NULL & &b2 ==NULL) 
  {
    return 1;
  }

  return(strcmp( ((NoteTables *)b1)->index_str, ((NoteTables *)b2)->index_str) );
}

int NoteTablesHashValue(void *p)
{
  NoteTables    *tables;
  unsigned int  hashValue;

  tables = (NoteTables *)p;


  hashValue = CalcHashnr((unsigned char *)(tables->index_str),strlen(tables->index_str));

  return (int) (hashValue % HASHLISTSIZE);
}

NoteTables * NoteTablesInit(char *index,char *value)
{
  NoteTables *tables ;

  tables = (NoteTables *)d_malloc(sizeof(NoteTables));
  if(tables == NULL)
  {
    return NULL;
  }

  memset(tables,0,sizeof(NoteTables));

  tables->index_str = (char *)d_malloc(strlen(index) + 1);
  if (tables->index_str == NULL)
  {
    d_free(tables);
    return NULL;
  }
  memset(tables->index_str,0,sizeof(strlen(index) + 1));
  strcpy(tables->index_str,index);
  tables->value_size = strlen(value) + 1;
  tables->value_str = (char *)d_malloc(tables->value_size);
  if (tables->value_str != NULL)
  {
    memset(tables->value_str,0,tables->value_size);
    strcpy(tables->value_str,value);
  }
  else
  {
    d_free(tables->index_str);
    d_free(tables);
    tables->value_size = 0;
    return NULL;
  }

  return tables;
}

int NoteTablesDel(void *b)
{
  if(b != NULL)
  {
    if ( ((NoteTables *)b)->index_str != NULL )
    {
      d_free(((NoteTables *)b)->index_str); 
    }
    if ( ((NoteTables *)b)->value_str != NULL )
    {
      d_free(((NoteTables *)b)->value_str);
    }
    d_free(b);
  }

  return 0;
}


static int add(Cmd *cmd)
{
  int         rv;
  NoteTables  *tables ;

  tables = NoteTablesInit(cmd->index_str,cmd->value_str);
  if(tables == NULL)
  {
    cmd->ans = NOTE_NOMEMORY;
    return cmd->ans;
  }

  rv = HList_add(&kList,(void *)tables);
  if (rv)
  {
    NoteTablesDel(tables);
    cmd->ans = NOTE_NOMEMORY;
    return cmd->ans;
  }
  cmd->ans = 0;
  return 0;
}

static int get(Cmd *cmd)
{
  int  rv;
  NoteTables  *index,*found;

  index = NoteTablesInit(cmd->index_str,"");
  if(index == NULL)
  {
    cmd->ans = NOTE_NOMEMORY;
    return cmd->ans;
  }

  rv = HList_find(&kList,(void *)index,(void **)&found);
  if(rv)
  {
    NoteTablesDel(index);
    cmd->ans = NOTE_NOTFOUND;
    return cmd->ans;
  }
  strncpy(cmd->value_str,found->value_str,found->value_size);
  cmd->ans = 0;
  
  return 0;
}

static int put(Cmd *cmd)
{
  int         rv,size;
  NoteTables  *index,*found;
  char        *new_value;

  index = NoteTablesInit(cmd->index_str,"");
  if(index == NULL)
  {
    cmd->ans = NOTE_NOMEMORY;
    return cmd->ans;
  }

  rv = HList_find(&kList,(void *)index,(void **)&found);
  if(rv)
  {
    NoteTablesDel(index);
    cmd->ans = NOTE_NOTFOUND;
    return cmd->ans;
  }

  size = strlen(cmd->value_str) + 1;
  if (size > NOTEBUFSIZE + 1)
  {
    size = NOTEBUFSIZE + 1;
  }
  
  if (size > found->value_size)
  {
    new_value=(char *)d_malloc(size);
    if (new_value == NULL)
    {
      NoteTablesDel(index);
      cmd->ans = NOTE_NOMEMORY;
      return cmd->ans;
    }
    d_free(found->value_str);
    found->value_str  = new_value;
    found->value_size = size;
  }
  memset(found->value_str,0,size);
  strcpy(found->value_str,cmd->value_str);
  cmd->ans = 0;
  return 0;
}

static int clean()
{
  HList_clean(&kList,&NoteTablesDel);
  return 0;
}

static int sysfree()
{
  HList_free(&kList,&NoteTablesDel);
  return 0;
}


static int skipspace(int *n,char *buf)
{
  while(buf[*n] != '\0')
  {
    switch(buf[*n])
    {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        ++(*n); 
        break;
      default:
        return 0;
    }
  }
  return 0;
}

static int copystr(int *n,char *buf,char *str)
{
  while(buf[*n] != '\0')
  {
    switch(buf[*n])
    {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        *str = '\0'; 
        return 0;
      default:
        *(str++) = buf[(*n)++];
    }
  }
  *str = '\0';
  return 0;
}

static int getstr(int *n,char *buf,char *str)
{
  skipspace(n,buf);
  copystr(n,buf,str);
  return 0;
}

static int isspaceline(char *line)
{
  while(*line != '\0')
  {
    switch(*line)
    {
      case '\r':
      case '\n':
      case '#': 
        return 1;
      case ' ':
      case '\t':
        ++line;
        break;
      default:
        return 0;
    }
  }
  return 0;
}

static int getdata(char *line,char *name,int indexN,char *index_str,char *value_str)
{
  int   n = 0;
  int   m = 0;
  int   i;
  char  str[256];
  int   rv;

  rv = note_putstr(&m,index_str,name);
  if (rv != 0)
  {
    return rv;
  }

  for(i = 0; i < indexN; ++i)
  {
    memset(str,0,sizeof(str));
    getstr(&n,line,str);
    if (str[0] == '\0')
    {
      break;
    }
    rv = note_putstr(&m,index_str,str);
    if (rv != 0)
    {
      return rv;
    }
  }

  if (i != indexN)
  {
    return NOTE_DATALOST;
  }
  
  m = 0;
  
  while(line[n] != '\0')
  {
    memset(str,0,sizeof(str));
    getstr(&n,line,str);
    if (str[0] == '\0')
    {
      break;
    }
    rv = note_putstr(&m,value_str,str);
    if (rv != 0)
    {
      return rv;
    }
  }
  return 0;
}

static int loadfile(char *name,int indexN,char *filename)
{
  FILE   *in;
  char   line[256];
  char   *p;
  Cmd    cmd;
  int    rv = 0;
  int    lineNo = 0;
  extern int errno;

  in = fopen((char*)GetEtcFileName(filename),"r");
  if (in ==NULL)
  {
    logger(__FILE__,__LINE__,"loadfile(%s) => fopen() errno %d,%s",filename,errno,strerror(errno));
    return NOTE_FILEERROR;
  }

  while( !feof(in) )
  {
    memset(line,0,sizeof(line));
    ++lineNo;
    p = fgets(line,255,in);
    if (p == NULL)
    {
      break;
    }
    if (isspaceline(line))
    {
      continue;
    }

    memset(&cmd,0,sizeof(cmd));
    rv = getdata(line,name,indexN,cmd.index_str,cmd.value_str);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"loadfile(%s) =>getdata() error %d",filename,rv);
      goto E;
    }
    
    rv = add(&cmd);
    if (rv != 0)
    {
      logger(__FILE__,__LINE__,"loadfile(%s) =>addcmd() error %d",filename,rv);
      goto E;
    }

  }
E:
  fclose(in);
  if (rv != 0)
  {
    logger(__FILE__,__LINE__,"file [%s] line [%d] err[%d]", filename,lineNo,rv);
  }

  return rv;
  
}

static int load(Cmd *cmd)
{
  FILE   *ctrlfile;
  char   line[256];
  char   name[32],filename[128];
  int    indexN;
  int    n;
  int    rv;
  char   *p;
  int    lineNo=0;
  extern int errno;

  clean();
  
  p = (char*)getenv(ENV_NOTECTRLFILE);
  if(p == NULL)
  {
    cmd->ans = NOTE_FILEERROR;
    logger(__FILE__,__LINE__,"环境变量 NOTECTRLFILE 没有设置");
    return -1;
  }

  ctrlfile = fopen((char*)GetEtcFileName(p),"r");
  if (ctrlfile == NULL)
  {
    cmd->ans = NOTE_FILEERROR;
    logger(__FILE__,__LINE__,"load(%s) => fopen() errno %d,%s",p,errno,strerror(errno));
    return cmd->ans;
  }

  while(!feof(ctrlfile))
  {
    lineNo++;
    memset(line,0,sizeof(line));
    p = fgets(line,255,ctrlfile);
    if (p == NULL)
    {
      break;
    }

    memset(name,0,sizeof(name));
    memset(filename,0,sizeof(filename));
    n=sscanf(line," %31s %d %127s",name,&indexN,filename);
    if (name[0] == '\0'||name[0] == '#')
      continue;

    rv = loadfile(name,indexN,filename);
    if (rv!=0)
    {
      cmd->ans = rv;
      fclose(ctrlfile);
      return cmd->ans;
    }
  }

  fclose(ctrlfile);
  cmd->ans=0;
  return 0;
}

static int mainloop()
{
  int  rv;
  int  g = 1;
  Cmd  cmd;

  while(g)
  {
    memset(&cmd,0,sizeof(Cmd));
    rv = note_queue_recv_cmd(&cmd);
    if ( rv != 0 )
    {
      g = 0;
    }
    else
    {
      switch (cmd.cmd)
      {
        case NOTE_GET:
          get(&cmd);
          break;
        case NOTE_PUT:
          put(&cmd);
          break;
        case NOTE_RELOAD:
          load(&cmd);
          break;
        case NOTE_EXIT:
          cmd.ans = 0;
          g = 0;
          break;
        default:
          cmd.ans = -1;
          break;
      }

      rv = note_queue_send_ans(&cmd);
      if ( rv != 0 )
      {
        g = 0;
      }
    }
  }

  return rv;
}

void signal_term(int sig)
{
  note_queue_remove();
  sysfree();
  exit(1);
}

int main()
{
  int         rv;
  Cmd         cmd;
  pid_t       pid;
  int         cmd_p[2];
  char        *env;
  extern char *CurrentProgram;

  CurrentProgram = "IBnote";
  
  env = getenv(ENV_HASHLISTSIZE);
  if(env == NULL)
  {
    HASHLISTSIZE = 9997;
  }
  else
  {
    HASHLISTSIZE = atoi(env);
  }
  HList_new(&kList,&NoteTablesHashValue,&NoteTablesCmp,HASHLISTSIZE);

  memset(&cmd,0,sizeof(cmd));

  rv = load(&cmd);
  if (rv)
  {
    printf("\n");
    printf(" 系统错误:装载参数数据失败,请检查.\n");
    printf(" 原    因:环境变量 NOTECTRLFILE 是否设置或者配置文件%s是否存在,配置文件是否正确.\n",getenv(ENV_NOTECTRLFILE));
    printf("\n");
    exit(-1);
  }

  signal(SIGCLD,SIG_IGN);
  
  pid = fork();

  if (pid < 0)
  {
    exit(-1);
  }
  if(pid > 0)
  {
    return 0;
  }
  
  setsid();
  signal(SIGINT,SIG_IGN);
  signal(SIGHUP,SIG_IGN);
  signal(SIGPIPE,SIG_IGN );
  signal(SIGTERM,signal_term);

  memset(&cmd,0,sizeof(cmd));
  rv = note_queue_init(1);
  if (rv)
  {
    printf("\n");
    printf("  系统错误:创建系统队列失败,请检查.\n");
    printf("  原    因:IBnote进程可能已经启动,或者IBnote的控制文件不存在.\n");
    sysfree();
    exit(-1);
  }

  mainloop();

  note_queue_remove();

  return 0;
}
