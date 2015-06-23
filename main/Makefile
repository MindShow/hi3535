include ../Makefile.param

TARGET = hi3535


CFLAGS := -Wall -g -O2 -fPIC

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
INCLUDE_PATH += -I../alarm

LIB_FILE := -lapp
LIB_FILE += -ldec
LIB_FILE += -lhisdk
LIB_FILE += -lhiapp
LIB_FILE += -lpthread 
LIB_FILE += -lpublic
LIB_FILE += -lshm
LIB_FILE += -lalarm

#海思的静态库
HI_MPI_LIBS := $(VDEC_LIBA)
HI_MPI_LIBS += $(AUDIO_LIBA) 
HI_MPI_LIBS += $(JPEGD_LIBA) 

#共享内存的静态库
HI_MPI_LIBS += $(SHM_LIBA) 
#-lrt -lstdc++的作用是让gcc也能编译c++的程序
all:
	$(CC) -lrt -lstdc++ $(CFLAGS) -o $(TARGET) main.c $(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) $(LIB_FILE) $(HI_MPI_LIBS) 
	$(CC) $(CFLAGS) -o myWatchDog myWatchDog.c ../app/watchdogLib.c	$(INC_FLAGS) $(INCLUDE_PATH) $(LIB_PATH) -lpthread
clean:
	rm  -rf  $(TARGET) *.o
