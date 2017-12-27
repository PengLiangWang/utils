/************************************************************
 * structs.h defined for win genenate manager             *
 ************************************************************
 * date 1989.8.22    Version 1.                             *
 ************************************************************/
#ifndef _WIN_H_
#define _WIN_H_
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************/
/* define for wgtdata */
#define FIELD_OVER  0
#define ALLACCEPT 1
#define FLDACCEPT 2
#define MOVEUP    3
#define MOVEDOWN  4
#define DISCARD   5
#define HELPNEED  6
#define MOVELEFT  7
#define MOVERIGHT 8
/******************************************/
#define AL_TYPE   0
#define AN_TYPE   1
#define NU_TYPE   2
#define BY_TYPE   3
#define TT_TYPE   4
#define SC_TYPE   5
#define UC_TYPE   6
#define SI_TYPE   7
#define UI_TYPE   8
#define SF_TYPE   9
#define UF_TYPE   10
#define SD_TYPE   11
#define UD_TYPE   12
#define DA_TYPE   13
#define TM_TYPE   14
#define SE_TYPE   15
#define C_SE_TYPE 16
/* ������ɫ */
#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED   4
#define MAGENTA   5
#define YELLOW    6
#define WHITE   7
/* �������� */
#define GRAPHIC   0x10
#define HIGHLIGHT 0x8
#define BLINK   0x4
#define INVERSE   0x2
#define UNDERLINE 0x1
#define NORMAL    0x20
/******************** end *****************/

#define ASCIIC    0x0
#define CWFB    0x1
#define CWSB    0x2
#define CTRLA   0x3

#define MAXL    100
#define MAXC    256
#define MAXROW    25
#define MAXCOL    80
#define STRTYPE   1
#define NUMTYPE   0

#define WIN   struct win_lst
#define DOT   struct dot_lst
#define CH    struct ch_lst
#define ATTR    struct attr_st 
/*********************** define for v_attr of win_lst ******/
#define AUTONEXTLINE  0x1 /* �Զ��س����� */
#define WINCHG    0x2 /* �����и��� */
#define VCURON    0x4 /* �����ʾ */
#define MUSTFRESH   0x8 /* ���ĸĶ� */
#define INSERT    0x10  /* ���ĸĶ���ʽ */
#define HIDDEN    0x20  /* ��������   */
#define SCROLL    0x40  /* ������� */
#define OVERLAY   0x80  /* ���� */
#define CURSORMOVE    0x100 /* ����ƶ�λ */
#define NEWFACE   0x200   /*  wnoutrefresh done */
#define DOOUTPUT  0x400   /* wcls done   */
/*********************** end for v_attr of win_lst ******/
#define NOTCHG    -1

struct dot_lst {
  short x,y;
};

struct attr_st {
  unsigned short f:3;     /* ǰ�� */
  unsigned short b:3;     /* ���� */
  unsigned short cm:2;    /* ���� 0:ASCII 1:CWFB 2:CWSB:3:CTL */
  unsigned short m:8;     /* ���� */
};

struct ch_lst {
  ATTR    attr;
  unsigned char ach;
};

struct win_lst {
  unsigned short  v_attr;   /* �������� */
  unsigned short  f_cls_s:1;
  unsigned short  f_cls_l:1;
  unsigned short  scrollflag:1;
  unsigned short  markflag:1;
  unsigned short  marktype:1;
#define C_MARKTYPE      0
#define A_MARKTYPE      1
  unsigned short  subwin:1;
  unsigned short  nocover:1;
  unsigned short  wfreshflag:1;
  unsigned short  docls:1;
  unsigned short  docll:1;
  unsigned short  doscrl:1;
  CH    mch;    /* mark CH */
  short   mbx;
  short   mby;
  short   mex;
  short   mey;
  DOT   uldot;    /* ��������Ļ�ϵ�λ�� */
  DOT   cur;    /* ���ڻ������Ĺ�� */
  DOT   ocur;   /* ���ڻ�������ԭ��� */
  DOT   vdot;   /* �����ڴ��ڻ�������λ�� */
  ATTR    curattr;  /* ���ڵ�ǰ��ʾ���� */
  CH    frich;    /* �ݴ溺�ֵ�һ�ֽ� */
  short     vrow;   /* ���ڵ���ʾ���� */
  short     vcol;   /* ���ڵ���ʾ���� */
  short     row;    /* ���ڵĴ��ڻ��������� */
  short     col;    /* ���ڵĴ��ڻ��������� */
  short   *friy;    /* �޸�ϵ��ͷ */
  short   *lasy;    /* �޸�ϵ��β */
  CH    **_y;   /* ���ڻ����� */
  WIN   *nwin;
};
#ifndef INIT
extern CH **video1,**video2;
extern WIN *firstw,*stdw,*curw;
#endif
extern unsigned char CC;
extern short VC,LI,CO;
extern char PC;
extern short _TTYTYPE;    /* 1--����̨,0--RS232 */
extern short ospeed;
WIN *crwin();
#define wmustfreshon(v)   ((v)->v_attr |= MUSTFRESH)
#define wmustfreshoff(v)  ((v)->v_attr &= ~MUSTFRESH)
#define wcuronon(v)     ((v)->v_attr |= VCURON)
#define wcuronoff(v)    ((v)->v_attr &= ~VCURON)
#define wautonlon(v)    ((v)->v_attr |= AUTONEXTLINE)
#define wautonloff(v)   ((v)->v_attr &= ~AUTONEXTLINE)
#define winserton(v)    ((v)->v_attr |= INSERT)
#define winsertoff(v)   ((v)->v_attr &= ~INSERT)
#define whiddenon(v)    ((v)->v_attr |= HIDDEN)
#define whiddenoff(v)   ((v)->v_attr &= ~HIDDEN)
#define wscrollon(v)    ((v)->v_attr |= SCROLL)
#define wscrolloff(v)   ((v)->v_attr &= ~SCROLL)
#define woverlayon(v)   ((v)->v_attr |= OVERLAY)
#define woverlayoff(v)    ((v)->v_attr &= ~OVERLAY)

#define wnocover(w)     (w->nocover = 1)
#define wrow(w)       (w->row)
#define wcol(w)       (w->col)
#define wvrow(w)      (w->vrow)
#define wvcol(w)      (w->vcol)

#ifdef __cplusplus
}
#endif

#endif /* _WIN_H_ */
