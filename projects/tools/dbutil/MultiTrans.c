#include <stdio.h>
#include "MultiTrans.h"
static int MultiCount = 0;
static int CurTransCount;

void MultiTransCount(int *count)
{
  *count = CurTransCount;
}

int MultiTransSetUp(int count)
{
  MultiCount = count;
  CurTransCount = 0;
}
int MultiTransBeginWork()
{
  int r;
  if(CurTransCount==0)
  {
    r = dbBeginWork();
  } else r = 0;
  CurTransCount++;
  return r;
}
int MultiTransCommit(int enforce_flag)
{
  int r;
  if((CurTransCount>=MultiCount)||(CurTransCount>0&&enforce_flag))
  {
    r = dbCommit();
    CurTransCount = 0;
  }
  else r = 0;
  return 0;
}

int MultiTransRollback()
{
  int r;
  r = 0;
  if(CurTransCount!=0)
  {
    r = dbRollback();
  }
  CurTransCount = 0;
  return r;
}
