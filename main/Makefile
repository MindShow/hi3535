include ../Makefile.param

TARGET = hi3535


CFLAGS := -Wall -g -O2 -fPIC -lrt -lstdc++

#CFLAGS += $(INC_FLAGS)

LIB_PATH = -L../lib

INCLUDE_PATH := -I$(HI_INC)
INCLUDE_PATH += -I$(HI_EXTDRV)/tlv320aic31
INCLUDE_PATH += -I../app
INCLUDE_PATH += -I../dec
INCLUDE_PATH += -I../HiSDK/common
INCLUDE_PATH += -I../HiApp
INCLUDE_PATH += -I../include
INCLUDE_PATH += -I../public
INCLUDE_PATH += -I../shm
INCLUDE_PATH += -I../database

LIB_FILE := -lapp
LIB_FILE += -ldec
LIB_FILE += -lhisdk
LIB_FILE += -lhiapp
LIB_FILE += -lpthread 
LIB_FILE += -lpublic
LIB_FILE += -lshm
LIB_FILE += -ldatabase

#海思的静态库
HI_MPI_LIBS := $(VDEC_LIBA)
HI_MPI_LIBS += $(AUDIO_LIBA) 
HI_MPI_LIBS += $(JPEGD_LIBA) 

#OpenSourceLib,开源代码的库文件

#共享内存的静态库
OPEN_SOURCE_LIBS := $(SHM_LIBA) 
#开源代码的共享库
OPEN_SOURCE_LIBS += $(SQLITE3_LIBA) 
#-lrt -lstdc++的作用是让gcc也能编译c++的程序
all:
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) $(LIB_FILE) $(HI_MPI_LIBS) $(OPEN_SOURCE_LIBS) 
	$(CC) $(CFLAGS) -o Wdt_Debug Wdt_Debug.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) -lpthread
clean:
	rm  -rf  $(TARGET) *.o
