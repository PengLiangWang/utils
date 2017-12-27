#ifndef _WIN_SCREEN_INCLUDE_H_
#define _WIN_SCREEN_INCLUDE_H_
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
#define USE_UITS
*/
#define USE_CURSES
//#define USE_CURSES_PANEL

#ifdef USE_CURSES

#ifndef HPUX
#define BOXWIDE   1
#else
#define BOXWIDE   2
#endif

#include "curses.h"
/*#include "ocurses.h"*/
#ifdef USE_CURSES_PANEL
#include "panel.h"
#endif


#define KEYRETURN '\n'
#define KEYESC    27
#define KEYTAB    '\t'
#define KEYBACKSPACE  KEY_BACKSPACE
#define KEYDOWN   KEY_DOWN
#define KEYUP   KEY_UP
#define KEYEND    KEY_END
#define KEYLEFT   KEY_LEFT
#define KEYRIGHT  KEY_RIGHT
#define KEYDEL    KEY_DC
#define KEYINS    KEY_IC
#define KEYNPAGE  KEY_NPAGE
#define KEYPPAGE  KEY_PPAGE

typedef struct PWIN_T
  {
    int row,column,high,wide;
    WINDOW  *win;
#ifdef USE_CURSES_PANEL
    PANEL *panel;
#else
    struct PWIN_T *p,*n,*sub,*f;
#endif
    int wait_mode;
  } PWIN;

#else
#ifdef USE_UITS

#define BOXWIDE   2

#include <win.h>

#define A_NORMAL  NORMAL
#define A_REVERSE INVERSE
#define A_STANDOUT  HIGHLIGHT

#define KEYRETURN '\n'
#define KEYESC    27
#define KEYTAB    '\t'
#define KEYBACKSPACE  260
#define KEYDOWN   261
#define KEYUP   262
#define KEYEND    263
#define KEYLEFT   264
#define KEYRIGHT  265
#define KEYDEL    266
#define KEYINS    267
#define KEYNPAGE  268

typedef struct
  {
    int row,column,high,wide;
    WIN *win;
  } PWIN;   /* Private WIN */

#endif
#endif

int WinFreeze();
int WinThaw();
int WinRedrawAll();
int WinForceRedraw();
int WinOpen(PWIN *pw,PWIN *w,int row,int column,int high,int wide);
int WinScrollOk(PWIN *w);
int WinDrawBox(PWIN *w);
int WinDrawBox_B(PWIN *win, char *b);
int WinDrawHLine(PWIN *w,int row,int column,int wide);
int WinDrawHLine_B(PWIN *win, char *b,int row,int column,int wide);
int WinDrawVLine(PWIN *w,int row,int column,int high);
int WinDrawVLine_B(PWIN *win, char *b,int row,int column,int high);
int WinDrawString(PWIN *w,char *s,int row,int column,int attr);
int WinDrawString_B(PWIN *win, char *b, char *s, int row, int column);
int WinDrawStringW(PWIN *w,char *s,int row,int column,int attr,int wide);
int WinDrawStar(PWIN *w,int row,int column,int attr,int wide,int nStar);
int WinFlush(PWIN *w);
int WinRedraw(PWIN *w);
int WinToTop(PWIN *w);
int WinGetKey(PWIN *w);
int WinPeekKey(PWIN *w);
int WinClear(PWIN *w);
int WinClose(PWIN *w);
int WinScrollUp(PWIN *w,int startRow);
int WinScrollDown(PWIN *w,int startRow);
int WinCursorOn(PWIN *w);
int WinSetCursor(PWIN *w,int row,int column);
int WinCursorOff(PWIN *w);
int WinPrint_va(PWIN *w,int attr,char *format,va_list ap);
int WinPrint(PWIN *w,int attr,char *format,...);
int WinGetCursRowColumn(PWIN *w,int *row,int *column);
int WinInit();
int WinResetTTY();
int WinCloseTTY();
int WinEnd();
int WinBeep();

#ifdef __cplusplus
}
#endif
#endif
