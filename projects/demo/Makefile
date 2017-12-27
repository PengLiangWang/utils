ACT_SRC_FILE=
C_FILE=
INCLUDE_FILE=
SQL_FILE=
SQL_SRC_FILE=
C_SRC_FILE=
LIB_FILE=
PUB_INCLUDE_SRC_FILE=
CLEAN_FILE_SRC=
USR_CLEAN_FILE=
BIN_FILE=tdes
EXEC_LOCAL=
ETC_FILE=
DOC_FILE=
SH_FILE=
include $(PREFIX)/make/makeobj.inc
C_FLAG=

LIBS= -L. -L$(LIBDIR) -lttsys -lcrypto $(DBLIB) $(SOCKETLIB)



tdes:destest.o
	$(CC) -g -o $@ destest.o $(LIBS)
   
tmsg:tmsg.o
	$(CC) -g -o $@ tmsg.o $(LIBS)

