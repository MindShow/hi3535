include ../../Makefile.param
LIB_NAME    := libhisdk

INC_FLAGS := -I$(HI_INC)
INC_FLAGS += -I$(HI_EXTDRV)/tlv320aic31

CFLAGS := -Wall -g -O2 -fPIC 
CFLAGS += $(INC_FLAGS) 
CFLAGS += -I../HiSDK/common

#找出当前目录下所有的.c文件
CSOURCE:=$(shell find . -type f -name "*.c")
#生成.c文件对应的.o文件
OBJECTS:=$(subst .c,.o,$(CSOURCE))
# 编译生成动态库
all: $(LIB_NAME).so 
# -shared :编译动态库 
#生成动态库
$(LIB_NAME).so:$(OBJECTS) $(HI_LIB)/*.a 
	$(CC) -shared -o $(LIB_NAME).so $(OBJECTS) $(HI_LIB)/*.a 
	@cp -rf $(LIB_NAME).so ../../lib/
	@cp -rf $(LIB_NAME).so /mnt/nfs/hi3535/lib/

#清除
clean:
	rm *.o *.d  $(LIB_NAME).so -f

