#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uit_read.h"
#include "WinUtil.h"

#define _RANGE  2000
#define _SUB 80
#define _LINE 512

#define KEY_PAGE_DOWN   6
#define KEY_PAGE_UP 2
#define KEY_HEAD  1
#define KEY_TAIL  5

FILE  *fp_src;

typedef struct
{
  long offset;
  int size;
}Node;

static Node N_Array[_RANGE];
static int array_num=0;
static int eof=0;

static int ext_space(char *);

static int get_line(FILE *fp_src,int num)
{
  int flag=0;
  char  t=0;

  array_num=0;
  memset(&N_Array,0,sizeof(Node)*_RANGE);
  if(fseek(fp_src,0,SEEK_SET)!=0)
    return -1;

  for(array_num=0;array_num<_RANGE;array_num++)
  {
    N_Array[array_num].offset=ftell(fp_src);
    while(!ferror(fp_src)&&!feof(fp_src))
    {
      if((t=fgetc(fp_src))=='\n')
        break;
    };
    N_Array[array_num].size=
      (ftell(fp_src)-N_Array[array_num].offset<_LINE)?
      ftell(fp_src)-N_Array[array_num].offset:_LINE;

    if(feof(fp_src) || ferror(fp_src))
      break;

  }
  return 0;
}

static int init_Buffer(char *filename)
{
  fp_src=fopen(filename,"r");
  if(fp_src==NULL)
    return -1;
  else
    get_line(fp_src,_RANGE);
  return 0;
}

static int get_line_byno(char *buf,int line_no)
{
  int r,no=0;
  if(line_no<0||line_no>=array_num)
    return -1;

  memset(buf,0,(N_Array[line_no].size+1)*sizeof(char));
  fseek(fp_src,N_Array[line_no].offset*sizeof(char),SEEK_SET);
  fgets(buf,N_Array[line_no].size,fp_src);

  r = ext_space(buf);
  return r;
}

static  IsHanZi(int n,char *s)
{ int f=0,i;
  int Eend;
  unsigned char *Buf;

  Buf=(unsigned char *)s;
  
  Eend=strlen((char *)Buf);
  if (n<Eend)
     for (i=0; i<=n; ++i)
    switch (f)
    { case 0: if (Buf[i]>160)   f=1;  break;
      case 1: f=2;      break;
      case 2: if (Buf[i]>160)   f=1;
          else    f=0;  break;
    };

  if ((n==(Eend-1))&&(f==1))
    f=0;
  return(f);
}

static int get_substr(char *line,char *str,int start,int length)
{
  char sub[_SUB+1];
  int i=0,j=0,flag=0;

  if(line==NULL)
    return -1;
  memset(sub,' ',_SUB*sizeof(char));
  sub[_SUB]=0;

  for(i=0;i<=length;i++)
    if(i+start<strlen(line))
      sub[i]=line[i+start];

  if (IsHanZi(start,line)==2)
    sub[0]=' ';
  if (IsHanZi(start+length-1,line)==1)
    sub[length-1]=' ';

  strcpy(str,sub);
  return 0;
}

static int get_substr_inline(char *buf,int line_no,int start,int len)
{
  char line[_LINE+1];
  int r=0;
  memset(line,0,_LINE*sizeof(char));
  r = get_line_byno(line,line_no);
  if(r!=0)
    return r;
  r=get_substr(line,buf,start,len);
  return 0;
}

static int ext_space(char *s)
{
  int n=0,i=0,j=0,m=0;
  char  out[_LINE+1];
  char  tmp[_LINE+1];

  memset(out,0,sizeof(char)*_LINE);
  for(i=0,n=0; ((s[i]!='\0') && (i<=_LINE)); ++i)
  {
    if (s[i]=='\t')
    {
      m=8-n%8;
      for(j=0; j<m; ++j)
        strcat(out," ");
      n+=m;
    }
    else
    {
      n++;
      sprintf(tmp,"%c",s[i]);
      strcat(out,tmp);
    };
  };

  out[n]=0;
  strncpy(s,out,_LINE);
  s[_LINE]='\0';
  return 0;
}

static int get_max_line_num()
{
  return array_num;
}

static int get_max_char(int top_line, int line_num)
{
  char buf[_LINE+1];
  int i=0,max_char_num=0,r=0;

  for(i=0;i<line_num-1;i++)
  {
    r=get_line_byno(buf,top_line+i);
    if (r==0)
      if (strlen(buf)>max_char_num)
        max_char_num=strlen(buf);
  };

  return max_char_num;
}

static int draw_line_col_mode
    (PWIN *pwin,
     int top_line,int left_line,
     char *fixcol ,
     int line_flag)
{
  int i=0,r=0;
  char line[_LINE+1], buf[_LINE+1];
  char s1[2048],s2[2048];
  int hight=0,width=0;

  width=pwin->wide;

  if (fixcol!=NULL)
  {

    WinDrawHLine(pwin,1,0,width);
    strcpy(line,fixcol);
    ext_space(line);

    r = get_substr(line,buf,left_line,width);
    if (r==0)
      WinDrawStringW(pwin,buf,0,0,A_NORMAL,width);
    if (line_flag==0)
    {
      for(i=2;i<pwin->high;i++)
      {

        r=get_substr_inline(buf,top_line+i-2,
                  left_line,width);

        if(r==0)
        {

          WinDrawStringW(pwin,buf,i,0,
                A_NORMAL,pwin->wide);

        }
      }

    }
    else
    {
      for(i=2;i<pwin->high-2;i++)
      {

        r=get_substr_inline(buf,top_line+i-2,
            left_line,width);
        if(r==0)
          WinDrawStringW(pwin,buf,i,0,
                A_NORMAL,pwin->wide);

      }
    }
  }
  else
  {
    if (line_flag==0)
    {

      for(i=0;i<pwin->high;i++)
      {
        r=get_substr_inline(buf,top_line+i,
            left_line,width);
        if(r==0)
          WinDrawStringW(pwin,buf,i,0,
              A_NORMAL,pwin->wide);
      }

    }
    else
    {
      for(i=0;i<pwin->high-2;i++)
      {

        r=get_substr_inline(buf,top_line+i,
            left_line,width);
        if (r==0)
          WinDrawStringW(pwin,buf,i,0,
              A_NORMAL,pwin->wide);
      }
    }
  }
  if (line_flag!=0)
  {

    WinDrawHLine(pwin,pwin->high-2,0,width);
    sprintf(buf," Line=%d,Column=%d",top_line+1,left_line+1);

    WinDrawStringW(pwin,buf, pwin->high-1,
        width-strlen(buf)-2,A_NORMAL,strlen(buf));
  }

  return 0;
}

static int DispText(PWIN *pwin,char *fixcol,int line_flag)
{
  int i,j,r;
  int line_num,width;
  char *line,tmp[_LINE+1];
  int top_line=0,left_line=0,max_char_num;
  int hight=0;
  int max_line_num;

  max_line_num=get_max_line_num();

  line_num=pwin->high;
  hight=pwin->high;
  if (line_flag==1) 
    hight=hight-1-BOXWIDE;
  if (fixcol!=NULL)
    hight=hight-1-BOXWIDE;
  width=pwin->wide-1;

  /* WinCursorOff(pwin); */
  draw_line_col_mode(pwin,top_line,left_line,fixcol,line_flag);

  WinFlush(pwin);
  do{

    r = WinGetKey(pwin);
    switch (r)
    {

    case KEYDOWN:
      if ((line_flag==1) && (fixcol!=NULL))
        j = get_line_byno(tmp,top_line+line_num-4);
      else if (  ((line_flag==1) && (fixcol==NULL))
          || ((line_flag!=1) && (fixcol!=NULL)) )
        j = get_line_byno(tmp,top_line+line_num-2);
      else if ((line_flag!=1) && (fixcol==NULL))
        j = get_line_byno(tmp,top_line+line_num);

      if (j==0)
      {
        top_line+=1;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
      }
      break;
    case KEYUP:
      j=get_line_byno(tmp,top_line);
      if (j==0)
      {
        if (top_line>0)
          top_line-=1;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
      }
      break;
    case KEYLEFT:
      if (left_line>0)
      {
        left_line-=8;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
      }
      break;
    case KEYRIGHT:
      max_char_num=get_max_char(top_line,line_num);
      if (left_line+width<max_char_num)
      {
        left_line+=8;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
      }
      break;
    case KEY_PAGE_DOWN:
      if ((line_flag==1) && (fixcol!=NULL))
        j =get_line_byno(tmp,top_line+line_num-4+hight);
      else if (  ((line_flag==1) && (fixcol==NULL))
          || ((line_flag!=1) && (fixcol!=NULL)) )
        j =get_line_byno(tmp,top_line+line_num-2+hight);
      else if ((line_flag!=1) && (fixcol==NULL))
        j = get_line_byno(tmp,top_line+line_num+hight);

      if (j==0)
      {
        top_line+=hight;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
      }
      else
      {
        top_line=max_line_num-hight;
        draw_line_col_mode(pwin,top_line,
                left_line,fixcol,line_flag);
        
      }
      break;
    case KEY_PAGE_UP:
      j=get_line_byno(tmp,top_line);
      if (j==0)
      {
        if (top_line>hight)
          top_line-=hight;
        else
          top_line=0;
        draw_line_col_mode(pwin,top_line,
               left_line,fixcol,line_flag);
      }
      break;
    case KEY_HEAD:
      top_line=0;
      left_line=0;
      draw_line_col_mode(pwin,top_line,left_line,fixcol
                ,line_flag);
      break;
    case KEY_TAIL:
      top_line=max_line_num-hight;
      left_line=0;
      draw_line_col_mode(pwin,top_line,left_line,fixcol
                ,line_flag);
      break;
    }
    WinFlush(pwin);
  }while (r!=KEYESC);
  return 0;
}

static int DispTextArea
    (int row,int column,int hight,int width,
     char *title,
     char *tail,
     char *fixcol,
     int line_flag)
{

  int r,mid,buflen;
  PWIN pwin,pwin2;
  char buf[_LINE+1];
  int row2,column2,high2,width2;
  int line_num;
  if (hight<10 && width<8)
    hight=10,width=8;

  WinOpen(NULL,&pwin,row,column,hight,width);

  WinDrawBox(&pwin);

  if (title!=NULL)
  {

    if (strlen(title)<(width-2*BOXWIDE))
      buflen=strlen(title);
    else
      buflen=width-2*BOXWIDE;
    mid =(int)((width-buflen)/2);
    if (mid<0) mid=0;

    WinDrawStringW(&pwin,title,1,mid,A_NORMAL,((width)/2 - mid)*2);
    WinDrawHLine(&pwin,2,BOXWIDE,width-BOXWIDE*2);
  }
  if (tail!=NULL)
  {

    if (strlen(tail)<(width-2*BOXWIDE))
      buflen=strlen(tail);
    else
      buflen=width-2*BOXWIDE;
    mid =(int)(width-2-buflen);
    if (mid<0) mid=0;
    WinDrawStringW(&pwin,tail,hight-2,mid,A_NORMAL,(width-2- mid));
    WinDrawHLine(&pwin,hight-3,BOXWIDE,width-2*BOXWIDE);
  }
  if (title!=NULL && tail!=NULL)
  {
    line_num=hight-6;
    WinOpen(&pwin,&pwin2,3,BOXWIDE,hight-6,width-BOXWIDE*2);
  }
  else if (title!=NULL && tail==NULL)
  {
    line_num=hight-4;
    WinOpen(&pwin,&pwin2,3,BOXWIDE,hight-4,width-BOXWIDE*2);
  }
  else if (title==NULL && tail!=NULL)
  {
    line_num=hight-2;
    WinOpen(&pwin,&pwin2,1,BOXWIDE,hight-4,width-BOXWIDE*2);
  }
  else
  {
    line_num=hight-2;
    WinOpen(&pwin,&pwin2,1,BOXWIDE,hight-2,width-BOXWIDE*2);
  }
  /*
  WinDrawBox(&pwin2);
  */
  WinFlush(&pwin);
  WinFlush(&pwin2);
  DispText(&pwin2,fixcol,line_flag);
  WinClose(&pwin2);
  WinClose(&pwin);
}

int DispTextAreaFile
    (char *filename,
     int row,int column,int high,int wide,
     char *title,
     char  *foot,
     char *fixline,
     int line_mode)
{
  int r;
  if (init_Buffer(filename)!=0)
    return r;
  DispTextArea(row,column,high,wide,title,foot,fixline,line_mode);
  fclose(fp_src);
  return 0;
}

static int Choose_draw_line_col_mode
    (PWIN *pwin,int top_line,int left_line)
{
  int i=0,r=0;
  char  strBuf[_LINE+1];
  int hight=0,width=0;

  width=pwin->wide;
  hight=pwin->high-1;
  memset(strBuf,0,sizeof(strBuf));

  for(i=0;i<hight-1;i++)
  {
    r=get_substr_inline(strBuf,top_line+i, left_line,width);
    if (r==0)
      WinDrawStringW(pwin,strBuf,i,0, A_NORMAL,width);
  };

  get_substr_inline(strBuf,top_line,0,width-1);
  WinDrawHLine(pwin,hight-1,0,width);
  WinDrawStringW(   pwin,strBuf, hight,
        width-strlen(strBuf)-2,
        A_BOLD|A_REVERSE,width);

  return 0;
}

static int ChooseDispText(PWIN *pwin)
{
  int i=0,j=0,r=0;
  int highwin=0,width=0,hight=0;
  char strBuf[_LINE+1];
  int top_line=0,left_line=0;
  int max_line_num=0,curline=0;
  max_line_num=array_num;
  hight=pwin->high-2-BOXWIDE;
  width=pwin->wide;
  highwin=pwin->high-1;
  Choose_draw_line_col_mode(pwin,top_line,left_line);
  memset(strBuf,0,sizeof(strBuf));
  get_line_byno(strBuf,top_line+curline);
  WinDrawStringW(pwin,strBuf, curline, 0, A_REVERSE|A_BOLD,width);
  WinDrawStringW(pwin,strBuf,hight+2, 0, A_REVERSE|A_BOLD,width);
  WinFlush(pwin);
  do{

    r = WinGetKey(pwin);
    switch (r)
    {
    case KEYDOWN:
      if(curline<hight)
      {
        memset(strBuf,0,sizeof(strBuf));
        get_line_byno(strBuf,top_line+curline);
        WinDrawStringW(pwin,strBuf,
            curline, 0, A_NORMAL,width);

        if(curline+1<max_line_num)
          curline++;
        get_line_byno(strBuf,top_line+curline);
        WinDrawStringW(pwin,strBuf, curline, 0,
              A_BOLD|A_REVERSE,width);
        WinDrawStringW(pwin,strBuf,highwin,0,
              A_BOLD|A_REVERSE,width);
        WinFlush(pwin);

        continue;
      };
      curline=hight;
      if(top_line+hight+1<max_line_num)
        top_line++;
      else
        continue;

      break;
    case KEYUP:
      if(curline>0)
      {
        memset(strBuf,0,sizeof(strBuf));
        get_line_byno(strBuf,top_line+curline);
        WinDrawStringW(pwin,strBuf, curline, 0,
                A_NORMAL,width);

        curline--;
        get_line_byno(strBuf,top_line+curline);
        WinDrawStringW(pwin,strBuf,curline, 0,
              A_BOLD|A_REVERSE,width);
        WinDrawStringW(pwin,strBuf,highwin,0,
              A_BOLD|A_REVERSE,width);
        WinFlush(pwin);

        continue;
      };
      curline=0;
      if(top_line>0)
        top_line--;
      else
        continue;

      break;
    case KEY_NPAGE:
    case KEYRIGHT:
      if(hight>=max_line_num)
      {
        curline=max_line_num-1;
        top_line=0;
        break;
      };

      if(curline+top_line+hight>max_line_num)
      {
        curline=hight;
        top_line=max_line_num-1-hight;

        break;
      };
      if(curline!=hight)
        curline=hight;
      else
      if(top_line+hight+1<max_line_num)
      {
        curline=0;
        top_line+=hight-1;
      };

      break;
    case KEY_PPAGE:
    case KEYLEFT:
      if(curline+top_line<hight)
      {
        curline=top_line=0;
        break;
      };

      if(curline!=0)
        curline=0;
      else
      if(top_line-hight>0)
      {
        curline=0;
        top_line-=hight-1;
      };

      break;
    case KEY_HOME:
    case KEY_HEAD:
      curline=top_line=0;
      break;
#ifdef KEY_END
    case KEY_END:
#endif
    case KEY_TAIL:
      if(hight>=max_line_num)
      {
        curline=max_line_num-1;
        top_line=0;
        break;
      };

      curline=hight;
      top_line=max_line_num-1-hight;
      break;
    default:
      continue;
    };

    Choose_draw_line_col_mode(pwin,top_line,left_line);
    get_line_byno(strBuf,top_line+curline);
    WinDrawStringW(pwin,strBuf,curline,
            0, A_BOLD|A_REVERSE,width);
    WinDrawStringW(pwin,strBuf,highwin,
            0,A_BOLD|A_REVERSE,width);
    WinFlush(pwin);

  }while (r!=KEYESC&&r!='\n');

  if(max_line_num==0)
    return -1;

  if(r=='\n')
    return (top_line+curline);

  return -1;
}

static int ChooseDispTextArea
    (int row,int column,int hight,int width,char *title)
{

  int r=0,mid=0,buflen=0;
  PWIN pwin,pwin2;
  char buf[_LINE+1];

  if(hight<8)
    hight=8;
  if(hight>22)
    hight=22;
  if(width<8)
    width=8;
  if(width>78)
    width=78;

  WinOpen(NULL,&pwin,row,column,hight,width);
  WinDrawBox(&pwin);

  if (title!=NULL)
  {

    if (strlen(title)<(width-2*BOXWIDE))
      buflen=strlen(title);
    else
      buflen=width-2*BOXWIDE;
    mid =(int)((width-buflen)/2);
    if (mid<0) mid=0;

    WinDrawStringW(&pwin,title,1,mid,A_NORMAL,((width)/2 - mid)*2);
    WinDrawHLine(&pwin,2,BOXWIDE,width-BOXWIDE*2);

    WinOpen(&pwin,&pwin2,3,BOXWIDE,hight-4,width-BOXWIDE*2);
  }
  else
    WinOpen(&pwin,&pwin2,1,BOXWIDE,hight-2,width-BOXWIDE*2);

  WinFlush(&pwin);
  WinFlush(&pwin2);

  r=ChooseDispText(&pwin2);

  WinClose(&pwin2);
  WinClose(&pwin);
  return r;
}

int ChooseDispTextAreaFile
    (char *fileName,
     int row,int column,int high,int wide,
     char *title,
     char *retString)
{
  int r=0;

  if((r=init_Buffer(fileName))!=0)
    return r;

  r=ChooseDispTextArea(row,column,high,wide,title);

  if(r!=-1)
    get_line_byno(retString,r);

  fclose(fp_src);
  return ((r==-1)?(-1):(r+1));
}

