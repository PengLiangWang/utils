proj_home=$(shell cd ../../..;pwd)
CC=gcc -g -Wall

TARGET_LIB=librdsms.a librdsms.so
TEST=test

INC_PATH=$(proj_home)/_debug/_inc
LIB_PATH=$(proj_home)/_debug/_lib

ARCH=$(shell uname -m)
ifeq (x86_64, ${ARCH})
   CFLAG=-m64 -fPIC
else
   CFLAG=-m32
endif

O_FILE=rdsapi.o

H_FILE=$(wildcard *.h)

all:${TARGET_LIB} ${TEST}

%.o:%.c
	$(CC) $(CFLAG) -c $< -I${INC_PATH}

librdsms.so:$(O_FILE)
	$(CC) $(CFLAG) -shared -rdynamic -o $@ $(O_FILE) -L. -lhiredis -L${LIB_PATH} -lexutil

librdsms.a:$(O_FILE)
	ar rv $@ $(O_FILE)

test:test.o
	$(CC) $(CFLAG) -o $@ test.o -L. -lrdsms -L${LIB_PATH} -lexutil

install:
	cp librdsms.a libhiredis.a ${LIB_PATH}
	cp $(H_FILE) ${INC_PATH}


clean:
	rm -f ${O_FILE} test.o
	rm -f ${TARGET_LIB} $(TEST)
