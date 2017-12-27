PUB_INCLUDE_SRC_FILE= ttsys.h ttdef.h memory.h exml.h dso.h strings.h xlate.h \
                      sockets.h config.h msgqueue.h shmem.h mutex.h \
                      file.h list.h hlist.h datetime.h \
                      shmempool.h hashex.h triex.h elog.h \
                      datapool.h bitmap.h dlist.h sheet.h abround.h \
                      DesUtil.h  MD5Util.h
INCLUDE_SRC_FILE=
C_SRC_FILE=
PC_SRC_FILE=
DOC_SRC_FILE= 
SQL_SRC_FILE=
SH_FILE=
OBJ_FILE= 
LIB_FILE=libttsys.a 
SH_FILE= 
BIN_FILE=
EXEC_LOCAL= 
OTHER_SRC_FILE= 
C_FLAG=-g
APP_DEF_USER=ttsys
include $(PREFIX)/make/makeobj.inc


LIBOBJ= memory.o snprintf.o exml.o dso.o strings.o xlate.o \
        sockets.o config.o msgqueue.o shmem.o mutex.o file.o \
        list.o hlist.o datetime.o bitmap.o\
        shmempool.o hashex.o triex.o elog.o \
        datapool.o ttsys.o dlist.o sheet.o abround.o\
        DesUtil.o MD5Util.o

LIBS= -L. -L$(LIBDIR) -lttsys -lxml2 -ldl $(SOCKETLIB)


libttsys.a:$(LIBOBJ)
	ar rv $@ $?

elogsvr:elogd.o
	$(CC) -g -o $@ elogd.o $(LIBS)
