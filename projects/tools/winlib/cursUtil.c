#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cursUtil.h"
#ifdef USE_CURSES
#define BOX_WIDE  1
#else
#define BOX_WIDE  2
#endif

static int CursMutiMenuItem_print(CursMutiMenu *menu, int n, int attr)
{
  if(menu->item[n].had)
    WinDrawString(&menu->subwin,"+",n - menu->topItem,1,attr);
  else if(menu->item[n].selected)
    WinDrawString(&menu->subwin,"*",n - menu->topItem,1,attr);
  else
    WinDrawString(&menu->subwin," ",n - menu->topItem,1,attr);
  WinDrawString(&menu->subwin,menu->item[n].name,n-menu->topItem,2,attr);
}

int CursMutiMenu_open(CursMutiMenu *menu)
{
  int len;
  int x;
  int i;
  int r;

  r=WinOpen(NULL,&menu->win,menu->y,menu->x,menu->nlines,menu->ncols);
  if (r!=0)
    return(-1);
  r=WinOpen(&menu->win,&menu->subwin,3,BOX_WIDE,
        menu->nlines-4,menu->ncols-2*BOX_WIDE);
  if (r!=0)
  {
    WinClose(&menu->win);
    return(-1);
  };
  WinScrollOk(&menu->subwin);
  if(menu->currentItem>=menu->itemN)  /*YJS*/
    menu->currentItem=menu->itemN-1;
  if(menu->topItem>=menu->itemN)  /*YJS*/
    menu->topItem=menu->itemN-1;
  if ((menu->currentItem-menu->topItem)>=(menu->nlines-4))
    menu->topItem=menu->currentItem-(menu->nlines-4)+1;

  WinDrawBox(&menu->win);
  WinDrawHLine(&menu->win,2,BOX_WIDE,menu->ncols-2*BOX_WIDE);
  len=strlen(menu->title);
  x=(menu->ncols-len)/2;
  if (x<0) x=0;
  WinDrawString(&menu->win,menu->title,1,x,A_STANDOUT);
  for(i=menu->topItem; (i<menu->itemN)
        &&((i-menu->topItem)<menu->nlines-4); ++i)
    CursMutiMenuItem_print(menu, i, A_NORMAL);
  CursMutiMenuItem_print(menu, menu->currentItem, A_REVERSE);
  WinFlush(&menu->win);
  WinFlush(&menu->subwin);
  return(0);
}

int CursMutiMenu_redraw(CursMutiMenu *menu)
{
  WinRedraw(&menu->win);
  return(0);
}

int CursMutiMenu_input(CursMutiMenu *menu)
{
  int key;
  int r=MUTIMENUACTION_NOP;

  key=WinGetKey(&menu->win);
  switch(key)
  {
    case '\r':
    case '\n':
      if(menu->item[menu->currentItem].had)
      {
        beep();
        break;
      }
      if(menu->item[menu->currentItem].selected)
      {
        menu->item[menu->currentItem].selected=0;
        r=MUTIMENUACTION_UNSELECT;
      }
      else
      {
        menu->item[menu->currentItem].selected=1;
        r=MUTIMENUACTION_SELECT;
      }
      CursMutiMenuItem_print(menu, menu->currentItem,
          A_REVERSE);
      WinFlush(&menu->subwin);
      break;
    case KEYUP:
      if (menu->currentItem>0)
      {
        CursMutiMenuItem_print(menu, menu->currentItem,
            A_NORMAL);
        --menu->currentItem;
        if (menu->currentItem<menu->topItem)
        {
          --menu->topItem;
          WinScrollDown(&menu->subwin,1);
        };
        CursMutiMenuItem_print(menu, menu->currentItem,
            A_REVERSE);
        WinFlush(&menu->subwin);
      }
      else
      { WinBeep();
        WinFlush(&menu->subwin);
      };
      break;
    case KEYDOWN:
      if (menu->currentItem<(menu->itemN-1))
      {
        CursMutiMenuItem_print(menu, menu->currentItem,
            A_NORMAL);
        ++menu->currentItem;
        if ((menu->currentItem-menu->topItem)
            >=menu->nlines-4)
        {
          ++menu->topItem;
          WinScrollUp(&menu->subwin,1);
        };
        CursMutiMenuItem_print(menu, menu->currentItem,
            A_REVERSE);
        WinFlush(&menu->subwin);
      }
      else
      { WinBeep();
        WinFlush(&menu->subwin);
      };
      break;
    case KEYEND:
    case 6:
      r=MUTIMENUACTION_END;
      break;
    case 27:
    case 5:
      r=MUTIMENUACTION_CANCEL;
      break;
    case 12:  /* ^L */
      CursMutiMenu_redraw(menu);
      break;
    default:
      WinBeep();
      WinFlush(&menu->subwin);
      WinFlush(&menu->subwin);
      break;
  };
  return(r);
}

int CursMutiMenu_close(CursMutiMenu *menu)
{
  WinClose(&menu->subwin);
  WinClose(&menu->win);
  return(0);
}

static int CursMenuItem_print(CursMenu *menu,int n,int attr)
{
  if (strncmp(menu->item[n],"----",4)==0)
    WinDrawHLine(&menu->subwin,n-menu->topItem,0,
          strlen(menu->item[n])+2);
  else WinDrawString(&menu->subwin,
        menu->item[n], n-menu->topItem,
        1,attr);
  return(0);
}

int CursMenu_open(CursMenu *menu)
{
  int len;
  int x;
  int i;
  int r;

  r=WinOpen(NULL,&menu->win,menu->y,menu->x,menu->nlines,menu->ncols);
  if (r!=0)
    return(-1);
  r=WinOpen(&menu->win,&menu->subwin,3,BOX_WIDE,
        menu->nlines-4,menu->ncols-2*BOX_WIDE);
  if (r!=0)
  {
    WinClose(&menu->win);
    return(-1);
  };
  WinScrollOk(&menu->subwin);
  if(menu->currentItem>=menu->itemN)  /*YJS*/
    menu->currentItem=menu->itemN-1;
  if(menu->topItem>=menu->itemN)  /*YJS*/
    menu->topItem=menu->itemN-1;
  if ((menu->currentItem-menu->topItem)>=(menu->nlines-4))
    menu->topItem=menu->currentItem-(menu->nlines-4)+1;

  WinDrawBox(&menu->win);
  WinDrawHLine(&menu->win,2,BOX_WIDE,menu->ncols-2*BOX_WIDE);
  len=strlen(menu->title);
  x=(menu->ncols-len)/2;
  if (x<0) x=0;
  WinDrawString(&menu->win,menu->title,1,x,A_STANDOUT);
  for(i=menu->topItem; (i<menu->itemN)
        &&((i-menu->topItem)<menu->nlines-4); ++i)
    CursMenuItem_print(menu,i,A_NORMAL);
  CursMenuItem_print(menu,menu->currentItem,A_REVERSE);
  WinFlush(&menu->win);
  WinFlush(&menu->subwin);
  return(0);
}

int CursMenu_redraw(CursMenu *menu)
{
  WinRedraw(&menu->subwin);
  WinRedraw(&menu->win);
  return(0);
}

int CursMenu_input(CursMenu *menu)
{
  int key;
  int r=MENUACTION_NOP;
  int g;

  key=WinGetKey(&menu->win);
  switch(key)
  {
    case 6:
    case '\r':
    case '\n':
      r=MENUACTION_SELECT;
      break;
    case KEYUP:
      g=1;
      while(g)
         if (menu->currentItem>0)
         {
        CursMenuItem_print(menu,menu->currentItem,
                A_NORMAL);
        --menu->currentItem;
        if (menu->currentItem<menu->topItem)
        {
          --menu->topItem;
          WinScrollDown(&menu->subwin,1);
        };
        CursMenuItem_print(menu,menu->currentItem,
                A_REVERSE);
        if (strncmp(menu->item[menu->currentItem],
                "----",4)==0)
              g=1;
            else  g=0;
         }
         else
         {  WinBeep();
        g=0;
         };
      WinFlush(&menu->subwin);
      break;
    case KEYDOWN:
      g=1;
      while(g)
         if (menu->currentItem<(menu->itemN-1))
         {
        CursMenuItem_print(menu,menu->currentItem,
                A_NORMAL);
        ++menu->currentItem;
        if ((menu->currentItem-menu->topItem)
            >=menu->nlines-4)
        {
          ++menu->topItem;
          WinScrollUp(&menu->subwin,1);
        };
        CursMenuItem_print(menu,menu->currentItem,
                A_REVERSE);
        if (strncmp(menu->item[menu->currentItem],
                "----",4)==0)
              g=1;
            else  g=0;
         }
         else
         {  WinBeep();
        g=0;
         };
      WinFlush(&menu->subwin);
      break;
    case KEYEND:
    case 5:
    case 27:
      r=MENUACTION_END;
      break;
    case 12:  /* ^L */
      CursMenu_redraw(menu);
      break;
    default:
      WinBeep();
      WinFlush(&menu->subwin);
      break;
  };
  return(r);
}

int CursMenu_close(CursMenu *menu)
{
  WinClose(&menu->subwin);
  WinClose(&menu->win);
  return(0);
}


static int inputItem_drawInput(PWIN *win,CursInputItem *item,int attr)
{
  int x,i;

  x=item->col+strlen(item->title);
  WinDrawStringW(win,"",item->line,x,A_NORMAL,item->wide);
  if (item->echo)
    WinDrawString(win,item->data,item->line,x,attr);
  else
  {
    for(i=0; item->data[i]!='\0'; ++i)
      WinDrawString(win,"*",item->line,x+i,attr);
    if (i==0)
      WinDrawString(win,item->data,item->line,x,attr);
  };
  return(0);
}

int inputItem_draw(PWIN *win,CursInputItem *item,int attr)
{
  WinDrawString(win,item->title,item->line,item->col,A_NORMAL);
  inputItem_drawInput(win,item,attr);
  return(0);
}

static int inputItem_delCh(PWIN *win,CursInputItem *item)
{
  if (item->n>0)
  {
    --item->n;
    item->data[item->n]='\0';
    inputItem_drawInput(win,item,A_REVERSE);
  }
  else  WinBeep();
  WinFlush(win);
  return(0);
}

static int inputItem_addCh(PWIN *win,CursInputItem *item,int key)
{
  if ((key>=' ')&&(key<255)&&(item->n<(item->wide-1)))
  {
    item->data[item->n]=(char)key;
    ++item->n;
    item->data[item->n]='\0';
    inputItem_drawInput(win,item,A_REVERSE);
  }
  else  WinBeep();
  WinFlush(win);
  return(0);
}

int CursInputWindow_open(CursInputWindow *win)
{
  int len;
  int x;
  int i;
  int r;

  r=WinOpen(NULL,&win->win,win->y,win->x,win->nlines,win->ncols);
  if (r!=0)
    return(-1);
  WinDrawBox(&win->win);
  WinDrawHLine(&win->win,2,BOX_WIDE,win->ncols-2*BOX_WIDE);
  len=strlen(win->title);
  x=(win->ncols-len)/2;
  if (x<0) x=0;
  WinDrawString(&win->win,win->title,1,x,A_STANDOUT);
  for(i=0; i<win->itemN; ++i)
    inputItem_draw(&win->win,&win->item[i],A_NORMAL);
  inputItem_draw(&win->win,&win->item[win->currentItem],A_REVERSE);
  win->item[win->currentItem].n=strlen(win->item[win->currentItem].data);
  WinFlush(&win->win);
  return(0);
}

int CursInputWindow_redraw(CursInputWindow *win)
{
  WinRedraw(&win->win);
  return(0);
}

int CursInputWindow_input(CursInputWindow *win)
{
  int key;
  int r=MENUACTION_NOP;

  key=WinGetKey(&win->win);
  switch(key)
  {
    case '\r':
    case '\n':
    case '\t':
    case KEYDOWN:
      r=INPUTACTION_NEXTRECORD;
      inputItem_draw(&win->win,
        &win->item[win->currentItem],A_NORMAL);
      ++win->currentItem;
      if (win->currentItem>=win->itemN)
            win->currentItem=0;
      inputItem_draw(&win->win,
        &win->item[win->currentItem],A_REVERSE);
      win->item[win->currentItem].n
        =strlen(win->item[win->currentItem].data);
      WinFlush(&win->win);
      break;

    case KEYUP:
      r=INPUTACTION_NEXTRECORD;
      inputItem_draw(&win->win,
        &win->item[win->currentItem],A_NORMAL);
      --win->currentItem;
      if (win->currentItem<0)
          win->currentItem=win->itemN-1;
      inputItem_draw(&win->win,
        &win->item[win->currentItem],A_REVERSE);
      win->item[win->currentItem].n
        =strlen(win->item[win->currentItem].data);
      WinFlush(&win->win);
      break;
    case KEYEND:
    case 6:
      r=INPUTACTION_END;
      break;
    case 5:
    case 27:
      r=INPUTACTION_CANCEL;
      break;
    case 8:
    case KEYBACKSPACE:
    case KEYLEFT:
      if ((win->disableInput==0)
          &&(win->item[win->currentItem].disableInput==0))
        inputItem_delCh(&win->win,
          &win->item[win->currentItem]);
      break;
    case 12:  /* ^L */
      CursInputWindow_redraw(win);
      break;
    default:
      if ((win->disableInput==0)
          &&(win->item[win->currentItem].disableInput==0))
        inputItem_addCh(&win->win,
          &win->item[win->currentItem],key);
      break;
  };
  return(r);
}

int CursInputWindow_close(CursInputWindow *win)
{
  WinClose(&win->win);
  return(0);
}


int CursMsgWindow_attrset(CursMsgWindow *win,int attr)
{
  win->attr=attr;
}

int CursMsgWindow_printw(CursMsgWindow *win,char *format,...)
{
  va_list ap;

  va_start(ap,format);
  WinPrint_va(&win->subwin,win->attr,format,ap);
  va_end(ap);
/**
  WinFlush(&win->subwin);
**/
  return(0);
}

int CursMsgWindow_open(CursMsgWindow *win)
{
  int len;
  int x;
  int i;
  int r;

  r=WinOpen(NULL,&win->win,win->y,win->x,win->nlines,win->ncols);
  if (r!=0)
    return(-1);
  r=WinOpen(&win->win,&win->subwin,3,BOX_WIDE,
        win->nlines-4,win->ncols-2*BOX_WIDE);
  if (r!=0)
  {
    WinClose(&win->win);
    return(-1);
  };
  WinScrollOk(&win->subwin);

  WinDrawBox(&win->win);
  WinDrawHLine(&win->win,2,BOX_WIDE,win->ncols-2*BOX_WIDE);
  len=strlen(win->title);
  x=(win->ncols-len)/2;
  if (x<0) x=0;
  WinDrawString(&win->win,win->title,1,x,A_STANDOUT);
  WinFlush(&win->win);
  WinFlush(&win->subwin);
  return(0);
}

int CursMsgWindow_redraw(CursMsgWindow *win)
{
  WinRedraw(&win->win);
  return(0);
}

int CursMsgWindow_input(CursMsgWindow *win)
{
  int key;

  WinFlush(&win->subwin);
  key=WinGetKey(&win->win);
  if(key==MSGACTION_REFRESH || key==12) /* ^L */
  {
    key=MSGACTION_REFRESH;
    CursMsgWindow_redraw(win);
  }
  return(key);
}

int CursMsgWindow_close(CursMsgWindow *win)
{
  WinClose(&win->subwin);
  WinClose(&win->win);
  return(0);
}

static int CursMenuBarItem_close(CursMenuBarItem *menu)
{
  WinClose(&menu->subwin);
  WinClose(&menu->win);
  return(0);
}
static int CursMenuBarItem_print(CursMenuBarItem *menu,int n,int attr)
{
  if (strncmp(menu->item[n],"----",4)==0)
    WinDrawHLine(&menu->subwin,n-menu->topItem,0,
          strlen(menu->item[n])+2);
  else WinDrawString(&menu->subwin,
        menu->item[n], n-menu->topItem,
        1,attr);
  return(0);
}
static int CursMenuBarItem_open(CursMenuBarItem *menu,
        PWIN *pwin,int row,int column)
{
  int r;
  int i;

  r=WinOpen(pwin,&menu->win,row,column,menu->nlines,menu->ncols);
  if (r!=0)
    return(-1);
  r=WinOpen(&menu->win,&menu->subwin,1,BOX_WIDE,
        menu->nlines-2,menu->ncols-2*BOX_WIDE);
  if (r!=0)
  {
    WinClose(&menu->win);
    return(-1);
  };
  WinDrawBox(&menu->win);
  WinScrollOk(&menu->subwin);
  if(menu->currentItem>=menu->itemN)  /*YJS*/
    menu->currentItem=menu->itemN-1;
  if(menu->topItem>=menu->itemN)  /*YJS*/
    menu->topItem=menu->itemN-1;
  if ((menu->currentItem-menu->topItem)>=(menu->nlines-2))
    menu->topItem=menu->currentItem-(menu->nlines-2)+1;

  for(i=menu->topItem; (i<menu->itemN)
        &&((i-menu->topItem)<menu->nlines-2); ++i)
    CursMenuBarItem_print(menu,i,A_NORMAL);
  CursMenuBarItem_print(menu,menu->currentItem,A_REVERSE);
  WinFlush(&menu->win);
  WinFlush(&menu->subwin);
  return(0);
}

int CursMenuBar_redraw(CursMenuBar *menu)
{
  WinRedraw(&menu->item[menu->currentItem].subwin);
  WinRedraw(&menu->item[menu->currentItem].win);
  WinRedraw(&menu->win);
  return(0);
}

static int CursMenuBarItem_up(CursMenuBarItem *menu)
{
  int g;

  g=1;
  while(g)
     if (menu->currentItem>0)
     {
    CursMenuBarItem_print(menu,menu->currentItem,A_NORMAL);
    --menu->currentItem;
    if (menu->currentItem<menu->topItem)
    {
      --menu->topItem;
      WinScrollDown(&menu->subwin,1);
    };
    CursMenuBarItem_print(menu,menu->currentItem,A_REVERSE);
    if (strncmp(menu->item[menu->currentItem], "----",4)==0)
          g=1;
        else  g=0;
     }
     else
     {  WinBeep();
    g=0;
     };
  WinFlush(&menu->subwin);
  return(0);
}

static int CursMenuBarItem_down(CursMenuBarItem *menu)
{
  int g=1;

  while(g)
     if (menu->currentItem<(menu->itemN-1))
     {
    CursMenuBarItem_print(menu,menu->currentItem,A_NORMAL);
    ++menu->currentItem;
    if ((menu->currentItem-menu->topItem)
        >=menu->nlines-2)
    {
      ++menu->topItem;
      WinScrollUp(&menu->subwin,1);
    };
    CursMenuBarItem_print(menu,menu->currentItem,A_REVERSE);
    if (strncmp(menu->item[menu->currentItem], "----",4)==0)
          g=1;
        else  g=0;
     }
     else
     {  WinBeep();
    g=0;
     };
  WinFlush(&menu->subwin);
  return(0);
}
int CursMenuBar_input(CursMenuBar *menu)
{
  int key;
  int r=MENUACTION_NOP;
  int g;

  key=WinGetKey(&menu->win);
  switch(key)
  {
    case 6:
    case '\r':
    case '\n':
      r=MENUACTION_SELECT;
      break;
    case KEYUP:
      CursMenuBarItem_up(&menu->item[menu->currentItem]);
      break;
    case KEYDOWN:
      CursMenuBarItem_down(&menu->item[menu->currentItem]);
      break;
    case KEYLEFT:
      if (menu->currentItem>0)
      {
        CursMenuBarItem_close(
            &menu->item[menu->currentItem]);
        WinDrawString(&menu->win,
          menu->item[menu->currentItem].title,
          3,menu->itemWide*menu->currentItem+2,
          A_NORMAL);
        --menu->currentItem;
        WinDrawString(&menu->win,
          menu->item[menu->currentItem].title,
          3,menu->itemWide*menu->currentItem+2,
          A_REVERSE);
        CursMenuBarItem_open(
            &menu->item[menu->currentItem],
            &menu->win,4,
            menu->itemWide
              *menu->currentItem+2);
      }
      else  WinBeep();
      WinFlush(&menu->win);
      break;

    case KEYRIGHT:
      if (menu->currentItem<menu->itemN-1)
      {
        CursMenuBarItem_close(
            &menu->item[menu->currentItem]);
        WinDrawString(&menu->win,
          menu->item[menu->currentItem].title,
          3,menu->itemWide*menu->currentItem+2,
          A_NORMAL);
        ++menu->currentItem;
        WinDrawString(&menu->win,
          menu->item[menu->currentItem].title,
          3,menu->itemWide*menu->currentItem+2,
          A_REVERSE);
        CursMenuBarItem_open(
            &menu->item[menu->currentItem],
            &menu->win,4,
            menu->itemWide
              *menu->currentItem+2);
      }
      else  WinBeep();
      WinFlush(&menu->win);
      break;

    case KEYEND:
    case 5:
    case 27:
      r=MENUACTION_END;
      break;
    case 12:  /* ^L */
      CursMenuBar_redraw(menu);
      break;
    default:
      WinBeep();
      WinFlush(&menu->win);
      break;
  };
  return(r);
}

int CursMenuBar_open(CursMenuBar *menu)
{
  int len;
  int x;
  int i;
  int r;

  r=WinOpen(NULL,&menu->win,menu->y,menu->x,menu->nlines,menu->ncols);
  if (r!=0)
    return(-1);
  WinDrawBox(&menu->win);
  WinDrawHLine(&menu->win,2,BOX_WIDE,menu->ncols-2*BOX_WIDE);
  len=strlen(menu->title);
  x=(menu->ncols-len)/2;
  if (x<0) x=0;
  WinDrawString(&menu->win,menu->title,1,x,A_STANDOUT);
  for(i=0; i<menu->itemN; ++i)
    WinDrawString(&menu->win,menu->item[i].title,
          3,2+menu->itemWide*i,A_NORMAL);
  WinDrawString(&menu->win,menu->item[menu->currentItem].title,
        3,menu->itemWide*menu->currentItem+2,A_REVERSE);
  r=CursMenuBarItem_open(&menu->item[menu->currentItem],
      &menu->win,4,menu->itemWide*menu->currentItem+2);
  if (r!=0)
  {
    WinClose(&menu->win);
    return(-1);
  };
  WinFlush(&menu->win);
  return(0);
}

int CursMenuBar_close(CursMenuBar *menu)
{
  CursMenuBarItem_close(&menu->item[menu->currentItem]);
  WinClose(&menu->win);
  return(0);
}

int CursInit()
{
  return(WinInit());
}

int CursEnd()
{
  return(WinEnd());
}

