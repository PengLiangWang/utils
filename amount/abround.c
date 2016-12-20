#include "abround.h"
#include <stdio.h>
#include <string.h>

int doublecmp(double value1,double value2)
{
  double  d;

  d = value1-value2;

  if (d>0.001) return(1);
  if (d<-0.001) return(-1);
  return(0);
}

int DOUBLE_IS_NULL(double amt)
{
  static unsigned char  mm[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

  if (memcmp((char *)&amt,(char *)mm,8)==0)
     return(1);
  return(0);
}

static double Round_Tune_Value[] = {
        0.5,
        0.05,
        0.005,
        0.0005,
        0.00005,
        0.000005,
        0.0000005,
        0.00000005,
        0.000000005,
        0.0000000005,
        };
static double Round_Tune_Value01[] = {
        0.1,
        0.01,
        0.001,
        0.0001,
        0.00001,
        0.000001,
        0.0000001,
        0.00000001,
        0.000000001,
        0.0000000001,
        };
int AB_TURNCATE(double *amt,int turncate_wide)
{
  static char ta[1024];
  double atof();

  if(turncate_wide<0||turncate_wide>9) return(-1);
  sprintf(ta,"%.*lf",turncate_wide+1,*amt);
  ta[strlen(ta)-1] = '\0';
  *amt = atof(ta);

  return 0;
}
int AB_ROUND(double *amt,int round_wide)
{
  int     r;

  if(round_wide<0||round_wide>9) return(-1);

  r=doublecmp(*amt,0);

  if (r==0)
  {
    *amt = 0;
    return 0;
  }
  if (r>0)
    *amt += Round_Tune_Value[round_wide];
  else
    *amt -= Round_Tune_Value[round_wide];

  return AB_TURNCATE(amt,round_wide);
}

int AB_ROUND_01(double *amt,int round_wide)
{
  int     r;

  if(round_wide<0||round_wide>9) return(-1);

  r=doublecmp(*amt,0);

  if (r==0)
  {
    *amt=0;
    return(0);
  };

  if (r>0)
    *amt += Round_Tune_Value01[round_wide];
  else
    *amt -= Round_Tune_Value01[round_wide];

  return AB_TURNCATE(amt,round_wide);
}
