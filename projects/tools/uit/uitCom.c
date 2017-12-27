#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static FILE *c_file=NULL;
static FILE *h_file=NULL;
static char c_filename[128];
static char h_filename[128];
static FILE *in_file;
static char *base;
static char *uit_file;

/**************************** MALLOC and FREE ************************/

static void *MALLOC(size_t size)
{
  void  *p;

  p=malloc(size);
  if (p==NULL)
  {
    fprintf(stderr,"NO MEMORY!\n");
    exit(1);
  };
  memset(p,0,size);
  return(p);
}

static void FREE(void *p)
{
  if (p!=NULL)
    free(p);
}

/*************************** INPUT FILE *********************************/
#define BUFLEN    1024

typedef struct
  {
    char  *str;
    int keyword;
  } Str;

static  int in_cc;
static  int in_cc_flag=0;
static  int lineCnt=1;

static int GetChar(FILE *in)
{
  if (in_cc_flag!=0)
  {
    in_cc_flag=0;
    return(in_cc);
  };

  in_cc=fgetc(in);
  if (in_cc=='\n')
    ++lineCnt;
  return(in_cc);
}

static int UGetChar(int cc)
{
  in_cc=cc;
  in_cc_flag=1;
  if (cc=='\n')
    --lineCnt;
  return(in_cc);
}

static char *StrDup(char *str)
{
  char  *nstr;

  nstr=MALLOC(strlen(str)+1);

  strcpy(nstr,str);
  return(nstr);
}

static char line_buf[BUFLEN];

static char *GetLine(FILE *in)
{
  int n;
  int c;

  n=0;
  memset(line_buf,0,BUFLEN);

  while(n<BUFLEN-1)
  {
    c=GetChar(in);
    if (c==EOF||c=='\n')
      break;

    line_buf[n++]=(char)c;
  };

  line_buf[n]='\0';
  if (n==0&&c==EOF)
    return(NULL);

  return(line_buf);
}

#define K_END     101
#define K_MENU      102
#define K_SUBMENU   103
#define K_BORDER    104
#define K_TITLE     105
#define K_TITLEWIN    106
#define K_MSGWIN    107
#define K_YES     108
#define K_NO      109
#define K_AT      110
#define K_SIZE      111
#define K_ID      112
#define K_REM     113
#define K_ITEM      114
#define K_TEXT      115
#define K_FORM      115
#define K_FIELD     116
#define K_HIDDEN    117
#define K_DISPLAYSIZE   118
#define K_DEC     119
#define K_RANGE     120
#define K_KEY     121
#define K_TYPE      122
#define K_NOTNULL   123
#define K_INPUTWIN    124
#define K_NOTZERO   125
#define K_FIXLEN    126
#define K_DISPLAYONLY   127
#define K_SUBMIT    128
#define K_PLUS      129
#define K_MINUS     130
#define K_NOTPLUS   131
#define K_NOTMINUS    132
#define K_HOTKEY    133

#define K_INT     1
#define K_UINT      2
#define K_SHORT     3
#define K_USHORT    4
#define K_DOUBLE    5
#define K_STR     6
#define K_PASSWD    7
#define K_YMD     8
#define K_HMS     9
#define K_SELECT    10
#define K_OPTION    11
#define K_NSTR      12
#define K_LONG      13
#define K_ULONG     14
#define K_BUTTON    15


static struct
  {
    char  *str;
    int keyword;
  } kList[]
  ={
    {"END",   K_END},
    {"MENU",  K_MENU},
    {"SUBMENU", K_SUBMENU},
    {"BORDER",  K_BORDER},
    {"TITLE", K_TITLE},
    {"TITLEWIN",  K_TITLEWIN},
    {"MSGWIN",  K_MSGWIN},
    {"YES",   K_YES},
    {"NO",    K_NO},
    {"AT",    K_AT},
    {"SIZE",  K_SIZE},
    {"ID",    K_ID},
    {"REM",   K_REM},
    {"ITEM",  K_ITEM},
    {"TEXT",  K_TEXT},
    {"FORM",  K_FORM},
    {"FIELD", K_FIELD},
    {"HIDDEN",  K_HIDDEN},
    {"DISPLAYSIZE", K_DISPLAYSIZE},
    {"DEC",   K_DEC},
    {"RANGE", K_RANGE},
    {"KEY",   K_KEY},
    {"TYPE",  K_TYPE},
    {"NOTNULL", K_NOTNULL},
    {"INPUTWIN",  K_INPUTWIN},
    {"NOTZERO", K_NOTZERO},
    {"FIXLEN",  K_FIXLEN},
    {"DISPLAYONLY", K_DISPLAYONLY},
    {"SUBMIT",  K_SUBMIT},
    {"PLUS",  K_PLUS},
    {"MINUS", K_MINUS},
    {"NOTPLUS", K_NOTPLUS},
    {"NOTMINUS",  K_NOTMINUS},
    {"HOTKEY",  K_HOTKEY},
  
    {"INT",   K_INT},
    {"UINT",  K_UINT},
    {"SHORT", K_SHORT},
    {"USHORT",  K_USHORT},
    {"DOUBLE",  K_DOUBLE},
    {"STR",   K_STR},
    {"PASSWD",  K_PASSWD},
    {"YMD",   K_YMD},
    {"HMS",   K_HMS},
    {"SELECT",  K_SELECT},
    {"OPTION",  K_OPTION},
    {"NSTR",  K_NSTR},
    {"LONG",  K_LONG},
    {"ULONG", K_ULONG},
    {"BUTTON",  K_BUTTON},

    {NULL,0}
  };

static int CheckKeyWord(char *str)
{
  int i;

  for(i=0; kList[i].str!=NULL; ++i)
    if (strcmp(kList[i].str,str)==0)
      return(kList[i].keyword);
  return(-1);
}
static char *GetKeywordName(int keyword)
{
  int i;
  for(i=0; kList[i].str!=NULL; ++i)
    if (kList[i].keyword==keyword)
      return(kList[i].str);
  return(NULL);
}

static int GetStr(FILE *in,Str *s)
{
  int n;
  int stat;
  int c;
  int go_on=1;

  n=0;
  memset(line_buf,0,BUFLEN);

  stat=0;
  while(go_on)
  {
    if (n>=BUFLEN-1)
      return(-1);

    c=GetChar(in);

    switch(stat)
    {
      case 0:
        if (c==EOF)
          return(-1);
        else if (c==' '||c=='\t'||c=='\n'||c=='\r')
          break;
        else if (c=='#')
          GetLine(in);
        else if (c=='"')
          stat=10;
        else
        {
          line_buf[n++]=(char)c;
          stat=20;
        };
        break;

      case 10:
        if (c==EOF)
          return(-1);
        else if (c=='\\')
          stat=11;
        else if (c=='"')
          go_on=0;
        else  line_buf[n++]=(char)c;
        break;
      case 11:
        if (c==EOF)
          return(-1);

        line_buf[n++]=(char)c;
        break;

      case 20:
        if (c=='#')
        {
          UGetChar(c);
          go_on=0;
        }
        else if (c==' '||c=='\t'||c=='\n'||c=='\r')
          go_on=0;
        else
          line_buf[n++]=(char)c;
        break;
      default:
        return(-1);
    };
  };

  line_buf[n]='\0';

  s->str=line_buf;
  s->keyword=CheckKeyWord(s->str);

  return(0);
}
/*************************** Get **************************************/
static int GetYesNo(FILE *in,int *option)
{
  int r;
  Str s;

  r=GetStr(in,&s);
  if (r!=0)
    return(r);

  switch(s.keyword)
  {
    case K_YES: *option=1;  break;
    case K_NO:  *option=0;  break;
    default:
      r=-1;
  };

  return(r);
}
static int GetNum(FILE *in,int *num)
{
  int r;
  Str s;
  int i;

  r=GetStr(in,&s);
  if (r!=0)
    return(r);

  if (s.str[0]=='\0')
    return(-1);

  for(i=0; s.str[i]!='\0'; ++i)
    if (s.str[i]<'0'||s.str[i]>'9')
      return(-1);
      
  *num=atoi(s.str);
  return(0);
}
/***************************** TEXT ***********************************/
typedef struct Text_t
  {
    struct Text_t   *next;
    char      *str;
  } Text;
static int ClearTextList(Text *pt)
{
  Text  *pt2;

  while(pt!=NULL)
  {
    pt2=pt->next;
    FREE(pt->str);
    FREE(pt);
    pt=pt2;
  };
  return(0);
}

static int GetTextList(FILE *in,Text **p)
{
  int r;
  Str s;
  int g=1;
  char  *line;
  char  *mark;

  if (*p!=NULL)
    return(-1);

  r=GetStr(in,&s);
  if (r!=0)
    return(r);
  
  mark=StrDup(s.str);

  while(g&&r==0)
  {
    line=GetLine(in);
    if (line==NULL)
      return(-1);

    if (strcmp(line,mark)==0)
      g=0;
    else
    {
      *p=(Text *)MALLOC(sizeof(Text));

      (*p)->str=StrDup(line);
      p=&(*p)->next;
    };
  };

  FREE(mark);

  return(r);
} 
  
/**************************** MENU ************************************/
typedef struct MenuItem_t
  {
    struct MenuItem_t *next;

    char      *title;
    char      *rem;
    char      *name;
    int     p_row,p_col;
    int     ID;
    char      key;
  } MenuItem;
typedef struct MenuHotKey_t
  {
    struct MenuHotKey_t *next;

    char      *name;
    int     ID;
    char      key;
  } MenuHotKey;
static struct
  {
    char    *name;
    char    *title;
    int   p_row,p_col,s_row,s_col;
    int   border,titlewin,msgwin;

    Text    *t_head;
    MenuItem  *i_head,*i_end;
    MenuHotKey  *k_head,*k_end;
  } menu;

static int ClearMenu()
{
  MenuItem  *p,*p2;
  MenuHotKey  *pk,*pk2;

  FREE(menu.name);
  FREE(menu.title);

  ClearTextList(menu.t_head);

  p=menu.i_head;
  while(p!=NULL)
  {
    p2=p->next;
    FREE(p->title);
    FREE(p->name);
    FREE(p->rem);
    FREE(p);

    p=p2;
  };
  pk=menu.k_head;
  while(pk!=NULL)
  {
    pk2=pk->next;
    FREE(pk->name);
    FREE(pk);

    pk=pk2;
  };

  menu.name=NULL;
  menu.title=NULL;
  menu.p_row=0;menu.p_col=0;
  menu.s_row=0;menu.s_col=0;
  menu.border=1;
  menu.titlewin=1;
  menu.msgwin=1;
  menu.t_head=NULL;
  menu.i_head=NULL;menu.i_end=NULL;
  menu.k_head=NULL;menu.k_end=NULL;
  return(0);
}
/******************************* PRINT TEXT *******************************/
static int PrintString(FILE *out,char *s)
{
  int n;
  int i;
  int m;
  int j;

  if (s==NULL)
  { fprintf(out,"NULL");
    return(0);
  };

  fprintf(out,"\"");
  for(i=0,n=0; s[i]!='\0'; ++i)
  {
    if (s[i]=='"')
    { n++;fprintf(out,"\\\"");  }
    else if (s[i]=='\\')
    { n++;fprintf(out,"\\\\");  }
/**
    else if (s[i]=='%')
    { n++;fprintf(out,"%%%%");  }
**/
    else if (s[i]=='\t')
    {
      m=8-n%8;
      for(j=0; j<m; ++j)
        fprintf(out," ");
      n+=m;
    }
    else
    {
      n++;fprintf(out,"%c",s[i]);
    };
  };
  fprintf(out,"\"");

  return(0);
}

static int PrintTextList(FILE *out,char *unit,char *name,Text *list)
{
  fprintf(out,"static char *%s_%s[]={\n",unit,name);
  while(list!=NULL)
  {
    fprintf(out,"\t");
    PrintString(out,list->str);
    fprintf(out,",\n");
    list=list->next;
  };
  fprintf(out,"\tNULL};\n");
  return(0);
}
/******************************* PRINT MENU ITEM **************************/
static int PrintMenuItemList(FILE *out,char *unit,char *name,MenuItem *list)
{
  fprintf(out,"static UitMenuItemList %s_%s[]={\n",unit,name);
  while(list!=NULL)
  {
    fprintf(out,"\t{");
    PrintString(out,list->title);
    fprintf(out,",");
    PrintString(out,list->rem);
    fprintf(out,",");
    if (list->name==NULL||list->name[0]=='\0')
      fprintf(out,"NULL");
    else fprintf(out,"&UitMenu_%s",list->name);
    fprintf(out,",%d,%d,%d,",list->p_row,list->p_col,list->ID);
    if (list->key=='\0')
      fprintf(out,"'\\0'},\n");
    else  fprintf(out,"'%c'},\n",list->key);
  
    list=list->next;
  };
  fprintf(out,"\t{NULL,NULL,NULL,0,0,0,'\\0'}\n\t};\n");

  return(0);
}
/******************************* PRINT MENU HOT KEY **************************/
static int PrintMenuHotKeyList(FILE *out,char *unit,char *name,MenuHotKey *list)
{
  fprintf(out,"static UitMenuHotKeyList %s_%s[]={\n",unit,name);
  while(list!=NULL)
  {
    fprintf(out,"\t{");
    if (list->key=='\0')
      fprintf(out,"'\\0'");
    else  fprintf(out,"'%c'",list->key);

    fprintf(out,",%d",list->ID);
    if (list->name==NULL||list->name[0]=='\0')
      fprintf(out,",NULL},\n");
    else fprintf(out,",&UitMenu_%s},\n",list->name);
  
    list=list->next;
  };
  fprintf(out,"\t{'\\0',0,NULL}\n\t};\n");

  return(0);
}
/******************************* PRINT MENU ******************************/
static int PrintMenuDef()
{
  if (menu.t_head!=NULL)
    PrintTextList(c_file,"UitMenuText",menu.name,menu.t_head);
  PrintMenuItemList(c_file,"UitMenuItem",menu.name,menu.i_head);
  if (menu.k_head!=NULL)
    PrintMenuHotKeyList(c_file,"UitMenuHotKey",
            menu.name,menu.k_head);

  fprintf(c_file,"UitMenu UitMenu_%s={\n",menu.name);
  fprintf(c_file,"\t");
  PrintString(c_file,menu.name);
  fprintf(c_file,",");
  PrintString(c_file,menu.title);
  fprintf(c_file,",\n\tUitMenuItem_%s,\n",menu.name);
  if (menu.k_head!=NULL)
    fprintf(c_file,"\tUitMenuHotKey_%s,\n",menu.name);
  else  fprintf(c_file,"\tNULL,\n");
  fprintf(c_file,"\t%d,%d,%d,%d,\n",
      menu.p_row,menu.p_col,
      menu.s_row,menu.s_col);
  if (menu.t_head!=NULL)
    fprintf(c_file,"\tUitMenuText_%s,\n",menu.name);
  else  fprintf(c_file,"\tNULL,\n");
  fprintf(c_file,"\tUIT_BORDER_%s|UIT_TITLEWIN_%s|UIT_MSGWIN_%s\n\t};\n",
      menu.border==0?"NO":"YES",
      menu.titlewin==0?"NO":"YES",
      menu.msgwin==0?"NO":"YES");
  fprintf(c_file,"\n");
  fprintf(h_file,"extern UitMenu UitMenu_%s;\n",menu.name);
  return(0);
}

/******************************* READ MENU ********************************/
static int GetMenuItem(FILE *in,char *name)
{
  int   r=0;
  MenuItem  *item;
  Str   s;
  int   g=1;

  item=(MenuItem *)MALLOC(sizeof(MenuItem));

  item->name=StrDup(name);
  if (menu.i_head==NULL)
  {
    menu.i_head=item;
    menu.i_end=item;
    item->next=NULL;
  }
  else
  {
    menu.i_end->next=item;
    menu.i_end=item;
    item->next=NULL;
  };

  while(g&&r==0)
  {
    r=GetStr(in,&s);
    if (r!=0)
      break;

    switch(s.keyword)
    {
      case K_END:
        r=0;g=0;  break;
      case K_TITLE:
        if (item->title!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            item->title=StrDup(s.str);
        };
        break;
      case K_REM:
        if (item->rem!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            item->rem=StrDup(s.str);
        };
        break;
      case K_AT:
        r=GetNum(in,&item->p_row);
        if (r==0)
          r=GetNum(in,&item->p_col);
        break;
      case K_ID:
        r=GetNum(in,&item->ID);
        break;
      case K_KEY:
        r=GetStr(in,&s);
        if (r==0)
          item->key=s.str[0];
        break;
      default:
        r=-1;
    };
  };

  return(r);
}
static int GetMenuHotKey(FILE *in)
{
  int   r=0;
  MenuHotKey  *hotkey;
  Str   s;
  int   g=1;

  hotkey=(MenuHotKey *)MALLOC(sizeof(MenuHotKey));

  if (menu.k_head==NULL)
  {
    menu.k_head=hotkey;
    menu.k_end=hotkey;
    hotkey->next=NULL;
  }
  else
  {
    menu.k_end->next=hotkey;
    menu.k_end=hotkey;
    hotkey->next=NULL;
  };

  while(g&&r==0)
  {
    r=GetStr(in,&s);
    if (r!=0)
      break;

    switch(s.keyword)
    {
      case K_END:
        r=0;g=0;  break;
      case K_ID:
        r=GetNum(in,&hotkey->ID);
        break;
      case K_KEY:
        r=GetStr(in,&s);
        if (r==0)
          hotkey->key=s.str[0];
        break;
      case K_SUBMENU:
        if (hotkey->name!=NULL)
        {
          r=-1;
          break;
        };
        r=GetStr(in,&s);
        if (r==0)
          hotkey->name=StrDup(s.str);
        break;
      default:
        r=-1;
    };
  };

  return(r);
}


static int ProcessMenu(FILE *in)
{
  int r;
  int g=1;
  Str s;

  ClearMenu();

  r=GetStr(in,&s);
  if (r!=0)
    return(r);

  menu.name=StrDup(s.str);

  while(g&&r==0)
  {
    r=GetStr(in,&s);
    if (r!=0)
      break;

    switch(s.keyword)
    {
      case K_END: r=0;g=0;  break;
      case K_TEXT:
        r=GetTextList(in,&menu.t_head);
        break;
      case K_ITEM:
        r=GetMenuItem(in,"");
        break;
      case K_SUBMENU:
        r=GetStr(in,&s);
        if (r==0)
          r=GetMenuItem(in,s.str);
        break;
      case K_HOTKEY:
        r=GetMenuHotKey(in);
        break;

      case K_BORDER:
        r=GetYesNo(in,&menu.border);  break;
      case K_TITLEWIN:
        r=GetYesNo(in,&menu.titlewin);  break;
      case K_MSGWIN:
        r=GetYesNo(in,&menu.msgwin);  break;

      case K_TITLE:
        if (menu.title!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            menu.title=StrDup(s.str);
        };
        break;
      case K_AT:
        r=GetNum(in,&menu.p_row);
        if (r==0)
          r=GetNum(in,&menu.p_col);
        break;
      case K_SIZE:
        r=GetNum(in,&menu.s_row);
        if (r==0)
          r=GetNum(in,&menu.s_col);
        break;

      default:
        r=-1;g=0;
    };
  };

  if (r==0)
    PrintMenuDef();

  return(r);
}
/********************************* FORM *******************************/
typedef struct Option_T
  {
    struct  Option_T  *next;
    int     ID;
    char      *str;
  } Option;
  
typedef struct Field_T
  {
    struct Field_T  *next;

    char    *name;
    char    *title;
    char    *rem;

    int   p_row,p_col;
    int   type;
    int   hidden;
    int   size;
    int   dec;
    int   displaysize;
    int   notnull;
    int   notzero;
    int   plus;
    int   notplus;
    int   minus;
    int   notminus;
    int   fixlen;
    int   displayonly;
    int   submit;

    char    *v_max,*v_min;
    Option    *o_head,*o_end;
  } Field;

typedef struct
  {
    char    *name;
    char    *title;
    int   border;
    int   titlewin;
    int   msgwin;
    int   inputwin;
    int   p_row,p_col;
    int   s_row,s_col;
    Text    *t_head;
    Field   *f_head,*f_end;
  } Form;
static Form form;

/**************************** CLEAR FORM *******************/

static int ClearOptionList(Option *p)
{
  Option  *p2;

  while(p!=NULL)
  {
    p2=p->next;
    FREE(p->str);
    FREE(p);
    p=p2;
  };
  return(0);
}

static int ClearFieldList(Field *p)
{
  Field *p2;

  while(p!=NULL)
  {
    p2=p->next;
    ClearOptionList(p->o_head);
    FREE(p->name);
    FREE(p->title);
    FREE(p->rem);
    FREE(p->v_max);
    FREE(p->v_min);
    FREE(p);
    p=p2;
  };
  return(0);
}

static int ClearForm()
{
  ClearTextList(form.t_head);
  ClearFieldList(form.f_head);

  FREE(form.name);
  FREE(form.title);

  form.name=NULL;
  form.title=NULL;
  form.border=1;
  form.titlewin=1;
  form.msgwin=1;
  form.inputwin=1;
  form.p_row=0;
  form.p_col=0;
  form.s_row=20;
  form.s_col=70;
  form.t_head=NULL;
  form.f_head=NULL;
  form.f_end=NULL;
  return(0);
}
/********************************* PRINT FORM ***************************/
static int PrintFormStructDef()
{
  Field *p;
  int i;

  fprintf(h_file,"typedef struct {\n");

  for(p=form.f_head; p!=NULL; p=p->next)
      switch(p->type)
      {
    case K_SELECT:
      fprintf(h_file,"\tUF_Select\t%s;\n",p->name); break;
    case K_INT:
      fprintf(h_file,"\tint\t%s;\n",p->name); break;
    case K_UINT:
      fprintf(h_file,"\tunsigned int\t%s;\n",p->name); break;
    case K_LONG:
      fprintf(h_file,"\tlong\t%s;\n",p->name); break;
    case K_ULONG:
      fprintf(h_file,"\tunsigned long\t%s;\n",p->name); break;
    case K_SHORT:
      fprintf(h_file,"\tshort\t%s;\n",p->name); break;
    case K_USHORT:
      fprintf(h_file,"\tunsigned short\t%s;\n",p->name); break;
    case K_DOUBLE:
      fprintf(h_file,"\tdouble\t%s;\n",p->name); break;
    case K_STR:
    case K_NSTR:
    case K_PASSWD:
      fprintf(h_file,"\tchar\t%s[%d];\n",
            p->name,p->size+1);
      break;
    case K_YMD:
      fprintf(h_file,"\tchar\t%s[11];\n",p->name); break;
    case K_HMS:
      fprintf(h_file,"\tchar\t%s[10];\n",p->name); break;
    case K_BUTTON:
      fprintf(h_file,"\tUF_Button\t%s;\n",p->name); break;
    default:
      return(-1);
      };
  fprintf(h_file,"\t} %s;\n",form.name);
  for(p=form.f_head,i=0; p!=NULL; p=p->next,i++)
    fprintf(h_file,"#define UFD_%s_%s\t%d\n",
            form.name,p->name,i);
  return(0);
}
static int PrintFormFieldOptionList()
{
  Field *f;
  Option  *p;

  for(f=form.f_head; f!=NULL; f=f->next)
     if (f->o_head!=NULL)
     {
    fprintf(c_file,"static UitFormFieldOption UitFormFieldOption_%s_%s[]={\n",
          form.name,f->name);
    for(p=f->o_head; p!=NULL; p=p->next)
    {
      fprintf(c_file,"\t{%d,",p->ID);
      PrintString(c_file,p->str);
      fprintf(c_file,"},\n");
    };
    fprintf(c_file,"\t{-1,NULL}\n\t};\n");
     };
  return(0);
}

static int PrintFormFieldCtrl()
{
  Field *p;

  fprintf(c_file,"static UitFormFieldList UitFormField_%s[]={\n",
                form.name);
  for(p=form.f_head;p!=NULL; p=p->next)
  {
    fprintf(c_file,"\t{offsetof(%s,%s),",form.name,p->name);
    fprintf(c_file,"UF_%s,",GetKeywordName(p->type));
    PrintString(c_file,p->name);fprintf(c_file,",");
    PrintString(c_file,p->title);fprintf(c_file,",");
    PrintString(c_file,p->rem);fprintf(c_file,",");
    fprintf(c_file,"%d,%d,%d,%d,%d,0",
        p->size,p->dec,p->displaysize,
        p->p_row,p->p_col);
    if (p->notnull) fprintf(c_file,"|UFO_NOTNULL");
    if (p->hidden)  fprintf(c_file,"|UFO_HIDDEN");
    if (p->notzero) fprintf(c_file,"|UFO_NOTZERO");
    if (p->plus)  fprintf(c_file,"|UFO_PLUS");
    if (p->notplus) fprintf(c_file,"|UFO_NOTPLUS");
    if (p->minus) fprintf(c_file,"|UFO_MINUS");
    if (p->notminus) fprintf(c_file,"|UFO_NOTMINUS");
    if (p->fixlen)  fprintf(c_file,"|UFO_FIXLEN");
    if (p->submit)  fprintf(c_file,"|UFO_SUBMIT");
    if (p->displayonly)
        fprintf(c_file,"|UFO_DISPLAYONLY");
    if (p->o_head==NULL)
      fprintf(c_file,",0,NULL},\n");
    else  fprintf(c_file,",0,UitFormFieldOption_%s_%s},\n",
          form.name,p->name);
  };
  fprintf(c_file,"\t{-1,-1,NULL,NULL,NULL,0,0,0,0,0,0,0,NULL}\n\t};\n");
  return(0);
}
static int PrintFormDef()
{
  PrintFormStructDef();
  PrintFormFieldOptionList();
  PrintFormFieldCtrl();
  if (form.t_head!=NULL)
    PrintTextList(c_file,"UitFormText",form.name,form.t_head);
  fprintf(c_file,"UitFormCtrl UitFormCtrl_%s={\n",form.name);
  fprintf(c_file,"\t");
  PrintString(c_file,form.name);
  fprintf(c_file,",");
  PrintString(c_file,form.title);
  fprintf(c_file,",\n\tUitFormField_%s,\n\t",form.name);
  fprintf(c_file,"%d,%d,%d,%d,\n",
      form.p_row,form.p_col,
      form.s_row,form.s_col);
  if (form.t_head!=NULL)
    fprintf(c_file,"\tUitFormText_%s,\n",form.name);
  else  fprintf(c_file,"\tNULL,\n");
  fprintf(c_file,"\tUIT_BORDER_%s|UIT_TITLEWIN_%s|UIT_MSGWIN_%s|UIT_INPUTWIN_%s\n\t};\n",
      form.border==0?"NO":"YES",
      form.titlewin==0?"NO":"YES",
      form.msgwin==0?"NO":"YES",
      form.inputwin==0?"NO":"YES");
  fprintf(c_file,"\n");
  fprintf(h_file,"extern UitFormCtrl UitFormCtrl_%s;\n",form.name);
  return(0);
}
/********************************* READ OPTION **************************/
static int GetFieldOption(FILE *in,Field *field)
{
  int r;
  Option  *option;
  Str s;

  option=(Option *)MALLOC(sizeof(Option));
  if (field->o_head==NULL)
  {
    field->o_head=option;
    field->o_end=option;
    option->next=NULL;
  }
  else
  {
    field->o_end->next=option;
    field->o_end=option;
    option->next=NULL;
  };


  r=GetNum(in,&option->ID);
  if (r!=0)
    return(r);
  r=GetStr(in,&s);
  if (r!=0)
    return(r);
  option->str=StrDup(s.str);
  return(0);
} 
/***************************** READ FIELD TYPE **********************/
static int GetFieldType(FILE *in,int *type)
{
  int r;
  Str s;

  r=GetStr(in,&s);
  if (r!=0)
    return(r);
  switch(s.keyword)
  {
    case K_INT:
    case K_UINT:
    case K_LONG:
    case K_ULONG:
    case K_SHORT:
    case K_USHORT:
    case K_DOUBLE:
    case K_STR:
    case K_NSTR:
    case K_PASSWD:
    case K_YMD:
    case K_HMS:
    case K_SELECT:
    case K_BUTTON:
      *type=s.keyword;  break;
    default:
      r=-1;
  };

  return(r);
}
/*************************** READ FORM FIELD ****************************/
static int GetFormField(FILE *in)
{
  int   r;
  Field   *field;
  Str   s;
  int   g=1;

  field=(Field *)MALLOC(sizeof(Field));

  if (form.f_head==NULL)
  {
    form.f_head=field;
    form.f_end=field;
    field->next=NULL;
  }
  else
  {
    form.f_end->next=field;
    form.f_end=field;
    field->next=NULL;
  };

  r=GetStr(in,&s);
  if (r!=0)
    return(r);
  field->name=StrDup(s.str);

  while(g&&r==0)
  {
    r=GetStr(in,&s);
    if (r!=0)
      break;

    switch(s.keyword)
    {
      case K_END:
        g=0;r=0;  break;
      case K_TITLE:
        if (field->title!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            field->title=StrDup(s.str);
        };
        break;
      case K_REM:
        if (field->rem!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            field->rem=StrDup(s.str);
        };
        break;
      case K_AT:
        r=GetNum(in,&field->p_row);
        if (r==0)
          r=GetNum(in,&field->p_col);
        break;
      case K_TYPE:
        r=GetFieldType(in,&field->type);
        if (r==0)
        {
          if (field->type==K_YMD)
            field->size=10;
          else if (field->type==K_HMS)
            field->size=8;
        };
        break;
      case K_HIDDEN:
        r=GetYesNo(in,&field->hidden);  break;
      case K_SIZE:
        r=GetNum(in,&field->size);  break;
      case K_DEC:
        r=GetNum(in,&field->dec); break;
      case K_DISPLAYSIZE:
        r=GetNum(in,&field->displaysize);break;
      case K_NOTNULL:
        field->notnull=1;   break;
      case K_NOTZERO:
        field->notzero=1;   break;
      case K_PLUS:
        field->plus=1;      break;
      case K_MINUS:
        field->minus=1;     break;
      case K_NOTPLUS:
        field->notplus=1;   break;
      case K_NOTMINUS:
        field->notminus=1;    break;
      case K_FIXLEN:
        field->fixlen=1;    break;
      case K_SUBMIT:
        field->submit=1;    break;
      case K_DISPLAYONLY:
        field->displayonly=1;   break;
      case K_OPTION:
        if (field->type!=K_SELECT)
          r=-1;
        else r=GetFieldOption(in,field);
        break;
      default:
        r=-1;
    };
  };

  return(r);
}
/******************************* READ FORM *****************************/
static int ProcessForm(FILE *in)
{
  int r;
  int g=1;
  Str s;

  ClearForm();

  r=GetStr(in,&s);
  if (r!=0)
    return(r);

  form.name=StrDup(s.str);

  while(g&&r==0)
  {
    r=GetStr(in,&s);
    if (r!=0)
      break;

    switch(s.keyword)
    {
      case K_END: r=0;g=0;  break;
      case K_TEXT:
        r=GetTextList(in,&form.t_head);
        break;
      case K_FIELD:
        r=GetFormField(in);
        break;

      case K_BORDER:
        r=GetYesNo(in,&form.border);  break;
      case K_TITLEWIN:
        r=GetYesNo(in,&form.titlewin);  break;
      case K_MSGWIN:
        r=GetYesNo(in,&form.msgwin);  break;
      case K_INPUTWIN:
        r=GetYesNo(in,&form.inputwin);  break;

      case K_TITLE:
        if (form.title!=NULL)
          r=-1;
        else
        {
          r=GetStr(in,&s);
          if (r==0)
            form.title=StrDup(s.str);
        };
        break;
      case K_AT:
        r=GetNum(in,&form.p_row);
        if (r==0)
          r=GetNum(in,&form.p_col);
        break;
      case K_SIZE:
        r=GetNum(in,&form.s_row);
        if (r==0)
          r=GetNum(in,&form.s_col);
        break;

      default:
        r=-1;g=0;
    };
  };

  if (r==0)
    PrintFormDef();

  return(r);
}

/**********************************************************************/

static int ProcessFile(FILE *in)
{
  int r;
  Str s;
  int g=1;

  while(g)
  {
    r=GetStr(in,&s);
    if (r!=0)
      g=0;
    else switch(s.keyword)
         {
      case K_END:
        r=0;g=0;  break;
      case K_MENU:
        r=ProcessMenu(in);
        if (r!=0)
          g=0;        break;
      case K_FORM:
        r=ProcessForm(in);
        if (r!=0)
          g=0;
        break;
      default:
        r=-1;
        g=0;
          };
  };

  return(r);
}
/****************************** INIT *******************************/
static char *getBaseFileName(char *s)
{
  static  char  name[128];
  int   i;

  strncpy(name,s,128);
  name[127]='\0';
  i=strlen(name);
  while(i>=0)
    if (name[i]=='.')
    {
      name[i]='\0';
      return(name);
    }
    else if (name[i]=='\\')
      return(name);
    else --i;
  return(name);
}
static int OpenFiles(char *uit_filename)
{
  base=getBaseFileName(uit_filename);
  sprintf(c_filename,"%s.c",base);
  sprintf(h_filename,"%s.h",base);

  in_file=fopen(uit_filename,"r");
  if (in_file==NULL)
  {
    fprintf(stderr,"Open %s error.\n",uit_filename);
    return(-1);
  };

  c_file=fopen(c_filename,"w");
  if (c_file==NULL)
  {
    fprintf(stderr,"Open %s error.\n",c_filename);
    return(-1);
  };
  h_file=fopen(h_filename,"w");
  if (h_file==NULL)
  {
    fprintf(stderr,"Open %s error.\n",h_filename);
    return(-1);
  };

  return(0);
}
static int PrintCopyright(FILE *out,char *date)
{
  fprintf(out,"\n");
  fprintf(out,"/**********************************************************\n");
  fprintf(out," * UIT 编译工具 Version 1.0                               *\n");
  fprintf(out," * 源 文 件: %-40s     *\n",uit_file);
  fprintf(out," * 日    期: %-40s     *\n",date);
  fprintf(out," **********************************************************/\n");
  fprintf(out,"\n\n");
}


static int PrintFileHead()
{
  time_t timer;
  struct tm *lt;
  char  *date;

  timer = time(NULL);
  lt = localtime(&timer);
  date=asctime(lt);
  date[strlen(date) - 1 ] = 0;

  fprintf(h_file,"#ifndef _UIT_%s_H_\n",base);
  fprintf(h_file,"#define _UIT_%s_H_\n",base);
  fprintf(h_file,"#include <UIT.h>\n\n");
  PrintCopyright(h_file,date);
  PrintCopyright(c_file,date);
  fprintf(c_file,"#include <stdio.h>\n");
  fprintf(c_file,"#include <%s>\n\n",h_filename);
  return(0);
}
static int PrintFileEnd()
{
  fprintf(h_file,"#endif\n");
  fprintf(c_file,"/******************** END ******************/\n");
  return(0);
}

/***********************************************************************/

int main(int argc,char *argv[])
{
  int del=0;
  int r;

  if (argc==2)
    uit_file=argv[1];
  else if (argc==3)
    if (strcmp(argv[1],"-D")==0)
    {
      del=1;
      uit_file=argv[2];
    }
    else goto E;
  else goto E;

  r=OpenFiles(uit_file);
  if (r==0)
  {
    PrintFileHead();
    r=ProcessFile(in_file);
    if (r==0)
      PrintFileEnd();
    fclose(c_file);
    fclose(h_file);
    if (r==0)
      return(0);
  
    fprintf(stderr,"ERROR AT LINE %d\n",lineCnt);
  };

  if (del)
  {
    unlink(c_filename);
    unlink(h_filename);
  };

  exit(1);
E:
  fprintf(stderr,"USE: uitCom [-D] <uit_filename>\n");
  exit(1);
}
 
