APP_DEF_USER = IBcom
PUB_INCLUDE_SRC_FILE=IB.h IBerr.h RCI.h IBSERVICE.h
C_SRC_FILE=
ETC_FILE=IBserver
ETC_SRC_FILE=IBdaemon
LIB_FILE=libIBcom.a
C_FLAG=

include $(PRELOAD)/make/makeobj.inc

LINKLIB=  -L. -L$(PREFIX)/lib -lIBcom -lIBsec -lIBnote -lIButil -lcrypto $(SOCKETLIB)

LIBOBJ=IBcom.o RCI.o RCIUTIL.o semphore.o IBSERVICE.o

libIBcom.a:$(PARALLELMAKE) $(LIBOBJ)
	ar rv $@ $(LIBOBJ)

IBserver:IBserver.o libIBcom.a
	$(CC) -g -o $@ IBserver.o $(LINKLIB)
