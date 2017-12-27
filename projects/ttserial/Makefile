ACT_SRC_FILE=
C_FILE=
INCLUDE_FILE=
SQL_FILE=
SQL_SRC_FILE=ttserial.sql
C_SRC_FILE=
LIB_FILE=libtserial.a
PUB_INCLUDE_SRC_FILE=ttserial.h
CLEAN_FILE_SRC=
USR_CLEAN_FILE=
BIN_FILE=ttseriald ttserial
EXEC_LOCAL=
ETC_FILE=
DOC_FILE=
SH_FILE=
include $(PREFIX)/make/makeobj.inc
C_FLAG=

LIBS= -L. -L$(LIBDIR) -lttsys -ldbutil $(DBLIB) $(SOCKETLIB)
LIB_OBJ=ttserial.o

$(LIB_FILE): $(PARALLELMAKE) $(LIB_OBJ)
	ar rv $@ $(LIB_OBJ)

ttserial:ttserial.o ttserialctrl.o ttdbserial.o
	$(CC) -g -o $@ ttserial.o ttserialctrl.o ttdbserial.o $(LIBS)

ttseriald:ttserialserver.o ttdbserial.o
	$(CC) -g -o $@ ttserialserver.o ttdbserial.o $(LIBS)
