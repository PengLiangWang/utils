C_SRC_FILE=
C_FILE=DB_HsmKeyBox.c
PUB_INCLUDE_FILE=
INCLUDE_FILE=
EC_FILE=
SQL_FILE=DB_HsmKeyBox.sql
ACT_SRC_FILE=
LIB_FILE=libhsmd.a libHsmAPI2.a
PUB_INCLUDE_SRC_FILE=HsmAPI2.h
CLEAN_FILE_SRC=
USR_CLEAN_FILE=
BIN_FILE=HsmServer HsmCtrl2 hsm56test
MOD_FILE=HsmCommand1.so
EXEC_LOCAL=
ETC_FILE=
DOC_FILE=
SH_FILE=
C_FLAG=-rdynamic

include $(PREFIX)/make/makeobj.inc


LIBS= -L. -L$(LIBDIR) -lhsmd -lttsys -lxml2 -lcrypto -ldl $(SOCKETLIB)
LIBS1=-L. -L$(LIBDIR) -lHsmAPI2

DB_HsmKeyBox.h   : DB_HsmKeyBox.act
DB_HsmKeyBox.sql : DB_HsmKeyBox.act
DB_HsmKeyBox.c   : DB_HsmKeyBox.act
DB_HsmKeyBox.o   : DB_HsmKeyBox.c

HSMSERVEROBJ=HsmWorker.o HsmServer.o

HsmServer:$(HSMSERVEROBJ)
	$(CC) $(C_FLAG) -g -o HsmServer $(HSMSERVEROBJ) $(LIBS)

LIBOBJ=DB_HsmKeyBox.o HsmCommand.o HsmConfig.o HsmRes.o HsmKeyBox.o HsmMethod.o

libhsmd.a:$(LIBOBJ)
	ar rv $@ $?

LIBSOOBJ=HsmFunction.o
HsmCommand1.so:$(LIBSOOBJ)
	$(CC) $(C_FLAG) -shared -o $@ $(LIBSOOBJ) $(LIBS)

LIBOBJ2=HsmAPI2.o HsmAPI56.o
libHsmAPI2.a:$(LIBOBJ2)
	ar rv $@ $?

HsmCtrl2:HsmCtrl2.o
	$(CC) $(C_FLAG) -g -o HsmCtrl2 HsmCtrl2.o $(LIBS1) $(LIBS)

RsaDe:RsaDecrypt.o
	$(CC) $(C_FLAG) -g -o RsaDe RsaDecrypt.o $(LIBS1) $(LIBS)

RsaEn:RsaEncrypt.o
	$(CC) $(C_FLAG) -g -o RsaEn RsaEncrypt.o $(LIBS1) $(LIBS)

hsm56test:Hsm56Test.o
	$(CC) $(C_FLAG) -g -o hsm56test Hsm56Test.o $(LIBS1) $(LIBS)
