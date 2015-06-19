# 包含(include)当前目录下的makefile
include Makefile.param 
# 子目录
SUBDIRS += app
SUBDIRS += dec
SUBDIRS += HiSDK/common
SUBDIRS += HiApp
SUBDIRS += public 
SUBDIRS += shm
SUBDIRS += alarm
#-C:切换编译的目录 
#但下面的语句，首先会执行$(SUBDIRS):，即切换到子目录(SUBDIRS)appLib中，执行里面的Makefile,然后再切换返回
#当前目录，再切换到demo目录中执行其中的makefile。
all: $(SUBDIRS)
	@make -C main 
	@cp main/hi3535 /mnt/nfs/hi3535
# > /dev/NULL:表示不输出编译的打印过程(未验证)
$(SUBDIRS):  
	@make -C $@ > /dev/NULL
#foreach:这里的含义是遍历$(SUBDIRS)中的目录，并执行make clean 操作。
clean:
	@$(foreach SUBDIR, $(SUBDIRS), make clean -C $(SUBDIR) clean > /dev/NULL ;)
	@make -C main clean > /dev/NULL
.PHONY: clean $(SUBDIRS)
