#ifndef _UIT_test_H_
#define _UIT_test_H_
#include <UIT.h>


/**********************************************************
 * UIT 编译工具 Version 1.0                               *
 * 源 文 件: test.uit                                     *
 * 日    期: Mon Jun  2 19:43:18 2014                     *
 **********************************************************/


extern UitMenu UitMenu_SubMenu_1;
extern UitMenu UitMenu_Query_Menu;
extern UitMenu UitMenu_Test_Menu;
typedef struct {
	char	field_1[17];
	UF_Select	field_2;
	char	hidden[33];
	double	field_3;
	char	field_4[21];
	char	date_str[11];
	char	time_str[10];
	long	long_value;
	UF_Button	button;
	} Test_Form;
#define UFD_Test_Form_field_1	0
#define UFD_Test_Form_field_2	1
#define UFD_Test_Form_hidden	2
#define UFD_Test_Form_field_3	3
#define UFD_Test_Form_field_4	4
#define UFD_Test_Form_date_str	5
#define UFD_Test_Form_time_str	6
#define UFD_Test_Form_long_value	7
#define UFD_Test_Form_button	8
extern UitFormCtrl UitFormCtrl_Test_Form;
#endif
