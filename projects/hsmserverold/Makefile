C_SRC_FILE=
C_FILE=DB_HsmKeyBox.c
PUB_INCLUDE_FILE=
INCLUDE_FILE=
EC_FILE=
SQL_FILE=DB_HsmKeyBox.sql
ACT_SRC_FILE=
LIB_FILE=libhsmd.a libhsml.a
PUB_INCLUDE_SRC_FILE=HsmAPI.h
CLEAN_FILE_SRC=
USR_CLEAN_FILE=
BIN_FILE=HsmServer HsmCtrl RsaEn
MOD_FILE=HsmCommand1.so
EXEC_LOCAL=
ETC_FILE=
DOC_FILE=
SH_FILE=
C_FLAG= -rdynamic

include $(PREFIX)/make/makeobj.inc


LIBS= -L. -L$(LIBDIR) -lhsmd -lttsys -lxml2 -ldbutil -lcrypto $(DBLIB) -ldl $(SOCKETLIB)
LIBS1= -L. -L$(LIBDIR) -lhsml

DB_HsmKeyBox.h   : DB_HsmKeyBox.act
DB_HsmKeyBox.sql : DB_HsmKeyBox.act
DB_HsmKeyBox.c   : DB_HsmKeyBox.act
DB_HsmKeyBox.o   : DB_HsmKeyBox.c

HSMSERVEROBJ=HsmWorker.o HsmServer.o

HsmServer:$(HSMSERVEROBJ)
	$(CC) $(C_FLAG) -g -o HsmServer $(HSMSERVEROBJ) -Wl,--hash-style=sysv $(LIBS)

LIBOBJ=DB_HsmKeyBox.o HsmCommand.o HsmConfig.o HsmRes.o HsmKeyBox.o HsmMethod.o

libhsmd.a:$(LIBOBJ)
	ar rv $@ $?

LIBSOOBJ=HsmFunction.o
HsmCommand1.so:$(LIBSOOBJ)
	$(CC) $(C_FLAG) -shared -o $@ $(LIBSOOBJ) $(LIBS)

LIBOBJ2=HsmAPI.o
libhsml.a:$(LIBOBJ2)
	ar rv $@ $?

HsmCtrl:HsmCtrl.o
	$(CC) $(C_FLAG) -g -o HsmCtrl HsmCtrl.o  -Wl,--hash-style=sysv $(LIBS1) $(LIBS)

RsaEn:RsaEncrypt.o
	$(CC) $(C_FLAG) -g -o RsaEn RsaEncrypt.o $(LIBS1) $(LIBS)

