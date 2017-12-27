##########  This File is Your Makefile Demo
APP_DEF_USER=DBUTIL
PUB_INCLUDE_SRC_FILE= MultiTrans.h OracleOci.h businessdb.h DBulog.h
C_SRC_FILE= MultiTrans.c businessdb.c dbutil_va.c ociutil.c \
	dbLogin.pc dbutil.cp dbutil.ec dbutil.info.h dbutil.ora.h dbutil.syb.h
PUB_INCLUDE_FILE=dbutil.h
C_FILE=dbutil.c
DOC_SRC_FILE=ChangeLog
ETC_SRC_FILE=

LIB_FILE=libdbutil.a
ORACLEUSERID=scott/tiger

C_FLAG=-DESQL_HIGH_VERSION

APP_DEF_USER=UTIL_DB

CLEAN_FILE_SRC=
USR_CLEAN_FILE=

include $(PREFIX)/make/makeobj.inc

###### The Following is The Objects That Your Need Compile

LIBOBJ = $(LIBOBJ_ORACLE)
LIBOBJ_INFOR = dbutil.o businessdb.o MultiTrans.o DBulog.o
LIBOBJ_ORACLE = $(LIBOBJ_INFOR) dbutil_va.o ociutil.o DBulog.o

###### The Following is The Normal Compile Rule
businessdb.o:businessdb.c businessdb.h
MultiTrans.o:MultiTrans.c MultiTrans.h

dbutil.h : dbutil.info.h dbutil.ora.h dbutil.syb.h
	@if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		cp dbutil.ora.h dbutil.h ;			\
	elif [ "A$(USESYBASE)" = "AYES" ] ; then		\
		cp dbutil.syb.h dbutil.h ;			\
	else							\
		echo "----USE INFORMIX----" ;			\
		cp dbutil.info.h dbutil.h ;			\
	fi

dbutil.o:dbutil.ec dbLogin.pc dbutil.cp dbutil.h OracleOci.h
	if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		$(DBLINK) $(PCCFLAGS) -I. iname=dbLogin.pc;	\
		$(CC) $(C_FLAGS) -c -I. -I$(INCLUDEDIR) -I$(USER_INCLUDE) $(APP_FLAG) dbLogin.c;\
		/bin/mv dbLogin.o dbutil.o;				\
		/bin/rm dbLogin.c ;					\
		/bin/rm dbLogin.lis ;				\
		#find . -empty -type f -name "tp*" | xargs /bin/rm;	\
	elif [ "A$(USESYBASE)" = "AYES" ] ; then		\
		cpre dbutil.cp ;				\
		$(CC) $(C_FLAGS) -c -I. -I$(INCLUDEDIR) -I$(SYBASEINCL) dbutil.c $(DBLIBS);\
	else							\
		echo "----USE INFORMIX----" ;			\
		$(DBLINK) -c -I. -I$(PREFIX)/include $(DBINCL) $(MISC_INCL) $(APP_FLAG) $(C_FLAG) dbutil.ec;\
		/bin/rm dbutil.c;					\
	fi


libdbutil.a: $(LIBOBJ)
	@if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		ar rv libdbutil.a $(LIBOBJ_ORACLE) ;		\
	else	ar rv libdbutil.a $(LIBOBJ_INFOR) ;		\
	fi

ociutil.o : ociutil.c
	@if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		$(CC) $(C_FLAGS) -c -I. -I$(INCLUDEDIR) -I$(USER_INCLUDE) $(APP_FLAG) ociutil.c;\
	else	cat /dev/null > ociutil.o ;	fi

dbutil_va.o : dbutil_va.c
	@if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		$(CC) $(C_FLAGS) -c -I. -I$(INCLUDEDIR) -I$(USER_INCLUDE) $(APP_FLAG) dbutil_va.c;\
	else	cat /dev/null > dbutil_va.o ;	fi

libdbutil.so:$(LIBOBJ)
	@if [ "A$(USEORACLE)" = "AYES" ] ; then			\
		$(DBLINK) $(C_FLAGS) -G -o $@ $(LIBOBJ_ORACLE) $(DBSHARELIB);\
	else							\
		$(DBLINK) $(C_FLAGS) -G -o $@ $(LIBOBJ_INFOR) $(DBSHARELIB) ;\
	fi
