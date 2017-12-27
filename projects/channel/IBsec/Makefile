APP_DEF_USER=IBsec
PUB_INCLUDE_SRC_FILE=IBsec.h
C_SRC_FILE=
LIB_FILE=libIBsec.a
ETC_FILE=IBsec IBsm
ETC_SRC_FILE=IBSEC_CTRL
C_FLAG=-D_IBSM_DEBUG_

include $(PRELOAD)/make/makeobj.inc

LINKLIB=  -L. -L$(PREFIX)/lib -lIBsec -lIButil -lcrypto

LIBOBJ=IBsecQueue.o IBsec.o IBsecBox.o 

libIBsec.a:$(PARALLELMAKE) $(LIBOBJ)
	ar rv $@ $(LIBOBJ)

IBSECOBJ=IBsecKeyData.o IBsecBox.o IBsecUtil.o IBsecServer.o

IBsec:$(IBSECOBJ)
	$(CC) -o $@ $(IBSECOBJ) $(LINKLIB) $(WINLIB)

IBsm:IBsm.o
	$(CC) -o $@ IBsm.o $(LINKLIB) $(WINLIB)
