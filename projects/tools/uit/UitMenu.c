#include <stdio.h>
#include <string.h>
#include "UIT.h"

static int UitMenuItemPrint(UitMenu *menu,int index,int attr)
{
  char  s[16];

  if (menu->item[index].title==NULL)
    return(-1);

  if (menu->item[index].key=='\0')
    WinDrawString(&menu->win,menu->item[index].title,
        menu->top+menu->item[index].p_row,
        menu->left+menu->item[index].p_col,attr);
  else
  {
    sprintf(s,"<%c> ",menu->item[index].key);
    WinDrawString(&menu->win,s,
        menu->top+menu->item[index].p_row,
        menu->left+menu->item[index].p_col,A_NORMAL);
    WinDrawString(&menu->win,menu->item[index].title,
        menu->top+menu->item[index].p_row,
        menu->left+menu->item[index].p_col+4,attr);
  };
  return(0);
}
static int UitMenuItemPrintMsg(UitMenu *menu,int index)
{
  char  *rem;

  if (menu->win_option&UIT_MSGWIN_YES)
  {
    if (menu->item[index].rem==NULL)
      rem="";
    else rem=menu->item[index].rem;

    WinDrawStringW(&menu->win,rem,menu->msg_line,menu->left,
          A_NORMAL,menu->s_col-2*menu->left);
  };
  return(0);
}

int DrawUitMenu(UitMenu *menu)
{
  int r;
  int n;
  int col;

  menu->top=0;
  menu->left=0;
  menu->bottom=0;

  if (menu->win_option&UIT_BORDER_YES)
  {
    WinDrawBox(&menu->win);
    menu->top=1;menu->left=2;
    menu->bottom=menu->s_row-2;
  }
  else  menu->bottom=menu->s_row-1;

  if (menu->title!=NULL)
  {
    col=(menu->s_col-strlen(menu->title))/2;
    if (col<0)
      col=0;

    if (menu->win_option&UIT_TITLEWIN_YES)
    {
      WinDrawHLine(&menu->win,menu->top+1,BOXWIDE,
            menu->s_col-2*BOXWIDE);
      WinDrawString(&menu->win,menu->title,
          menu->top,col,A_TITLE);
      menu->top+=2;
    }
    else if (menu->win_option&UIT_BORDER_YES)
      WinDrawString(&menu->win,menu->title,0,col,A_TITLE);
  };

  if (menu->win_option&UIT_MSGWIN_YES)
  {
    menu->msg_line=menu->bottom;
    WinDrawHLine(&menu->win,menu->msg_line-1,BOXWIDE,
            menu->s_col-2*BOXWIDE);
    WinDrawStringW(&menu->win,"",
        menu->msg_line,menu->left,A_TITLE,
        menu->s_col-2*menu->left);
    menu->bottom-=2;
  };

  UitPrintWinText(&menu->win,menu->top,menu->left,menu->text);
  
  for(n=0; menu->item[n].title!=NULL; ++n)
      UitMenuItemPrint(menu,n,A_NORMAL);

  if ((menu->index>=n)||(menu->index<0))
      menu->index=0;

  UitMenuItemPrint(menu,menu->index,A_REVERSE);
  UitMenuItemPrintMsg(menu,menu->index);

  WinRedraw(&menu->win);

  return(0);
}
static int OpenUitMenu(UitMenu *menu)
{
  int r;

  r=WinOpen(NULL,&menu->win,menu->p_row,menu->p_col,
          menu->s_row,menu->s_col);
  if (r!=0)
    return(-1);

  DrawUitMenu(menu);
  return(0);
}

static int CloseUitMenu(UitMenu *menu)
{
  WinClose(&menu->win);
  return(0);
}

typedef struct  HotKeyLink_t
  {
    struct HotKeyLink_t *f;
    UitMenuHotKeyList *hotkey;
  } HotKeyLink;

static int GetUitMenuWithHotKeyLink(
    UitMenu *menu,int (*func)(int ID),HotKeyLink *link_f)
{
  int   r;
  int   key;
  int   g=1,e=0;
  int   i;
  HotKeyLink  link,*p;

  link.hotkey =menu->hotkey;
  link.f    =link_f;

  r=OpenUitMenu(menu);
  if (r!=0)
    return(r);

  while(g&&e==0)
  {
      WinFlush(&menu->win);
      key=WinGetKey(&menu->win);
      switch(key)
      {
    case KEYUP:
    case KEYLEFT:
      UitMenuItemPrint(menu,menu->index,A_NORMAL);
      if (menu->index>0)
        --menu->index;
      else while (menu->item[menu->index+1].title!=NULL)
          ++menu->index;

      UitMenuItemPrint(menu,menu->index,A_REVERSE);
      UitMenuItemPrintMsg(menu,menu->index);
      break;
    case KEYDOWN:
    case KEYRIGHT:
      UitMenuItemPrint(menu,menu->index,A_NORMAL);
      if (menu->item[menu->index+1].title!=NULL)
        ++menu->index;
      else menu->index=0;
      UitMenuItemPrint(menu,menu->index,A_REVERSE);
      UitMenuItemPrintMsg(menu,menu->index);
      break;
    case 27:
    case 5:
      g=0;  break;

    case '\n':
    case '\r':
    case KEYNPAGE:
      if (menu->item[menu->index].submenu!=NULL)
      {
        r=(*func)(menu->item[menu->index].ID);
        if (r!=0)
          break;
        e=GetUitMenuWithHotKeyLink(
          menu->item[menu->index].submenu,
          func,&link);
      }
      else
        e=(*func)(menu->item[menu->index].ID);

      WinRedraw(&menu->win);
      WinFlush(&menu->win);
      break;
    default:
       for(i=0; menu->item[i].title!=NULL; i++)
        if (menu->item[i].key==(char)key)
        {
      UitMenuItemPrint(menu,menu->index,A_NORMAL);
      menu->index=i;
      UitMenuItemPrint(menu,menu->index,A_REVERSE);
      UitMenuItemPrintMsg(menu,menu->index);
      WinFlush(&menu->win);

      if (menu->item[menu->index].submenu!=NULL)
      {
        r=(*func)(menu->item[menu->index].ID);
        if (r!=0)
          break;
        e=GetUitMenuWithHotKeyLink(
          menu->item[menu->index].submenu,
          func,&link);
      }
      else
        e=(*func)(menu->item[menu->index].ID);

      WinRedraw(&menu->win);
      WinFlush(&menu->win);
      goto LOOP;
        };

      for (p=&link;p!=NULL; p=p->f)
        if (p->hotkey!=NULL)
      for(i=0; p->hotkey[i].key!='\0'; ++i)
         if ((char)key==p->hotkey[i].key)
         {
        if (p->hotkey[i].submenu!=NULL)
        {
          r=(*func)(p->hotkey[i].ID);
          if (r!=0)
            break;
          e=GetUitMenu(
            p->hotkey[i].submenu,
            func);
        }
        else
          e=(*func)(p->hotkey[i].ID);

        WinRedraw(&menu->win);
        WinFlush(&menu->win);
        goto LOOP;
         };

    WinBeep(&menu->win);
LOOP:
    break;
      };
  };

  CloseUitMenu(menu);
  if (e>0)
    --e;

  return(e);
}

int GetUitMenu(UitMenu *menu,int (*func)(int ID))
{
  int   e;
  HotKeyLink  link;

  link.hotkey =menu->hotkey;
  link.f    =NULL;

  e=GetUitMenuWithHotKeyLink(menu,func,&link);

  return(e);
}
