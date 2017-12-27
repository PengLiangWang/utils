#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <termio.h>
#include "WinUtil.h"

static int chrcpy(char *d, char *s, int w)
{
  int i;

  w = (w > strlen(s)) ? strlen(s) : w;
  for(i = 0; i<w; i++)
    d[i] = s[i];

  return 0;
}

#ifdef USE_CURSES
static int freeze=0;
#ifndef USE_CURSES_PANEL
static PWIN screen={0,0,24,80,
      NULL,
      NULL,NULL,NULL,NULL};

static void WINLOG_ERROR()
{
  fprintf(stderr,"WinUtil WINLOG ERROR!\n");
  exit(1);
}
static void WinLog(PWIN *pw,PWIN *w)
{
  PWIN  *p;

  w->f = pw;
  w->n = NULL;
  w->sub = NULL;
  if (pw->sub == NULL)
  {
    pw->sub = w;
    w->p = NULL;
  }
  else
  {
    p = pw->sub;
    while(p->n != NULL) p = p->n;
    p->n = w;
    w->p = p;
  };
}
static void WinUnLog(PWIN *w)
{
  if (w->f==NULL)
  {
    WINLOG_ERROR();
  }
  if (w->p==NULL)
  {
    w->f->sub=w->n;
  }
  else
  {
    w->p->n=w->n;
  }

  if (w->n!=NULL)
  {
    w->n->p=w->p;
  }

  w->f=NULL;w->p=NULL;w->n=NULL;
}
static void WinLogToTop(PWIN *w)
{
  PWIN  *f;

  if (w->f==NULL)
    WINLOG_ERROR();

  f=w->f;
  WinUnLog(w);
  WinLog(f,w);
}
static void WinRedrawList(PWIN *w)
{
  while(w!=NULL)
  {
    touchwin(w->win);
    wnoutrefresh(w->win);
    if (w->sub!=NULL)
      WinRedrawList(w->sub);
    w=w->n;
  };
}
int WinRedrawAll()
{
  if (screen.win==NULL)
    screen.win=stdscr;

  WinRedrawList(&screen);
  if (freeze<=0)
    doupdate();
  return 0;
}
#else
static void WinLog(PWIN *w)
{
  w->panel=new_panel(w->win);
}
static void WinUnLog(PWIN *w)
{
  if (w->panel!=NULL)
  {
    del_panel(w->panel);
    w->panel=NULL;
  }
}
int WinRedrawAll()
{
  PANEL *p=NULL;

  while((p=panel_above(p))!=NULL)
  {
    touchwin(panel_window(p));
    wnoutrefresh(panel_window(p));
  }
  update_panels();
/** if (freeze<=0) **/
    doupdate();
  return 0;
}
#endif

int WinForceRedraw()
{
  touchwin(stdscr);
  wrefresh(stdscr);
  WinRedrawAll();
  return 0;
}


int WinFreeze()
{
  return ++freeze;
}
int WinThaw()
{
  if (freeze>0)
    --freeze;
  if (freeze<=0)
    doupdate();
  return freeze;
}

#endif

int WinOpen(PWIN *pw,PWIN *w,int row,int column,int high,int wide)
{

#ifdef USE_CURSES

  if (pw==NULL)
  {
    w->win=newwin(high,wide,row,column);
    if (w->win==NULL)
      return(-1);
#ifdef USE_CURSES_PANEL
    WinLog(w);
#else
    WinLog(&screen,w);
#endif
  }
  else
  {
    row=pw->row+row;
    column=pw->column+column;
    w->win=subwin(pw->win,high,wide,row,column);
    if (w->win==NULL)
      return(-1);
#ifndef USE_CURSES_PANEL
    WinLog(pw,w);
#else
/**
    w->panel=NULL;
**/
    WinLog(w);
#endif
  };

  keypad(w->win,TRUE);
  wattrset(w->win,A_NORMAL);
  w->wait_mode=-1;

#else
#ifdef USE_UITS

  if (pw != NULL)
  {
    row = pw->row + row;
    column = pw->column + column;
  }
  w->win = crwin(row, column, high, wide, high, wide);
  if (w->win==NULL)
    return(-1);
  wsattr(w->win,WHITE,BLACK,NORMAL);
  wcuronoff(w->win);
  wcls(w->win);
  WinFlush(w);

#endif
#endif

  w->row=row;
  w->column=column;
  w->high=high;
  w->wide=wide;
  return(0);
}

int WinScrollOk(PWIN *w)
{

#ifdef USE_CURSES

  scrollok(w->win,TRUE);

#else
#ifdef USE_UITS

  wscrollon(w->win);

#endif
#endif

  return(0);
}

int WinDrawBox(PWIN *w)
{
#ifdef USE_CURSES

#ifndef HANZI_BOX
  wattrset(w->win,A_NORMAL);
  box(w->win,0,0);
#else
  int i,j,width;
  if ((w->wide % 2)==0)
    width = w->wide;
  else
    width = w->wide-1;
  if ((width<4) || (w->high<3))
    return -1;
  WinDrawString(w,"©°",0,0,A_NORMAL);
  WinDrawString(w,"©¼",w->high-1,width-2,A_NORMAL);
  WinDrawString(w,"©´",0,width-2,A_NORMAL);
  WinDrawString(w,"©¸",w->high-1,0,A_NORMAL);
  for(j=2;j<width-2;j+=2)
  {
    WinDrawString(w,"©¤",0,j,A_NORMAL);
    WinDrawString(w,"©¤",w->high-1,j,A_NORMAL);
  }
  for(j=1;j<w->high-1;j++)
  {
    WinDrawString(w,"©¦",j,0,A_NORMAL);
    WinDrawString(w,"©¦",j,width-2,A_NORMAL);
  }
#endif

#else
#ifdef USE_UITS

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wbox(w->win, 0, 0, w->high-1, w->wide-1);

#endif
#endif

  return(0);
}

int WinDrawBox_B(PWIN *win, char *b)
{
  int i,j,width;

  if ((win->wide % 2)==0)
    width = win->wide;
  else
    width = win->wide-1;
  if ((width<4) || (win->high<3))
    return -1;

  WinDrawString_B(win, b,"©°",0,0);
  WinDrawString_B(win, b,"©¼",win->high-1,width-2);
  WinDrawString_B(win, b,"©´",0,width-2);
  WinDrawString_B(win, b,"©¸",win->high-1,0);

  for(j=2;j<width-2;j+=2)
  {
    WinDrawString_B(win, b,"©¤",0,j);
    WinDrawString_B(win, b,"©¤",win->high-1,j);
  }
  for(j=1;j<win->high-1;j++)
  {
    WinDrawString_B(win, b,"©¦",j,0);
    WinDrawString_B(win, b,"©¦",j,width-2);
  }

  return(0);
}

int WinDrawHLine(PWIN *w,int row,int column,int wide)
{

#ifdef USE_CURSES

//#define NO_WHLINE

#ifndef NO_WHLINE
  wattrset(w->win,A_NORMAL);
  wmove(w->win,row,column);
  whline(w->win,'-',wide);
  //whline_set(w->win,"©¤",wide/2);
#else
  int i ;
  for(i=0; i<wide; i+=2)
    WinDrawString(w,"©¤",row,column+i,A_NORMAL);
#endif

#else
#ifdef USE_UITS

  int i;

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wxy(w->win,row,column);
  for(i=2;i<(wide/2) - 1;i++)
    waddstr(w->win,"©¤");

#endif
#endif

  return(0);
}

int WinDrawHLine_B(PWIN *win, char *b,int row,int column,int wide)
{

  int i;
  for(i=0; i<wide; i+=2)
    WinDrawString_B(win,b,"©¤",row,column+i);

  return(0);
}

int WinDrawVLine(PWIN *w,int row,int column,int high)
{

#ifdef USE_CURSES

#ifndef NO_WVLINE
  wattrset(w->win,A_NORMAL);
  wmove(w->win,row,column);
  wvline(w->win,0,high);
#else
  int i;
  for(i=0; i<high; ++i)
    WinDrawString(w,",©¦",row+i,column,A_NORMAL);
#endif

#else
#ifdef USE_UITS

  int i;

  wsattr(w->win,WHITE,BLACK,NORMAL);
  for(i=0;i<high;i++)
  {
    wxy(w->win,row+i,column);
    waddstr(w->win,"©¦");
  }

#endif
#endif

  return(0);
}

int WinDrawVLine_B(PWIN *win, char *b,int row,int column,int high)
{

  int i;
  for(i=0; i<high; ++i)
    WinDrawString_B(win,b,",©¦",row+i,column);

  return(0);
}

int WinDrawString(PWIN *w,char *s,int row,int column,int attr)
{
#ifdef USE_CURSES
  wattrset(w->win,attr);
  mvwaddstr(w->win,row,column,s);
#else
#ifdef USE_UITS
  wsattr(w->win,WHITE,BLACK,attr);
  wxystr(w->win,row,column,s);
#endif
#endif
  return(0);
}

int WinDrawString_B(PWIN *win, char *b, char *s, int row, int column)
{

  chrcpy(&b[row * win->wide + column], s, strlen(s));

  return(0);
}

int WinDrawStringW(PWIN *w,char *s,int row,int column,int attr,int wide)
{
  char  buf[128];
  int len;
  int i;

  if (wide>127)
    wide=127;
  len=strlen(s);
  if (len>=wide)
  {
    strncpy(buf,s,wide);
    buf[wide]='\0';
    WinDrawString(w,buf,row,column,attr);
  }
  else
  {
    if (len>0)
      WinDrawString(w,s,row,column,attr);


    memset(buf,' ',wide-len);
    buf[wide-len]='\0';
    WinDrawString(w,buf,row,column+len,attr);
  };
  return(0);
}

int WinDrawStringW_B
    (PWIN *win, char *b,char *s,int row,int column,int wide)
{
  char  buf[128];
  int len;


  if (wide>127)
    wide=127;

  len=strlen(s);
  if (len>=wide)
  {
    chrcpy(buf,s,wide);
    buf[wide]='\0';

    chrcpy(&b[row * win->wide + column], buf, wide);

  }
  else
  {
    if (len>0)
      chrcpy(&b[row * win->wide + column], s, len);
  };
  return(0);
}

int WinDrawStar(PWIN *w,int row,int column,int attr,int wide,int nStar)
{
  int i;

  for(i=0;i<nStar;i++)
    WinDrawStringW(w,"*",row,column+i,attr,1);
  WinDrawStringW(w, "", row, column+nStar, attr,wide-nStar);
}

int WinFlush(PWIN *w)
{
#ifdef USE_CURSES
#ifndef USE_CURSES_PANEL
  wnoutrefresh(w->win);
  if (freeze<=0) doupdate();
#else
  WinRedrawAll();
#endif
#else
  wrefresh(w->win);
#endif
  return(0);
}

int WinRedraw(PWIN *w)
{

#ifdef USE_CURSES
  touchwin(w->win);
#endif
  WinFlush(w);
  return(0);
}

int WinToTop(PWIN *w)
{

#ifdef USE_CURSES
#ifdef USE_CURSES_PANEL
  if (w->panel!=NULL)
    top_panel(w->panel);
#else
  WinLogToTop(w);
  WinRedraw(w);
#endif
#else
  w_to_top(w->win);
  WinFlush(w);
#endif
  return(0);
}


static int getkey(PWIN *w,int wait)
{
  int key;
  int r;

#ifdef USE_CURSES

  if (wait)
  {
    if (w->wait_mode!=1)
    {
      nodelay(w->win,FALSE);
      w->wait_mode=1;
    }
  }
  else
  {
    if (w->wait_mode!=0)
    {
      r=nodelay(w->win,TRUE);
      w->wait_mode=0;
    };
  }
  
  key=wgetch(w->win);
#else
#ifdef USE_UITS
  if (wait)
    key=wgkey();
  else  key=tkey();

  if((gkeyno("c-J") == key) || (gkeyno("enter") == key))
    return KEYRETURN;
  if(gkeyno("esc") == key)
    return KEYESC;
  if(gkeyno("tab") == key)
    return KEYTAB;
  if(gkeyno("backsp") == key)
    return KEYBACKSPACE;
  if(gkeyno("down") == key)
    return KEYDOWN;
  if(gkeyno("up") == key)
    return KEYUP;
  if(gkeyno("end") == key)
    return KEYEND;
  if(gkeyno("npage") == key)
    return KEYNPAGE;
  if(gkeyno("left") == key)
    return KEYLEFT;
  if(gkeyno("right") == key)
    return KEYRIGHT;
  if(gkeyno("del") == key)
    return KEYDEL;
  if(gkeyno("ins") == key)
    return KEYINS;

#endif
#endif

  return(key);
}
int WinGetKey(PWIN *w)
{
  int key;

LOOP:
  key=getkey(w,1);
#ifdef USE_CURSES
  if (key==18||key=='')
  {
    WinForceRedraw();
    goto LOOP;
  };
#endif
  return(key);
}
int WinPeekKey(PWIN *w)
{
  int   key;

LOOP:
  key=getkey(w,0);
#ifdef USE_CURSES
  if (key==18||key=='')
  {
    WinForceRedraw();
    goto LOOP;
  };
#endif
  return(key);
}


int WinClear(PWIN *w)
{

#ifdef USE_CURSES

  waddch(w->win,' '|A_NORMAL);
  wnoutrefresh(w->win);
  werase(w->win);
  WinFlush(w);

#else
#ifdef USE_UITS

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wrefresh(w->win);
  wcls(w->win);

#endif
#endif

  return(0);
}

int WinClose(PWIN *w)
{

#ifdef USE_CURSES
  waddch(w->win,' '|A_NORMAL);
  wnoutrefresh(w->win);
  werase(w->win);
  wnoutrefresh(w->win);
  delwin(w->win);
//LOG_TRACE("delwin end.");
  WinUnLog(w);
//LOG_TRACE("WinUnLog end.");
  WinRedrawAll();
//LOG_TRACE("WinRedrawAll end.");
#else
#ifdef USE_UITS

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wrefresh(w->win);
  dewin(w->win);

#endif
#endif

  w->win=NULL;
  return(0);
}

int WinScrollUp(PWIN *w,int startRow)
{

#ifdef USE_CURSES

  wmove(w->win,startRow,0);
  wdeleteln(w->win);
#else
#ifdef USE_UITS

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wrefresh(w->win);
  wscroll(w->win,1,startRow,w->high-1);

#endif
#endif

  return(0);
}

int WinScrollDown(PWIN *w,int startRow)
{

#ifdef USE_CURSES

  wmove(w->win,startRow,0);
  winsertln(w->win);

#else
#ifdef USE_UITS

  wsattr(w->win,WHITE,BLACK,NORMAL);
  wrefresh(w->win);
  wscroll(w->win,-1,startRow,w->high-1);

#endif
#endif

  return(0);
}

int WinCursorOn(PWIN *w)
{

#ifdef USE_CURSES


#else
#ifdef USE_UITS

  wcuronon(w->win);

#endif
#endif

  return(0);
}

int WinSetCursor(PWIN *w,int row,int column)
{

#ifdef USE_CURSES

  wmove(w->win,row,column);

#else
#ifdef USE_UITS

  WinCursorOn(w);
  wxy(w->win,row,column);

#endif
#endif

  return(0);
}

int WinCursorOff(PWIN *w)
{

#ifdef USE_CURSES

  WinSetCursor(w,w->high-1,w->wide-1);

#else
#ifdef USE_UITS

  wcuronoff(w->win);

#endif
#endif

  return(0);
}

int WinPrint_va(PWIN *w,int attr,char *format,va_list ap)
{
  unsigned char buf[8192];
  int i,n;

  vsprintf((char *)buf,format,ap);
  
#ifdef USE_CURSES
#define WADDCH    waddch
  wattrset(w->win,attr);
/**
  vwprintw(w->win,format,ap);
**/

#else
#ifdef USE_UITS
#define WADDCH    waddchar

  wsattr(w->win,WHITE,BLACK,attr);
/**
  wprtw(w->win,format,ap);
**/

#endif
#endif


  for(i=0,n=0; buf[i]!='\0'; ++i)
    if (buf[i]=='\t')
    {
      n=8-n%8;
      while(n>0)
      {
        WADDCH(w->win,' ');
        n--;
      };
    }
    else if (buf[i]=='\n')
    {
      n=0;
      WADDCH(w->win,'\n');
    }
    else if ((buf[i]<' ')&&(buf[i]!=8))
    {

    }
    else
    {
      n++;
      WADDCH(w->win,buf[i]);
    };
/***
  wrefresh(w->win);
***/
  return(0);
}
int WinPrint(PWIN *w,int attr,char *format,...)
{
  va_list ap;


  va_start(ap,format);
  WinPrint_va(w,attr,format,ap);
  va_end(ap);

/**
  wrefresh(w->win);
***/
  return(0);
}

int WinGetCursRowColumn(PWIN *w,int *row,int *column)
{

#ifdef USE_CURSES
  int x,y;

  getyx(w->win,y,x);
  *row=y;*column=x;
#else
#ifdef USE_UITS
  short x,y;

  wgetxy(w->win,&x,&y);
  *row=(int)y;*column=(int)x;
#endif
#endif
  return(0);
}


static struct termio   ttynew;
static struct termio   ttynew2;
static struct termio   ttynew3;
static struct termio   ttyold;
static struct termio   ttyold2;
static struct termio   ttyold3;

int WinInit()
{
  int r;

#ifdef USE_CURSES

  ioctl(STDIN_FILENO,TCGETA,&ttyold);
  ioctl(STDOUT_FILENO,TCGETA,&ttyold2);
  ioctl(STDERR_FILENO,TCGETA,&ttyold3);
  initscr(); cbreak(); noecho();
  keypad(stdscr,TRUE);
/** win_mode_set(); */
#ifdef DISABLE_TYPEAHEAD
  typeahead(-1);
#endif

#else
#ifdef USE_UITS

  winit(1,1);

#endif
#endif

  ioctl(STDIN_FILENO,TCGETA,&ttynew);
  ioctl(STDOUT_FILENO,TCGETA,&ttynew2);
  ioctl(STDERR_FILENO,TCGETA,&ttynew3);
  return(0);
}

int WinResetTTY()
{
  ioctl(STDIN_FILENO,TCSETA,&ttynew);
  ioctl(STDIN_FILENO,TCSETA,&ttynew2);
  ioctl(STDIN_FILENO,TCSETA,&ttynew3);
  return 0;
}
int WinCloseTTY()
{
  ioctl(STDIN_FILENO,TCSETA,&ttyold);
  ioctl(STDIN_FILENO,TCSETA,&ttyold2);
  ioctl(STDIN_FILENO,TCSETA,&ttyold3);
  return 0;
}

int WinEnd()
{

#ifdef USE_CURSES
/***  win_mode_restore(); ***/
  nocbreak();
  echo();
  endwin();
  fflush(stdout);
  setvbuf(stdout,NULL,_IOLBF,BUFSIZ);

#else
#ifdef USE_UITS

  endwin();

#endif
#endif

  return(0);
}

int WinBeep()
{
  beep();
  fflush(stdout);
  return(0);
}

