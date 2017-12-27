#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WinUtil.h"
#include "UitFormEdit.h"

static void Display(PWIN *win,char *s,int row,int col,int attr,
            int size,int echo)
{
  int i;

  if (echo)
  {
    WinDrawStringW(win,s,row,col,attr,size);
  }
  else
  {
    WinDrawStringW(win,"",row,col,attr,size);
    for(i=0; (i<size)&&(s[i]!='\0'); ++i)
      WinDrawString(win,"*",row,col+i,attr);
  };

}

int UitFormEditStr(PWIN *win,int row,int col,int displaysize,
      int echo,int numStr,int size,char *str)
{
  int key;
  int g=1;
  char  buf[128];
  int copy=1;
  int n=0;

  memset(buf,0,128);

  if (displaysize>80)
    displaysize=80;

  if (size>displaysize)
    size=displaysize;
  if (strlen(str)>size)
    str[size]='\0';

  strcpy(buf,str);
  n=0;

  while(g)
  {
    Display(win,buf,row,col,A_REVERSE,size,echo);
    WinSetCursor(win,row,col+n);
    WinFlush(win);
    fflush(stdout);
    key=WinGetKey(win);
    switch(key)
    {
      case KEYUP:
      case KEYDOWN:
      case '\t':
      case '\n':
      case '\r':
      case 6:
#ifndef NO_KEYEND
      case KEYEND:
#endif
      case KEYNPAGE:
        g=0; copy=1; break;

      case KEYRIGHT:
        if (n<(size-1)&&(buf[n]!='\0'))
          ++n;
        else
        {
          g=0; copy=1;
        };
        break;
      case KEYBACKSPACE:
      case 8:
        if (n==0)
          g=0;
        else 
        { --n; strcpy(buf+n,buf+n+1); };
        break;
      case KEYLEFT:
        if (n==0)
          g=0;
        else 
          --n;
        break;

      case 27:
        g=0; copy=0; break;

      default:
        if ((n<size)
          &&(((numStr==2)&&(key=='-')&&(n==0))
             ||((numStr!=0)&&key>='0'&&key<='9')
             ||((numStr==0)&&key>=' '&&key<255))
           )
        {
          buf[n++]=(char)key;
          if (n==size)
          {
            key='\0';
            copy=1;
            g=0;
          };
        }
        else WinBeep();
        break;
    };
  };

  if (copy)
    strcpy(str,buf);
  Display(win,str,row,col,A_NORMAL,size,echo);
  return(key);
}
int UitFormEditDouble(PWIN *win,int row,int col,int displaysize,
      int size,int dec,double *value)
{
  int key;
  int g=1;
  int copy=1;
  int n=0,nd=0;
  char  num[128],dec_num[32];
  int stat;
  int dec_p;
  char  format[16];
  int first = 1;
  int i;
  

  if (dec>30)
    dec=30;
  if (size>63)
    size=63;

  sprintf(format,"%%.%dlf",dec);
  sprintf(num,format,*value);

  strcpy(dec_num,"");
  for(n=0; num[n]!='\0'; ++n)
    if (num[n]=='.')
    {
      strcpy(dec_num,num+n+1);
      num[n]='\0';
      break;
    };
  if (strcmp(num,"0")==0)
    num[0]='\0';
  dec_p=displaysize-dec-1;
  n=strlen(num);
  stat=1;
  size-=dec;
  if (size>dec_p)
    size=dec_p;
  if (size<1)
    size=1;
  if (n>size)
  {
    num[size]='\0';
    n=size-1;
  };

  while(g)
  {
    WinDrawStringW(win,"",row,col,A_NORMAL,displaysize);
    WinDrawString(win,num,row,col+dec_p-n,A_REVERSE);
    WinDrawString(win,".",row,col+dec_p,A_REVERSE);
    WinDrawString(win,dec_num,row,col+dec_p+1,A_REVERSE);
    if (stat==1)
      WinSetCursor(win,row,col+dec_p-1);
    else  WinSetCursor(win,row,col+dec_p+1+nd);
    
    WinFlush(win);
    fflush(stdout);
    key=WinGetKey(win);
    if(first&&(key=='.'||key=='-'||(key>='0'&&key<='9')))
    {
      n = 0;nd = 0;
      for(i=0;i<dec;i++)
        dec_num[i]='0';
      dec_num[dec] = '\0';
    }
    switch(key)
    {
      case KEYUP:
      case KEYDOWN:
      case '\t':
      case 6:
#ifndef NO_KEYEND
      case KEYEND:
#endif
      case KEYNPAGE:
      case '\n':
      case '\r':
        g=0; copy=1; break;
      case KEYRIGHT:
        if (stat==1)
        {
          stat=2;nd=0;
        }
        else
        {
          g=0; copy=1;
        };
        break;

      case KEYBACKSPACE:
      case 8:
      case KEYLEFT:
        switch(stat)
        {
          case 1:
            if (n==0)
              g=0;
            else 
            { --n; num[n]='\0'; };
            break;
          case 2:
            dec_num[nd]='0';
            if (nd==0)
              stat=1;
            else --nd;
        };
        break;

      case 5:
      case 27:
        g=0; copy=0;
        break;


      case '.':
        if ((stat==1)&&(dec>0))
        { stat=2; nd=0; };
        break;
      case '-':
        if ((stat==1)&&(n==0))
        {
          num[n++]='-';
          num[n]='\0';
        }
        else WinBeep();
        break;

      default:
        if (key>='0'&&key<='9')
           switch(stat)
           {
          case 1: 
            if (n<size)
            { num[n++]=(char)key;
              num[n]='\0';
              if (n==size)
              {
                stat=2;nd=0;
              };
            }
            else
            {
              stat=2;nd=0;
              dec_num[0]=(char)key;
            };
            break;
          case 2:
            dec_num[nd++]=(char)key;
            if (nd>=dec)
            {
              key='\0';
              copy=1;
              g=0;
            };
            break;
           }
        else WinBeep();
        break;
    };
    first = 0;
  };

  if (copy)
  {
    strcat(num,".");
    strcat(num,dec_num);
    *value=atof(num);
  };
  sprintf(num,format,*value);
  WinDrawStringW(win,"",row,col,A_NORMAL,displaysize);
  WinDrawString(win,num,row,col+displaysize-strlen(num),A_NORMAL);
  WinFlush(win);
  return(key);
}
static int get_double_from_str(char *num,int dec,double *value)
{
  int i;
  int len;
  char  buf[128];

  if (num[0]=='-')
  {
    strcpy(buf,"-"); num++;
  }
  else  strcpy(buf,"");

  len=strlen(num);
  if (len>dec)
  {
    strncat(buf,num,len-dec);
    strcat(buf,".");
    strcat(buf,num+len-dec);
  }
  else
  {
    strcat(buf,"0.");
    for(i=0; i<dec-len; ++i)
      strcat(buf,"0");
    strcat(buf,num);
  };
  *value=atof(buf);
  return(0);
}
    
static int display_double_value(PWIN *win,int row,int col,
        int displaysize,int dec,
        char sign,double value,int attr)
{
  char  fmt[16];
  char  num[128];

  sprintf(fmt,"%%.%dlf",dec);
  sprintf(num,fmt,value);

  WinDrawStringW(win,"",row,col,A_NORMAL,displaysize);
  if ((sign=='-')&&(num[0]!='-'))
  {
    WinDrawString(win,"-",row,col+displaysize-strlen(num)-1,attr);
    WinDrawString(win,num,row,col+displaysize-strlen(num),attr);
  }
  else  WinDrawString(win,num,row,col+displaysize-strlen(num),attr);
  return(0);
}

/*
int UitFormEditDouble(PWIN *win,int row,int col,int displaysize,
      int size,int dec,double *value)
{
  int key;
  int g=1;
  int copy=1;
  int n=0,nd=0;
  char  num[128],*dec_num;
  int stat;
  int dec_p;
  char  format[16];
  double  tmp_value;
  

  if (dec>30)
    dec=30;
  if (size>63)
    size=63;

  sprintf(format,"%%.%dlf",dec);
  sprintf(num,format,*value);

  for(n=0; num[n]!='\0'; ++n)
    if (num[n]=='.')
    {
      strcpy(num+n,num+n+1);
      break;
    };
  if (num[0]=='-')
    while(num[1]=='\0')
      strcpy(num+1,num+2);
  else
    while(num[0]=='0')
      strcpy(num,num+1);

  n=strlen(num);
  if (n>size)
  { num[n-1]='\0';
    n=size-1;
  };
  while(g)
  {
    get_double_from_str(num,dec,&tmp_value);
    display_double_value(win,row,col,displaysize,dec,
          num[0],tmp_value,A_REVERSE);

    WinSetCursor(win,row,col+displaysize-1);
    WinFlush(win);
    key=WinGetKey(win);
    switch(key)
    {
      case KEYUP:
      case KEYDOWN:
      case '\t':
      case 6:
#ifndef NO_KEYEND
      case KEYEND:
#endif
      case KEYNPAGE:
      case '\n':
      case '\r':
      case KEYRIGHT:
        g=0; copy=1; break;

      case KEYBACKSPACE:
      case 8:
      case KEYLEFT:
        if (n==0)
            g=0;
        else 
        { --n; num[n]='\0'; };
        break;
      case 27:
        g=0; copy=0;
        break;

      case '-':
        if (n==0)
        {
          num[n++]='-';
          num[n]='\0';
        }
        else WinBeep();
        break;

      default:
        if ((key>='1'&&key<='9')
          ||((key=='0')&&(n>0)&&(num[0]!='-'))
          ||((key=='0')&&(n>1))
          )
          if (n<size)
          { num[n++]=(char)key;
            num[n]='\0';
            if (n==size)
            {
              key='\0';
              copy=1;
              g=0;
            };
          }
          else WinBeep();
        else WinBeep();
        break;
    };
  };

  if (copy)
  {
    get_double_from_str(num,dec,&tmp_value);
    *value=tmp_value;
  };
  display_double_value(win,row,col,displaysize,dec,
        '0',*value,A_NORMAL);
  WinFlush(win);
  return(key);
}
*/

static char dinm_leap[] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
static char dinm_uleap[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
static int LEAPYEAR(int y)
{
  if(y%4==0&&y%100!=0||y%400==0) return(1);
  return(0);
}
static int check_day(int y,int m,int d)
{
  if (d<1)
    return(-1);

  if (LEAPYEAR(y))
  {
    if (d>dinm_leap[m])
      return(-1);
  }
  else 
  {
    if (d>dinm_uleap[m])
      return(-1);
  }
  return(0);
}

int UitFormEditDateStr(PWIN *win,int row,int col,int notnull,char *str)
{
  int y,m,d;
  char  y_str[8];
  char  m_str[8];
  char  d_str[8];
  int g,stat;
  int copy=1;
  int i;
  int key;

  memset(y_str,0,8);
  memset(m_str,0,8);
  memset(d_str,0,8);
  strncpy(y_str,str,4);
  strncpy(m_str,str+5,2);
  strncpy(d_str,str+8,2);

  WinDrawStringW(win,y_str,row,col,A_REVERSE,4);
  WinDrawStringW(win,"-",row,col+4,A_REVERSE,1);
  WinDrawStringW(win,m_str,row,col+5,A_REVERSE,2);
  WinDrawStringW(win,"-",row,col+7,A_REVERSE,1);
  WinDrawStringW(win,d_str,row,col+8,A_REVERSE,2);


  g=1;stat=1;
  while(g)
  {
    switch(stat)
    {
        case 1:
      key=UitFormEditStr(win,row,col,4,1,1,4,y_str);
      break;
        case 2:
      key=UitFormEditStr(win,row,col+5,2,1,1,2,m_str);
      break;
        case 3:
      key=UitFormEditStr(win,row,col+8,2,1,1,2,d_str);
      break;
    };

    switch(key)
    {
        case KEYUP:
        case KEYDOWN:
        case '\t':
        case '\n':
        case '\r':
#ifndef HPUX
        case KEYEND:
#endif
        case 6:
      g=0;  break;

        case KEYLEFT:
        case KEYBACKSPACE:
        case 8:
      if (stat>1)
        --stat;
      else g=0;
      break;
        case KEYRIGHT:
        case '\0':
      if (stat<3)
        ++stat;
      else g=0;
      break;

        case 27:
        case 5:
      g=0; copy=0; break;
    };

    if (g==0&&copy)
    {
      y=atoi(y_str); m=atoi(m_str); d=atoi(d_str);
      if ((!notnull)&&y==0&&m==0&&d==0)
      {
        g=0;
      }
      else if (y<1900||y>2050)
      {
        y_str[0]='\0';  stat=1; g=1;
      }
      else if (m<1||m>12)
      {
        m_str[0]='\0';  stat=2; g=1;
      }
      else if (check_day(y,m,d)!=0)
      {
        d_str[0]='\0';  stat=3; g=1;
      }
    };
  };

  if (copy)
  {
    if (y==0&&m==0&&d==0)
    {
      memset(str,0,10);
    }
    else
    {
      str[0]='\0';
      for(i=0; i<(4-strlen(y_str)); ++i)
        strcat(str,"0");
      strcat(str,y_str);
      strcat(str,"-");
      for(i=0; i<(2-strlen(m_str)); ++i)
        strcat(str,"0");
      strcat(str,m_str);
      strcat(str,"-");
      for(i=0; i<(2-strlen(d_str)); ++i)
        strcat(str,"0");
      strcat(str,d_str);
    };
  };
  WinDrawStringW(win,str,row,col,A_NORMAL,10);
  WinFlush(win);
  return(key);
}
int UitFormEditTimeStr(PWIN *win,int row,int col,int notnull,char *str)
{
  int h,m,s;
  char  h_str[8];
  char  m_str[8];
  char  s_str[8];
  int g,stat;
  int copy=1;
  int i;
  int key;

  memset(h_str,0,8);
  memset(m_str,0,8);
  memset(s_str,0,8);
  strncpy(h_str,str,2);
  strncpy(m_str,str+3,2);
  strncpy(s_str,str+6,2);

  WinDrawStringW(win,h_str,row,col,A_REVERSE,2);
  WinDrawStringW(win,":",row,col+2,A_REVERSE,1);
  WinDrawStringW(win,m_str,row,col+3,A_REVERSE,2);
  WinDrawStringW(win,":",row,col+5,A_REVERSE,1);
  WinDrawStringW(win,s_str,row,col+6,A_REVERSE,2);

  g=1;stat=1;
  while(g)
  {
    switch(stat)
    {
        case 1:
      key=UitFormEditStr(win,row,col,2,1,1,2,h_str);
      break;
        case 2:
      key=UitFormEditStr(win,row,col+3,2,1,1,2,m_str);
      break;
        case 3:
      key=UitFormEditStr(win,row,col+6,2,1,1,2,s_str);
      break;
    };

    switch(key)
    {
        case KEYUP:
        case KEYDOWN:
        case '\t':
        case '\n':
        case '\r':
#ifndef NO_KEYEND
        case KEYEND:
#endif
        case KEYNPAGE:
        case 6:
      g=0;  break;

        case KEYLEFT:
        case KEYBACKSPACE:
        case 8:
      if (stat>1)
        --stat;
      else g=0;
      break;
        case KEYRIGHT:
        case '\0':
      if (stat<3)
        ++stat;
      else g=0;
      break;

        case 27:
        case 5:
      g=0; copy=0; break;
    };

    if (g==0&&copy)
    {
      h=atoi(h_str); m=atoi(m_str); s=atoi(s_str);
      if ((notnull&&h_str[0]=='\0')||h>23)
      {
        h_str[0]='\0';stat=1;g=1;
      }
      else if ((notnull&&m_str[0]=='\0')||m>59)
      {
        m_str[0]='\0';stat=2;g=1;
      }
      else if ((notnull&&s_str[0]=='\0')||s>59)
      {
        s_str[0]='\0';stat=3;g=1;
      };
    };
  };

  if (copy)
  {
    if (h_str[0]=='\0'&&m_str[0]=='\0'&&s_str[0]=='\0')
    {
      memset(str,0,8);
    }
    else
    {
      str[0]='\0';
      for(i=0; i<(2-strlen(h_str)); ++i)
        strcat(str,"0");
      strcat(str,h_str);
      strcat(str,":");
      for(i=0; i<(2-strlen(m_str)); ++i)
        strcat(str,"0");
      strcat(str,m_str);
      strcat(str,":");
      for(i=0; i<(2-strlen(s_str)); ++i)
        strcat(str,"0");
      strcat(str,s_str);
    };
  
  };
  WinDrawStringW(win,str,row,col,A_NORMAL,8);
  WinFlush(win);
  return(key);
}
int UitFormEditNum(PWIN *win,int row,int col,int displaysize,
          int sign,int size,char *value)
{
  int key;
  int g=1;
  int copy=1;
  int n=0;
  char  num[128];
  int stat;
  

  if (size>63)
    size=63;

  strncpy(num,value,63);
  if (strcmp(num,"0")==0)
    num[0]='\0';

  n=strlen(num);
  if (n>size)
  {
    num[size]='\0';
    n=size-1;
  };

  while(g)
  {
    WinDrawStringW(win,"",row,col,A_NORMAL,displaysize);
    WinDrawString(win,num,row,col+displaysize-n,A_REVERSE);
    WinSetCursor(win,row,col+displaysize-1);
    
    WinFlush(win);
    fflush(stdout);
    key=WinGetKey(win);
    switch(key)
    {
      case KEYUP:
      case KEYDOWN:
      case '\t':
      case KEYRIGHT:
      case 6:
#ifndef NO_KEYEND
      case KEYEND:
#endif
      case KEYNPAGE:
      case '\n':
      case '\r':
        g=0; copy=1; break;

      case KEYBACKSPACE:
      case 8:
      case KEYLEFT:
        if (n==0)
        { g=0;key=KEYLEFT;}
        else 
        { --n; num[n]='\0'; };
        break;
      case 5:
      case 27:
        g=0; copy=0;
        break;

      case '-':
        if ((n==0)&&sign)
        {
          num[n++]='-';
          num[n]='\0';
        }
        else WinBeep();
        break;

      default:
        if (key>='0'&&key<='9'&&n<size)
        {
          num[n++]=(char)key;
          num[n]='\0';
          if (n==size)
          {
            key='\0';
            copy=1;
            g=0;
          };
        }
        else WinBeep();
        break;
    };
  };

  if (copy)
  {
    strcpy(value,num);
  };
  WinDrawStringW(win,"",row,col,A_NORMAL,displaysize);
  WinDrawString(win,num,row,col+displaysize-strlen(num),A_NORMAL);
  WinFlush(win);
  return(key);
}

int UitFormEditButton(PWIN *win,int row,int col,char *title,UF_Button *p)
{
  int key;
  int g=1;
  int copy=1;
  int n=0;
  char  num[128];
  int stat;
  

  while(g)
  {
    WinDrawString(win,title,row,col,A_REVERSE);
    WinSetCursor(win,row,col);
    
    WinFlush(win);
    fflush(stdout);
    key=WinGetKey(win);
    switch(key)
    {
      case '\r':
      case '\n':
        g=0; copy=1; break;
      case '\t':
      case KEYUP:
      case KEYDOWN:
      case KEYRIGHT:
      case 6:
#ifndef NO_KEYEND
      case KEYEND:
#endif
      case KEYNPAGE:
      case 8:
      case KEYLEFT:
      case 5:
      case 27:
        g=0; copy=0; break;

      default:
        WinBeep();
        break;
    };
  };

  if (copy)
  {
    *p=BUTTON_ON;
  };
  WinDrawString(win,title,row,col,A_NORMAL);
  WinFlush(win);
  return(key);
}
