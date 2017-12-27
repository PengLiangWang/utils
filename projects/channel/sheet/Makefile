APP_DEF_USER=SBUF
PUB_INCLUDE_SRC_FILE=ServiceBuf.h
C_SRC_FILE=
LIB_FILE=libsbuf.a
ETC_FILE=
ETC_SRC_FILE=
C_FLAG=-g

include $(PRELOAD)/make/makeobj.inc

LINKLIB=  -L. -L$(PREFIX)/lib -lsbuf -lIBsec -lIButil -lcrypto

LIBOBJ=ServiceBuf.o

libsbuf.a:$(PARALLELMAKE) $(LIBOBJ)
	ar rv $@ $(LIBOBJ)
