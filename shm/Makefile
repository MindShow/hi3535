include ../Common.param

LIB_NAME := libshm

#找出当前目录下所有的.cpp文件
CPPSOURCE :=$(shell find . -type f -name "*.cpp")

#生成.cpp文件对应的.o文件
CPPOBJS :=$(subst .cpp,.o,$(CPPSOURCE))

# 编译生成动态库
all: $(LIB_NAME).so 
$(LIB_NAME).so:$(CPPOBJS) $(COBJS) 
	$(CPP) -shared -o $(LIB_NAME).so $(CPPOBJS) $(COBJS) 
	@cp -rf $(LIB_NAME).so ../lib/
	@cp -rf $(LIB_NAME).so /mnt/nfs/hi3535/lib/

$(CPPOBJS): %.o : %.cpp 
	$(CPP) $(CFLAGS) -c $< -o $@ 

$(COBJS): %.o : %.c 
	$(CC) $(CFLAGS) -c $< -o $@ 
#清除
clean:
	rm *.o *.d  $(LIB_NAME).so -f
