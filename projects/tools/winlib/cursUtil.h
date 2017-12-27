#ifndef _CURSUTIL_INCLUDE_
#define _CURSUTIL_INCLUDE_
#include "WinUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cursMutiMenuItem
{
  char  *name;
  int had;
  int selected;
};

typedef struct cursMutiMenuItem CursMutiMenuItem;

struct cursMutiMenu
{
  char  *title;
  int nlines,ncols,y,x;
  int itemN;
  CursMutiMenuItem  *item;
  int currentItem;
  int topItem;

  PWIN  win;
  PWIN  subwin;
};

typedef struct cursMutiMenu CursMutiMenu;

#define MUTIMENUACTION_SELECT   1
#define MUTIMENUACTION_UNSELECT   2
#define MUTIMENUACTION_END    3
#define MUTIMENUACTION_NOP    4
#define MUTIMENUACTION_CANCEL   5

struct cursMenu
{
  char  *title;
  int nlines,ncols,y,x;
  int itemN;
  char  **item;
  int currentItem;
  int topItem;

  PWIN  win;
  PWIN  subwin;
};

typedef struct cursMenu CursMenu;

typedef struct
{
  char  *title;
  int itemN;
  char  **item;
  int nlines;
  int ncols;
  int currentItem;
  int topItem;
  
  PWIN  win;
  PWIN  subwin;
} CursMenuBarItem;


typedef struct
{
  char      *title;
  int     nlines,ncols,y,x;
  int     itemN;
  CursMenuBarItem   *item;
  int     itemWide;
  int     currentItem;

  PWIN  win;
} CursMenuBar ;

#define MENUACTION_SELECT 1
#define MENUACTION_MOVE   2
#define MENUACTION_END    3
#define MENUACTION_NOP    4

struct cursInputItem
{
  char  *title;
  char  *data;
  int line,col,wide;
  int echo;
  int disableInput;

  int n;
};

typedef struct cursInputItem CursInputItem;

struct cursInputWindow
{
  char  *title;
  int nlines,ncols,y,x;
  int itemN;
  CursInputItem  *item;
  int currentItem;
  int disableInput;

  PWIN  win;
};

typedef struct cursInputWindow  CursInputWindow;
  
#define INPUTACTION_NEXTRECORD    1
#define INPUTACTION_END     2
#define INPUTACTION_CANCEL    3

struct cursMsgWindow
{
  char  *title;
  int nlines,ncols,y,x;
  int attr;

  PWIN  win;
  PWIN  subwin;
};
typedef struct cursMsgWindow CursMsgWindow;

#define MSGACTION_REFRESH   12

int CursMutiMenu_open(CursMutiMenu *menu);
int CursMutiMenu_redraw(CursMutiMenu *menu);
int CursMutiMenu_input(CursMutiMenu *menu);
int CursMutiMenu_close(CursMutiMenu *menu);

int CursMenu_open(CursMenu *menu);
int CursMenu_redraw(CursMenu *menu);
int CursMenu_input(CursMenu *menu);
int CursMenu_close(CursMenu *menu);
int CursMenuBar_open(CursMenuBar *menu);
int CursMenuBar_redraw(CursMenuBar *menu);
int CursMenuBar_input(CursMenuBar *menu);
int CursMenuBar_close(CursMenuBar *menu);

int CursInputWindow_open(CursInputWindow *win);
int CursInputWindow_redraw(CursInputWindow *win);
int CursInputWindow_input(CursInputWindow *win);
int CursInputWindow_close(CursInputWindow *win);

int CursMsgWindow_attrset(CursMsgWindow *win,int attr);
int CursMsgWindow_printw(CursMsgWindow *win,char *format,...);
int CursMsgWindow_open(CursMsgWindow *win);
int CursMsgWindow_input(CursMsgWindow *win);
int CursMsgWindow_redraw(CursMsgWindow *win);
int CursMsgWindow_close(CursMsgWindow *win);


int CursInit();
int CursEnd();

#ifdef __cplusplus
}
#endif

#endif

