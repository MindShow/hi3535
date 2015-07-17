# 编译生成动态库
all: $(LIB_NAME).so 
# -shared :编译动态库 
#生成动态库
$(LIB_NAME).so:$(COBJS) $(HI_LIB)/*.a 
	$(CC) -shared -o $(LIB_NAME).so $(COBJS) $(HI_LIB)/*.a 
	@mv $(LIB_NAME).so ../lib/
	@cp -rf ../lib/*.so /mnt/nfs/hi3535/lib/
	@rm $(COBJS)

#清除
clean:
	rm *.o *.d  $(LIB_NAME).so -f
