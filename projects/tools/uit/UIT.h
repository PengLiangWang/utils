#ifndef _UIT_INCLUDE_H_
#define _UIT_INCLUDE_H_
#include <stddef.h>
#include "WinUtil.h"
#include "UitFormEdit.h"


#define A_TITLE   A_NORMAL
/**
#define A_TILLE   A_STANDOUT
**/

#define UF_INT      1
#define UF_UINT     2
#define UF_SHORT    3
#define UF_USHORT   4
#define UF_STR      5
#define UF_NSTR     6
#define UF_PASSWD   7
#define UF_DOUBLE   8
#define UF_SELECT   9
#define UF_YMD      10
#define UF_HMS      11
#define UF_LONG     12
#define UF_ULONG    13
#define UF_BUTTON   14

#define UFO_NOTNULL   1
#define UFO_NOTZERO   2
#define UFO_FIXLEN    4
#define UFO_DISPLAYONLY   8
#define UFO_HIDDEN    16
#define UFO_SUBMIT    32
#define UFO_PLUS    64
#define UFO_MINUS   128
#define UFO_NOTPLUS   256
#define UFO_NOTMINUS    512

#define UIT_MSGWIN_NO     0
#define UIT_TITLEWIN_NO     0
#define UIT_BORDER_NO     0
#define UIT_INPUTWIN_NO     0

#define UIT_BORDER_YES      1
#define UIT_TITLEWIN_YES    2
#define UIT_MSGWIN_YES      4
#define UIT_INPUTWIN_YES    8

typedef struct
  {
    char      *title;
    char      *rem;
    struct UitMenu_t  *submenu;
    int     p_row,p_col;
    int     ID;
    char      key;
  } UitMenuItemList;
typedef struct
  {
    char      key;
    int     ID;
    struct UitMenu_t  *submenu;
  } UitMenuHotKeyList;
typedef struct UitMenu_t
  {
    char      *name;
    char      *title;
    UitMenuItemList   *item;
    UitMenuHotKeyList *hotkey;
    int     p_row,p_col;
    int     s_row,s_col;
    char      **text;
    long      win_option;

    int     index;
    PWIN      win;
    int     top,bottom;
    int     left;
    int     msg_line;
  } UitMenu;

typedef struct
  {
    int     index;
    int     ID;
    char      *str;
  } UF_Select;

typedef struct
  {
    int     ID;
    char      *str;
  } UitFormFieldOption;
typedef struct
  {
    long      offset;
    int     type;
    char      *name;
    char      *title;
    char      *rem;
    int     size,dec,displaysize;
    int     p_row,p_col;
    int     f_option;
    int     nouse;
    UitFormFieldOption  *option;
  } UitFormFieldList;
typedef struct
  {
    char      *name;
    char      *title;
    UitFormFieldList  *field;
    int     p_row,p_col;
    int     s_row,s_col;
    char      **text;
    long      win_option;

    int     index;
    PWIN      win;
    int     top,bottom;
    int     left;
    int     msg_line;
  } UitFormCtrl;

int UitPrintWinText(PWIN *win,int top,int left,char **text);

int GetUitMenu(UitMenu *menu,int (*func)(int ID));
int DrawUitMenu(UitMenu *menu);
int OpenUitForm(UitFormCtrl *form,void *data);
int DrawUitForm(UitFormCtrl *form,void *data);
int GetUitForm(UitFormCtrl *form,void *data,
    int (*func)(UitFormCtrl *form,void *data));
int PromptUitForm(UitFormCtrl *form, char *prompt);
int CloseUitForm(UitFormCtrl *form);
int UitForm_SetItemOptionIndexByID(UitFormCtrl *form,void *data,
           int itemIndex,int optionID);

#endif
