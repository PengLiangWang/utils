#    根据代码管理需要各个工作目录中的Makefile应将目录中的源文件，生成文件
#分类详细登记。分类登记必须正确，所有为登记在源文件标题下的文件可能被删除!!
#    提交的公共INCLUDE文件、SQL文件、ACT文件、HTML文件、CGI文件、执行文件，
#文件名应包括模块信息，以防止与其它模块中文件重名。
#　　此样本文件存放在make/make.obj

C_SRC_FILE=IB_RCI_TEST_SERVER.c IB_RCI_TEST_CLIENT.c IB_SERVICE_SERVER.c
DOC_SRC_FILE=
OTHER_SRC_FILE =otp_data.def
ETC_SRC_FILE=
INCLUDE_FILE= 
C_FILE=
ETC_FILE=
C_FLAG=-D_IB_DEBUG_
BIN_FILE=NET_ECHO_RCI NET_ECHO_RCI_SERVER NET_ECHO_SBUF NET_ECHO_SBUF_SERVER
include $(PRELOAD)/make/makeobj.inc #　公共规则

LIBS= -L. -L$(PREFIX)/lib -lIBcom -lIBnote -lIBsec -lIButil -lcrypto $(SOCKETLIB)


opt_data.o     :     opt_data.c opt_data.h
opt_data.c     :     opt_data.def
	comAction IB $<


NET_ECHO_RCI:IB_RCI_TEST_CALL.o opt_data.o
	$(CC) -g -o $@ IB_RCI_TEST_CALL.o opt_data.o $(LIBS)


NET_ECHO_RCI_SERVER:IB_RCI_TEST_SERVER.o opt_data.o
	$(CC) -g -o $@ IB_RCI_TEST_SERVER.o opt_data.o $(LIBS)

NET_ECHO_SBUF_SERVER:IB_SERVICE_SERVER.o
	$(CC) -g -o $@ IB_SERVICE_SERVER.o $(LIBS)

NET_ECHO_SBUF:IB_SERVICE_CLIENT.o
	$(CC) -g -o $@ IB_SERVICE_CLIENT.o $(LIBS)
