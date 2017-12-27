#include <stdio.h>
#include <fcntl.h>

#include "test.h"

Test_Form tt;

int form_check(UitFormCtrl *form, void *data)
{
  Test_Form *d;
  d=(Test_Form *)data;
  switch(form->index)
  {
  case UFD_Test_Form_field_1:
    if(strcmp(d->field_1, "1234")!=0)
    {
    WinDrawStringW(&form->win,"jjjjjjj",form->msg_line,form->left,
          A_NORMAL,form->s_col-2*form->left-1);
    WinGetKey(&form->win);
      return 1;
    }
    else
      return 0;
  }
  return 0;
}
static int func(int ID)
{
  int r;
  char  *buf;

  if (ID==100||ID==101)
  {
    UitForm_SetItemOptionIndexByID(&UitFormCtrl_Test_Form,&tt,
            UFD_Test_Form_field_2,3);
    OpenUitForm(&UitFormCtrl_Test_Form,&tt);
    r=GetUitForm(&UitFormCtrl_Test_Form,&tt,form_check);
    r=OutUitForm(&UitFormCtrl_Test_Form,&tt,&buf);
    if (r == 0){
      UitFormPrintBuf(
        "test.form", buf,
        UitFormCtrl_Test_Form.s_row,
        UitFormCtrl_Test_Form.s_col);
      free(buf);
    }
    CloseUitForm(&UitFormCtrl_Test_Form);
  };
  return(0);
}

main()
{
  WinInit();
  GetUitMenu(&UitMenu_Test_Menu,func);
  WinEnd();

  printf("\n\n field1[%s] select[%s] hidden[%s] field3[%.4lf]\n",
    tt.field_1,tt.field_2.str,tt.hidden,tt.field_3);

  printf(" field_4[%s] date[%s] time[%s]\n",
    tt.field_4,tt.date_str,tt.time_str);
}
