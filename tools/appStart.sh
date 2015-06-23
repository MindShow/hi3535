#/bin/sh
# 守护进程

while :
do
hi3535_pid=`ps | grep -v grep | grep "hi3535" | awk '{print $1}'`
if [ -z $hi3535_pid ] ; then
	echo   "there   isn't   hi3535   process!"
	killall -9 hi3535 NvrClient
	cd /mnt/nfs/hi3535
    ./hi3535&
    sleep 2
    cd /mnt/nfs/qt
    ./NvrClient -qws &
fi

sleep 2

gui_pid=`ps | grep -v grep | grep "NvrClient" | awk '{print $1}'`
if [ -z $gui_pid ] ; then
	echo   "there   isn't   gui   process!"
    cd /mnt/nfs/qt/
    ./NvrClient -qws &
fi 
done;


