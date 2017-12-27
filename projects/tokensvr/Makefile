ACT_SRC_FILE=
C_FILE=
INCLUDE_FILE=
SQL_FILE=
SQL_SRC_FILE=
C_SRC_FILE=
LIB_FILE=libtoken.a
PUB_INCLUDE_SRC_FILE=etoken.h
CLEAN_FILE_SRC=
USR_CLEAN_FILE=
BIN_FILE=tokensvr etoken
EXEC_LOCAL=
ETC_FILE=
DOC_FILE=
SH_FILE=
include $(PREFIX)/make/makeobj.inc
C_FLAG=

LIBS= -L. -L$(LIBDIR) -lttsys $(SOCKETLIB)
LIBS1= -L. -L$(LIBDIR) -ltoken
LIB_OBJ=tokenpub.o etoken.o

$(LIB_FILE): $(PARALLELMAKE) $(LIB_OBJ)
	ar rv $@ $(LIB_OBJ)

tokensvr:tokenpub.o tokenserver.o
	$(CC) -g -o $@ tokenpub.o tokenserver.o $(LIBS)

etoken:tokenctrl.o
	$(CC) -g -o $@ tokenctrl.o $(LIBS1) $(LIBS)
