#include <stdio.h>
#include <string.h>
#include "UIT.h"
#include "UitFormEdit.h"

static int UitFormSelect(PWIN *win,int row,int col,int wide,
      UF_Select *p,UitFormFieldOption *option)
{
  int key;
  int g;
  int index;
  int copy=1;
  int i;

  index=p->index;
  g=1;
  while(g)
  {
    WinDrawStringW(win,option[index].str,
            row,col,A_REVERSE,wide);
    WinFlush(win);
    key=WinGetKey(win);

    switch(key)
    {
      case KEYUP:
      case KEYDOWN:
      case '\t':
      case '\n':
      case '\r':
      case 6:
      case KEYNPAGE:
#ifndef NO_KEYEND
      case KEYEND:
#endif
        g=0;copy=1; break;
  
      case 5:
      case 27:
        g=0;copy=0; break;
      case KEYLEFT:
        if (index>0)
          --index;
        else
          while(option[index+1].str!=NULL)
              ++index;
        break;
      case KEYRIGHT:
      default:
/* add by BW 1998.05.30 select 1-9 */
        if (key-'0'>=0 && key-'0'<=9)
          {
          i = index;
          while (option[i+1].str != NULL)
            i ++; 
          if (key-'0' <= i)
            index = key-'0';
          }
        else
          {
/* add end */
          if (option[index+1].str==NULL)
            index=0;
          else  index++;
          }
        break;
    };
  };

  if (copy)
  {
    p->index=index;
    p->str=option[index].str;
    p->ID=option[index].ID;
  };

  WinDrawStringW(win,option[p->index].str, row,col,A_NORMAL,wide);
  return(key);
}

static int UitFormFieldEdit(UitFormCtrl *form,void *data)
{
  int key;
  int row,col,wide,size,dec;
  char  buf[128];
  char  *title;
  void  *p;
  int n;


  row=form->field[form->index].p_row+form->top;
  col=form->field[form->index].p_col+form->left
    +strlen(form->field[form->index].title)
    +((form->win_option&UIT_INPUTWIN_YES)?1:0);
  wide=form->field[form->index].displaysize;
  size=form->field[form->index].size;
  dec=form->field[form->index].dec;
  p=(void *)((char *)data+form->field[form->index].offset);

  switch(form->field[form->index].type)
  {
    case UF_LONG:
      sprintf(buf,"%ld",*(long*)p);
      key=UitFormEditNum(&form->win,row,col,wide,1,size,buf);
      *(long*)p=atol(buf);
      break;
    case UF_ULONG:
      sprintf(buf,"%u",*(unsigned long*)p);
      key=UitFormEditNum(&form->win,row,col,wide,0,size,buf);
      *(unsigned long*)p=atol(buf);
      break;
    case UF_INT:
      sprintf(buf,"%d",*(int*)p);
      key=UitFormEditNum(&form->win,row,col,wide,1,size,buf);
      *(int*)p=atoi(buf);
      break;
    case UF_UINT:
      sprintf(buf,"%u",*(unsigned int*)p);
      key=UitFormEditNum(&form->win,row,col,wide,0,size,buf);
      *(unsigned int*)p=atoi(buf);
      break;
    case UF_SHORT:
      sprintf(buf,"%d",*(short*)p);
      key=UitFormEditNum(&form->win,row,col,wide,1,size,buf);
      *(short*)p=(short)atoi(buf);
      break;
    case UF_USHORT:
      sprintf(buf,"%u",*(unsigned short*)p);
      key=UitFormEditNum(&form->win,row,col,wide,0,size,buf);
      *(unsigned short*)p=(short)atoi(buf);
      break;
    case UF_DOUBLE:
      key=UitFormEditDouble(&form->win,row,col,wide,size,dec,
            (double *)p);
      break;
    case UF_SELECT:
      key=UitFormSelect(&form->win,row,col,wide,
            (UF_Select *)p,
          form->field[form->index].option);
      break;
    case UF_NSTR:
      key=UitFormEditStr(&form->win,row,col,wide,1,1,size,
            (char *)p);
      break;
    case UF_STR:
      key=UitFormEditStr(&form->win,row,col,wide,1,0,size,
            (char *)p);
      break;
    case UF_YMD:
      key=UitFormEditDateStr(&form->win,row,col,
      (form->field[form->index].f_option&UFO_NOTNULL)?1:0,
              (char *)p);
      break;
    case UF_HMS:
      key=UitFormEditTimeStr(&form->win,row,col,
      (form->field[form->index].f_option&UFO_NOTNULL)?1:0,
              (char *)p);
      break;
    case UF_PASSWD:
      key=UitFormEditStr(&form->win,row,col,wide,0,0,size,
            (char *)p);
      break;
    case UF_BUTTON:
      col=form->field[form->index].p_col+form->left;
      title=form->field[form->index].title;
      key=UitFormEditButton(&form->win,row,col,title,p);
      break;
    default:
      key=27;
  };

  return(key);
}

static int r_WinDrawStringW(PWIN *win,char *s,int row,int col,int attr,int wide)
{
  int len;

  len=strlen(s);
  if (len<wide)
  {
    WinDrawStringW(win,"",row,col,attr,wide);
    WinDrawStringW(win,s,row,col+wide-len,attr,len);
  }
  else  WinDrawStringW(win,s,row,col,attr,wide);
  return(0);
}
static int r_WinDrawStringW_B(UitFormCtrl  *form, char *b,char *s,int row,int col,int wide)
{
  int len;

  len=strlen(s);
  if (len<wide)
  {
    WinDrawStringW_B(form,b,"",row,col,wide);
    WinDrawStringW_B(form,b,s,row,col+wide-len,len);
  }
  else  WinDrawStringW_B(form,b,s,row,col,wide);
  return(0);
}

static int UitFormFieldSelectAdjust(UitFormCtrl *form,int index, UF_Select *p)
{
  int i;

  for(i=0; (form->field[index].option[i].str!=NULL)&&(p->index==-1); ++i)
    if (form->field[index].option[i].ID==p->ID)
    {
      p->index=i;
      p->str=form->field[index].option[i].str;
      return(0);
    };
  return(-2);
}

static int UitFormFieldValuePrint_B(UitFormCtrl *form,int index,void *data,char *b)
{
  char  format[64];
  char  buf[128];
  int row,col,wide;
  void  *p;
  int n;
  int r;

  row=form->field[index].p_row+form->top;
  col=form->field[index].p_col+form->left
    +strlen(form->field[index].title)
    +((form->win_option&UIT_INPUTWIN_YES)?1:0);
  wide=form->field[index].displaysize;
  p=(void *)((char *)data+form->field[index].offset);
  
  switch(form->field[index].type)
  {
    case UF_LONG:
      sprintf(buf,"%ld",*(long*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_ULONG:
      sprintf(buf,"%lu",*(unsigned long*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_INT:
      sprintf(buf,"%d",*(int*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_UINT:
      sprintf(buf,"%u",*(unsigned int*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_SHORT:
      sprintf(buf,"%d",*(short*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_USHORT:
      sprintf(buf,"%u",*(unsigned short*)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_STR:
    case UF_NSTR:
    case UF_YMD:
    case UF_HMS:
      WinDrawStringW_B(form,b,(char *)p, row,col,wide);
      break;
    case UF_PASSWD:
      WinDrawStringW_B(form,b,"", row,col,wide);
      WinDrawStringW_B(form,b,
        "*******************************************",
          row,col,strlen((char*)p));
      break;
    case UF_DOUBLE:
      sprintf(format,"%%.%dlf",form->field[index].dec);
      sprintf(buf,format,*(double *)p);
      r_WinDrawStringW_B(form,b,buf,row,col,wide);
      break;
    case UF_SELECT:
      if (((UF_Select *)p)->index==-1)
      {
        r=UitFormFieldSelectAdjust(form,index,
              (UF_Select *)p);
        if (r!=0)
          return(r);
      };
      n=((UF_Select *)p)->index;
      ((UF_Select *)p)->str=form->field[index].option[n].str;
      ((UF_Select *)p)->ID=form->field[index].option[n].ID;
      WinDrawStringW_B(form,b,((UF_Select *)p)->str, row,col,wide);
      break;
    case UF_BUTTON:
      break;
    default:
      return(-1);
  }

  return(0);
}

static int UitFormFieldValuePrint(UitFormCtrl *form,int index,void *data)
{
  char  format[64];
  char  buf[128];
  int row,col,wide;
  void  *p;
  int n;
  int r;

  row=form->field[index].p_row+form->top;
  col=form->field[index].p_col+form->left
    +strlen(form->field[index].title)
    +((form->win_option&UIT_INPUTWIN_YES)?1:0);
  wide=form->field[index].displaysize;
  p=(void *)((char *)data+form->field[index].offset);
  
  switch(form->field[index].type)
  {
    case UF_LONG:
      sprintf(buf,"%ld",*(long*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_ULONG:
      sprintf(buf,"%lu",*(unsigned long*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_INT:
      sprintf(buf,"%d",*(int*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_UINT:
      sprintf(buf,"%u",*(unsigned int*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_SHORT:
      sprintf(buf,"%d",*(short*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_USHORT:
      sprintf(buf,"%u",*(unsigned short*)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_STR:
    case UF_NSTR:
    case UF_YMD:
    case UF_HMS:
      WinDrawStringW(&form->win,(char *)p,
            row,col,A_NORMAL,wide);
      break;
    case UF_PASSWD:
      WinDrawStringW(&form->win,"",
            row,col,A_NORMAL,wide);
      WinDrawStringW(&form->win,
        "*******************************************",
          row,col,A_NORMAL,strlen((char*)p));
      break;
    case UF_DOUBLE:
      sprintf(format,"%%.%dlf",form->field[index].dec);
      sprintf(buf,format,*(double *)p);
      r_WinDrawStringW(&form->win,buf,row,col,A_NORMAL,wide);
      break;
    case UF_SELECT:
      if (((UF_Select *)p)->index==-1)
      {
        r=UitFormFieldSelectAdjust(form,index,
              (UF_Select *)p);
        if (r!=0)
          return(r);
      };
      n=((UF_Select *)p)->index;
      ((UF_Select *)p)->str=form->field[index].option[n].str;
      ((UF_Select *)p)->ID=form->field[index].option[n].ID;
      WinDrawStringW(&form->win,((UF_Select *)p)->str,
            row,col,A_NORMAL,wide);
      break;
    case UF_BUTTON:
      break;
    default:
      return(-1);
  }

  return(0);
}
static int UitFormFieldPrint(UitFormCtrl *form,int index,void *data)
{
  int len;
  int r;

  if (form->field[index].displaysize==0)
    form->field[index].displaysize=form->field[index].size;

  if (form->field[index].title==NULL)
    return(-1);

  len=strlen(form->field[index].title);

  WinDrawString(&form->win,form->field[index].title,
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col,A_NORMAL);
  if (form->field[index].type==UF_BUTTON)
    return 0;

  if (form->win_option&UIT_INPUTWIN_YES)
  {
    WinDrawString(&form->win,"[",
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col+len,
        A_NORMAL);
    WinDrawString(&form->win,"]",
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col+len
        +form->field[index].displaysize+1,
        A_NORMAL);
  };
  r=UitFormFieldValuePrint(form,index,data);
  return(r);
}

static int UitFormFieldPrint_B(UitFormCtrl *form,int index,void *data, char *b)
{
  int len;
  int r;

  if (form->field[index].displaysize==0)
    form->field[index].displaysize=form->field[index].size;

  if (form->field[index].title==NULL)
    return(-1);

  len=strlen(form->field[index].title);

  WinDrawString_B(&form->win,b,form->field[index].title,
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col);
  if (form->field[index].type==UF_BUTTON)
    return 0;

  if (form->win_option&UIT_INPUTWIN_YES)
  {
    WinDrawString_B(&form->win,b,"[",
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col+len);
    WinDrawString_B(&form->win,b,"]",
        form->top+form->field[index].p_row,
        form->left+form->field[index].p_col+len
        +form->field[index].displaysize+1);
  };
  r=UitFormFieldValuePrint_B(form,index,data, b);
  return(r);
}
static int UitFormFieldPrintMsg(UitFormCtrl *form)
{
  char  *rem;

  if (form->win_option&UIT_MSGWIN_YES)
  {
    if (form->field[form->index].rem==NULL)
      rem="";
    else rem=form->field[form->index].rem;

    WinDrawStringW(&form->win,rem,form->msg_line,form->left,
          A_NORMAL,form->s_col-2*form->left-1);
  };
  return(0);
}

int DrawUitForm(UitFormCtrl *form,void *data)
{
  int r;
  int n;
  int col;

  form->top=0;
  form->left=0;
  form->bottom=0;

  if (form->win_option&UIT_BORDER_YES)
  {
    WinDrawBox(&form->win);
    form->top=1;form->left=2;
    form->bottom=form->s_row-2;
  }
  else  form->bottom=form->s_row-1;

  if (form->title!=NULL)
  {
    col=(form->s_col-strlen(form->title))/2;
    if (col<0)
      col=0;

    if (form->win_option&UIT_TITLEWIN_YES)
    {
      WinDrawHLine(&form->win,form->top+1,BOXWIDE,
            form->s_col-2*BOXWIDE);
      WinDrawString(&form->win,form->title,
          form->top,col,A_TITLE);
      form->top+=2;
    }
    else if (form->win_option&UIT_BORDER_YES)
      WinDrawString(&form->win,form->title,0,col,A_TITLE);
  };

  if (form->win_option&UIT_MSGWIN_YES)
  {
    form->msg_line=form->bottom;
    WinDrawHLine(&form->win,form->msg_line-1,BOXWIDE,
            form->s_col-2*BOXWIDE);
    WinDrawStringW(&form->win,"",
        form->msg_line,form->left,A_TITLE,
        form->s_col-2*form->left);
    form->bottom-=2;
  };

  UitPrintWinText(&form->win,form->top,form->left,form->text);
  
  for(n=0,r=0; (form->field[n].offset!=-1)&&(r==0); ++n)
    if (!(form->field[n].f_option&UFO_HIDDEN))
      r=UitFormFieldPrint(form,n,data);

  if ((form->index>=n)||(form->index<0))
      form->index=0;

  WinFlush(&form->win);
  return(r);
}

int OutUitForm(UitFormCtrl *form, void *data, char **buf)
{
  int r;
  int n;
  int col;

  form->top=0;
  form->left=0;
  form->bottom=0;

  if ((*buf = (char *)malloc(form->s_row * form->s_col + 1)) == NULL){
    return(-1);
  }

  memset(*buf, ' ', form->s_row * form->s_col);
  (*buf)[form->s_row * form->s_col] = 0;

  if (form->win_option&UIT_BORDER_YES)
  {
    WinDrawBox_B(&form->win, *buf);
    form->top=1;form->left=2;
    form->bottom=form->s_row-2;
  }
  else  form->bottom=form->s_row-1;

  if (form->title!=NULL)
  {
    col=(form->s_col-strlen(form->title))/2;
    if (col<0)
      col=0;

    if (form->win_option&UIT_TITLEWIN_YES)
    {
      WinDrawHLine_B(&form->win, *buf,form->top+1,form->left,
          form->s_col-2*form->left);
      WinDrawString_B(&form->win, *buf,form->title,
          form->top,col);
      form->top+=2;
    }
    else if (form->win_option&UIT_BORDER_YES)
      WinDrawString_B(&form->win, *buf,form->title,0,col);
  };

  if (form->win_option&UIT_MSGWIN_YES)
  {
    form->msg_line=form->bottom;
    WinDrawHLine_B(&form->win, *buf,form->msg_line-1,form->left,
          form->s_col-2*form->left);
    WinDrawStringW_B(&form->win, *buf,"",
        form->msg_line,BOXWIDE,
        form->s_col-2*BOXWIDE);
    form->bottom-=2;
  };

  UitPrintWinText_B(form, *buf,form->top,form->left,form->text);
  
  for(n=0,r=0; (form->field[n].offset!=-1)&&(r==0); ++n)
    if (!(form->field[n].f_option&UFO_HIDDEN))
      r=UitFormFieldPrint_B(form,n,data, *buf);

  if ((form->index>=n)||(form->index<0))
      form->index=0;

  return(r);
}

int OpenUitForm(UitFormCtrl *form,void *data)
{
  int r;

  r=WinOpen(NULL,&form->win,form->p_row,form->p_col,
          form->s_row,form->s_col);
  if (r!=0)
    return(-1);

  r=DrawUitForm(form,data);
  WinFlush(&form->win);
  if (r!=0)
    WinClose(&form->win);
  return(r);
}

int CloseUitForm(UitFormCtrl *form)
{
  WinClose(&form->win);
  return(0);
}
static DisplayInputWarm(UitFormCtrl *form,int n)
{
static char *msg[]=
  {
    "",
    "此数据项必须输入数据",
    "此数据项不允许为零",
    "此数据项必须输入正数",
    "此数据项必须输入负数",
    "此数据项不允许输入正数",
    "此数据项不允许输入负数",
    "此数据项必须输入指定长度数据"
  };
  if (form->win_option&UIT_MSGWIN_YES)
  {
    WinDrawStringW(&form->win,msg[n],
        form->msg_line,form->left,
        A_REVERSE,form->s_col-2*form->left-1);
  };
  return(0);
}

static int UitFormCheckNULLstr(int i,UitFormCtrl *form,char *data)
{
  char  *p;
  switch(form->field[i].type)
  {
    case UF_STR:
    case UF_NSTR:
    case UF_PASSWD:
    case UF_YMD:
    case UF_HMS:
      p=data+form->field[i].offset;
      if (strlen(p)==0)
      {
        form->index=i;
        return(1);
      };
  };
  return(0);
}
static int UitFormCheckStrLen(int i,UitFormCtrl *form,char *data)
{
  char  *p;
  switch(form->field[i].type)
  {
    case UF_STR:
    case UF_NSTR:
    case UF_PASSWD:
    case UF_YMD:
    case UF_HMS:
      p=data+form->field[i].offset;
      if (strlen(p)!=form->field[i].size)
      {
        form->index=i;
        return(7);
      };
  };
  return(0);
}

static int UitFormCheckZero(int i,UitFormCtrl *form,char *data)
{
  switch(form->field[i].type)
  {
    case UF_INT:
      if (*(int *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_UINT:
      if (*(unsigned int *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_SHORT:
      if (*(short *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_USHORT:
      if (*(unsigned short *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_LONG:
      if (*(long *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_ULONG:
      if (*(unsigned long *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
    case UF_DOUBLE:
      if (*(double *)(data+form->field[i].offset)==0)
      { form->index=i; return(2); };
      break;
  };
  return(0);
}
static int UitFormCheckPlus(int i,UitFormCtrl *form,char *data)
{
  switch(form->field[i].type)
  {
    case UF_INT:
      if (*(int *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_UINT:
      if (*(unsigned int *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_SHORT:
      if (*(short *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_USHORT:
      if (*(unsigned short *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_LONG:
      if (*(long *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_ULONG:
      if (*(unsigned long *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
    case UF_DOUBLE:
      if (*(double *)(data+form->field[i].offset)<=0)
      { form->index=i; return(3); };
      break;
  };
  return(0);
}
static int UitFormCheckMinus(int i,UitFormCtrl *form,char *data)
{
  switch(form->field[i].type)
  {
    case UF_INT:
      if (*(int *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
    case UF_UINT:
      if (*(unsigned int *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
    case UF_SHORT:
      if (*(short *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
    case UF_USHORT:
/*---
      if (*(unsigned short *)(data+form->field[i].offset)>=0)
---*/
      { form->index=i; return(4); };
      break;
    case UF_LONG:
      if (*(long *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
    case UF_ULONG:
      if (*(unsigned long *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
    case UF_DOUBLE:
      if (*(double *)(data+form->field[i].offset)>=0)
      { form->index=i; return(4); };
      break;
  };
  return(0);
}
static int UitFormCheckNotPlus(int i,UitFormCtrl *form,char *data)
{
  switch(form->field[i].type)
  {
    case UF_INT:
      if (*(int *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_UINT:
      if (*(unsigned int *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_SHORT:
      if (*(short *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_USHORT:
      if (*(unsigned short *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_LONG:
      if (*(long *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_ULONG:
      if (*(unsigned long *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
    case UF_DOUBLE:
      if (*(double *)(data+form->field[i].offset)>0)
      { form->index=i; return(5); };
      break;
  };
  return(0);
}
static int UitFormCheckNotMinus(int i,UitFormCtrl *form,char *data)
{
  switch(form->field[i].type)
  {
    case UF_INT:
      if (*(int *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
    case UF_UINT:
      if (*(unsigned int *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
    case UF_SHORT:
      if (*(short *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
    case UF_USHORT:
/*---
      if (*(unsigned short *)(data+form->field[i].offset)<0)
---*/
      { form->index=i; return(6); };
      break;
    case UF_LONG:
      if (*(long *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
    case UF_ULONG:
      if (*(unsigned long *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
    case UF_DOUBLE:
      if (*(double *)(data+form->field[i].offset)<0)
      { form->index=i; return(6); };
      break;
  };
  return(0);
}

static int UitFormCheckValue(UitFormCtrl *form,char *data)
{
  int i;
  int r;

  for(i=0; form->field[i].offset!=-1; ++i)
      if (!(form->field[i].f_option&UFO_HIDDEN))
      {
    if (form->field[i].f_option&UFO_FIXLEN)
    {
      r=UitFormCheckStrLen(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_NOTNULL)
    {
      r=UitFormCheckNULLstr(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_NOTZERO)
    {
      r=UitFormCheckZero(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_PLUS)
    {
      r=UitFormCheckPlus(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_NOTPLUS)
    {
      r=UitFormCheckNotPlus(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_MINUS)
    {
      r=UitFormCheckMinus(i,form,data);
      if (r!=0)
        return(r);
    };
    if (form->field[i].f_option&UFO_NOTMINUS)
    {
      r=UitFormCheckNotMinus(i,form,data);
      if (r!=0)
        return(r);
    };
      };
  return(0);
}

int GetUitForm(UitFormCtrl *form,void *data,
    int (*func)(UitFormCtrl *form,void *data))
{
  int r;
  int g=1,e=0;
  int i;
  int key;
  int f_msg=0;
  int index;

  DrawUitForm(form,data);

  while (form->field[form->index].f_option&(UFO_HIDDEN|UFO_DISPLAYONLY))
        if (form->field[form->index+1].offset!=-1)
      ++form->index;
        else form->index=0;

  while(g)
  {
/**yan add **/
/*      DrawUitForm(form,data); */
/** add end **/
      if (f_msg==0)
    UitFormFieldPrintMsg(form);
      else
    DisplayInputWarm(form,f_msg);
      key=UitFormFieldEdit(form,data);
      f_msg=0;
      switch(key)
      {
    case KEYUP:
    case KEYLEFT:
      index=form->index;
      if (func!=NULL)
      { 
        key=(*func)(form,data);
        if (key==5||key==27)
          goto E;
        if (key!=0)
          goto BEEP;
      };
      if (index==form->index)
      do 
          if (form->index>0)
        --form->index;
          else while(form->field[form->index+1].offset!=-1)
        ++form->index;
      while (form->field[form->index].f_option
          &(UFO_HIDDEN|UFO_DISPLAYONLY));

      break;
    case '\n':
    case '\r':
      if (form->field[form->index].f_option&UFO_SUBMIT)
        goto END;
    case '\0':
    case KEYDOWN:
    case KEYRIGHT:
    case '\t':
      index=form->index;
      if (func!=NULL)
      {
        key=(*func)(form,data);
        if (key==5||key==27)
          goto E;
/* ADD BY DONOR BW 1998.04.14 */
      if (key == 6)
        {
        if ((f_msg=UitFormCheckValue(form,data))==0)
          { 
          g=0;
          e=6;
          break;
          }
        }
/* END ADD BY BW */
        if (key!=0)
          goto BEEP;
      };
      if (index==form->index)
      {
/** del by yantao 1999.04.20 **
        if (key=='\0'&&
          form->field[form->index].f_option
                &UFO_SUBMIT)
            break;
****/
        do
            if (form->field[form->index+1].offset!=-1)
          ++form->index;
            else form->index=0;
        while (form->field[form->index].f_option
            &(UFO_HIDDEN|UFO_DISPLAYONLY));
      };
      break;
    case 27:
E:
      g=0;e=27; break;

    case 6:
#ifndef NO_KEYEND
    case KEYEND:
#endif
    case KEYNPAGE:
END:
      if (func!=NULL)
      { 
        key=(*func)(form,data);
        if (key==5||key==27)
          goto E;
        if (key !=0)
          goto BEEP;
      };
      if ((f_msg=UitFormCheckValue(form,data))==0)
        { 
        g=0;e=0;
        }
      break;
    default:
BEEP:
      WinBeep(&form->win);
      };
  };

  return(e);
}

int PromptUitForm(UitFormCtrl *form, char *prompt)
{
  if (form->win_option&UIT_MSGWIN_YES)
  {
    WinDrawStringW(&form->win,prompt,form->msg_line,form->left,
          A_BOLD,form->s_col-2*form->left-1);
    WinGetKey(&form->win);
  };
  return(0);
}
int UitFormPrintBuf(char *file, char *buf, int row, int col)
{
  int i, hd;
  FILE *fp;

  if ((fp = fopen(file, "w")) == NULL)
    return(-1);

  for(i = 0; i<row*col; i++){
    if (((i % col) == 0) && ( i != 0))
      fprintf(fp, "\n");

    fprintf(fp, "%c", buf[i]);
  }
  fclose(fp);

  return(0);
}

int UitForm_SetItemOptionIndexByID(UitFormCtrl *form,void *data,
           int itemIndex,int optionID)
{
  UF_Select   *item;
  UitFormFieldOption  *option;
  int     index;

  if (form->field[itemIndex].type!=UF_SELECT)
    return(-1);
  
  item=(UF_Select *)((char *)data+form->field[itemIndex].offset);
  option=form->field[itemIndex].option;
  index=0;
  while(option[index].str!=NULL)
    if (option[index].ID==optionID)
    {
      item->index=index;
      item->ID=optionID;
      item->str=option[index].str;
      return(0);
    }
    else index++;
  return(-1);
}
