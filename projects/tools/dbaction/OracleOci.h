#ifndef _ORACLE_OCI_DATA_INCLUDE_H_
#define _ORACLE_OCI_DATA_INCLUDE_H_

#define DT_ASC 1						     /* ASCII */
#define DT_NUM 2					    /* ORACLE NUMERIC */
#define DT_ITG 3						   /* INTEGER */
#define DT_REA 4						      /* REAL */
#define DT_STR 9						    /* STRING */
#define DT_RAW 6				       /* BINARY DATA STRINGS */
#define DT_LNG 8						      /* Long */

#define	DT_DAT	128					/* DATE */
#define	DT_YMD	129					/* YYYY-MM-DD */
#define	DT_YM	130					/* YYYY-MM    */

#define SQLNOTFOUND	4

/*
** DEFINE THE c VERSION OF THE CURSOR (FOR 32 BIT MACHINES)
*/
struct csrdef
{
   short	  csrrc;				  /* return code */
   short	  csrft;				/* function type */
   unsigned long  csrrpc;			 /* rows processed count */
   short	  csrpeo;			   /* parse error offset */
   unsigned char  csrfc;				/* function code */
   unsigned char  csrfil;				      /* filler  */
   unsigned short csrarc;			    /* reserved, private */
   unsigned char  csrwrn;				/* warning flags */
   unsigned char  csrflg;				  /* error flags */
   /*		     *** Operating system dependent *** 		 */
   unsigned int   csrcn;				/* cursor number */
   struct {					      /* rowid structure */
     struct {
	unsigned long	tidtrba;	   /* rba of first blockof table */
	unsigned short	tidpid; 		/* partition id of table */
	unsigned char	tidtbl; 		    /* table id of table */
	}		ridtid;
     unsigned long   ridbrba;			     /* rba of datablock */
     unsigned short  ridsqn;	      /* sequence number of row in block */
     } csrrid;
   unsigned int   csrose;		      /* os dependent error code */
   unsigned char  csrchk;				   /* check byte */
   unsigned char  crsfill[30];		       /* private, reserved fill */
};

typedef struct csrdef DBcurs;

typedef struct {
		char	host[256];
		DBcurs	lda;
	} DataBaseServer;

#define OFC_CTB 1					      /* CREATE TABLE */
#define OFC_CLN 2					       /* CREATE LINK */
#define OFC_INS 3						    /* INSERT */
#define OFC_SEL 4						    /* SELECT */
#define OFC_UPD 5						    /* UPDATE */
#define OFC_DLN 6						 /* DROP LINK */
#define OFC_DVW 7						 /* DROP VIEW */
#define OFC_DTB 8						/* DROP TABLE */
#define OFC_DEL 9						    /* DELETE */
#define OFC_DEV 10					       /* DEFINE VIEW */
#define OFC_EXP 11						    /* EXPAND */
 
#define OLOGIN	    -1
#define OLOGOF	    -2
#define OOPEN	    14
#define OCLOSE	    16
#define OSQL	    2
#define ODFINN	    8
#define OBIND	    6
#define ODSRBN	    10
#define OEXEC	    4
#define OFETCH	    12
#define OBINDN	    50
#define OSQL3	    26
 
#endif
