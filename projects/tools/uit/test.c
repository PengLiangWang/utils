
/**********************************************************
 * UIT 编译工具 Version 1.0                               *
 * 源 文 件: test.uit                                     *
 * 日    期: Mon Jun  2 19:43:18 2014                     *
 **********************************************************/


#include <stdio.h>
#include <test.h>

static UitMenuItemList UitMenuItem_SubMenu_1[]={
	{"菜单1","菜单项 1",NULL,1,2,100,'\0'},
	{"Sub Item 2","菜单项 1",NULL,3,2,101,'\0'},
	{NULL,NULL,NULL,0,0,0,'\0'}
	};
UitMenu UitMenu_SubMenu_1={
	"SubMenu_1","子菜单",
	UitMenuItem_SubMenu_1,
	NULL,
	10,20,10,30,
	NULL,
	UIT_BORDER_YES|UIT_TITLEWIN_NO|UIT_MSGWIN_YES
	};

static UitMenuItemList UitMenuItem_Query_Menu[]={
	{"Sub Item 1","菜单项 1",NULL,1,2,100,'\0'},
	{"Sub Item 2","菜单项 1",NULL,3,2,101,'\0'},
	{NULL,NULL,NULL,0,0,0,'\0'}
	};
UitMenu UitMenu_Query_Menu={
	"Query_Menu","查询菜单",
	UitMenuItem_Query_Menu,
	NULL,
	8,12,12,40,
	NULL,
	UIT_BORDER_YES|UIT_TITLEWIN_YES|UIT_MSGWIN_YES
	};

static char *UitMenuText_Test_Menu[]={
	"",
	"                        测试菜单",
	"                        UIT 自动生成",
	"",
	NULL};
static UitMenuItemList UitMenuItem_Test_Menu[]={
	{"Item 1","菜单项 1",NULL,1,2,1,'1'},
	{"Sub Menu","下级菜单",&UitMenu_SubMenu_1,3,2,0,'2'},
	{"Item 3","菜单项 3",NULL,5,2,1,'3'},
	{NULL,NULL,NULL,0,0,0,'\0'}
	};
static UitMenuHotKeyList UitMenuHotKey_Test_Menu[]={
	{'A',200,NULL},
	{'?',201,&UitMenu_Query_Menu},
	{'\0',0,NULL}
	};
UitMenu UitMenu_Test_Menu={
	"Test_Menu","Test Menu",
	UitMenuItem_Test_Menu,
	UitMenuHotKey_Test_Menu,
	4,10,15,40,
	UitMenuText_Test_Menu,
	UIT_BORDER_YES|UIT_TITLEWIN_YES|UIT_MSGWIN_YES
	};

static UitFormFieldOption UitFormFieldOption_Test_Form_field_2[]={
	{1,"选择项1"},
	{2,"选择项2"},
	{3,"选择项3"},
	{-1,NULL}
	};
static UitFormFieldList UitFormField_Test_Form[]={
	{offsetof(Test_Form,field_1),UF_STR,"field_1","输入项 1 ","请输入字符传",16,0,16,0,2,0|UFO_NOTNULL,0,NULL},
	{offsetof(Test_Form,field_2),UF_SELECT,"field_2","","选择项1 ",0,0,10,2,2,0,0,UitFormFieldOption_Test_Form_field_2},
	{offsetof(Test_Form,hidden),UF_STR,"hidden",NULL,NULL,32,0,0,2,30,0|UFO_HIDDEN,0,NULL},
	{offsetof(Test_Form,field_3),UF_DOUBLE,"field_3","输入项 3 ","请输入数字",15,2,16,4,2,0|UFO_PLUS,0,NULL},
	{offsetof(Test_Form,field_4),UF_NSTR,"field_4","输入项 N ","请输入字符传",20,0,20,6,2,0|UFO_FIXLEN,0,NULL},
	{offsetof(Test_Form,date_str),UF_YMD,"date_str","输入日期 ","格式 YYYY-MM-DD",10,0,0,0,37,0,0,NULL},
	{offsetof(Test_Form,time_str),UF_HMS,"time_str","输入时间 ","格式 HH:MM:SS",8,0,0,2,37,0,0,NULL},
	{offsetof(Test_Form,long_value),UF_LONG,"long_value","输入数字 ","七位数字",7,0,10,4,37,0|UFO_NOTMINUS,0,NULL},
	{offsetof(Test_Form,button),UF_BUTTON,"button","<确认>",NULL,0,0,0,6,37,0|UFO_SUBMIT,0,NULL},
	{-1,-1,NULL,NULL,NULL,0,0,0,0,0,0,0,NULL}
	};
static char *UitFormText_Test_Form[]={
	"─────────────────┬───────────────",
	"                                  │",
	"─────────────────┼───────────────",
	"                                  │",
	"─────────────────┼───────────────",
	"                                  │",
	"─────────────────┼───────────────",
	"                                  │",
	"─────────────────┼───────────────",
	"                                  │输入表格",
	"─────────────────┴───────────────",
	NULL};
UitFormCtrl UitFormCtrl_Test_Form={
	"Test_Form","输入窗口",
	UitFormField_Test_Form,
	7,8,15,70,
	UitFormText_Test_Form,
	UIT_BORDER_YES|UIT_TITLEWIN_YES|UIT_MSGWIN_YES|UIT_INPUTWIN_YES
	};

/******************** END ******************/
