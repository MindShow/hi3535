#!/bin/sh
#说明（by he）：firmware_prefix的含义是/tmp/update/ 那firmware_postfix呢？
echo 0 > /tmp/update.txt

sleep 1
echo "UUUUUUUUUUUUUUUUUUUUUU update system UUUUUUUUUUUUUUUUUUUUUU"

#MOUNT_PATH=$1
UPDATA_PATH=$1

#DBG_LOG_DEVICE=/dev/console
DBG_LOG_DEVICE=/dev/null

REBOOT=false
SKIPFWCHECK=true
#DEVICE_ALIAS=`cat /proc/gxvboard/dev_info/dev_id 2> ${DBG_LOG_DEVICE}|tr -d '[A-Z]' '[a-z]' |tr -d '-'`
DEVICE_ALIAS=HI3535
FW_DOWNLOAD_PIPE=/tmp/fw_down
PROG_STOPPED=false
SETZERO=0
UPDATE_LOCAL_VERSION=false
LOCAL_VERSION_FILE="/data/version"

CORE_IMAGE="CORE"
BASE_IMAGE="BASE"
APP1_IMAGE="APP1"
WWW_IMAGE="WWW"
LOGO_IMAGE="LOGO"
UPGRADE_BUFFER="Buffer"


CORE_MASK_BIT=1
BASE_MASK_BIT=2
APP1_MASK_BIT=4
WWW_MASK_BIT=8
LOGO_MASK_BIT=16


stop_prog() {
    if [ "$PROG_STOPPED" = "false" ]; then

        PROG_STOPPED=true
	echo -n stop_prog
	echo "provision killapp ..........."
	#/usr/local/sbin/killapp
    fi
}

check_ongoing_call_status () {
    TIME_OUT=1  #45 minutes
    local i=1

    while [ "$i" -ge 1 -a "$i" -le $TIME_OUT ] # -ge:大于等于；-a: 制定变量;-le:小于等于
    do
        i=$(($i+1))
        save_msg "Waiting ongoing calls to finish..."
        sleep 5 2> ${DBG_LOG_DEVICE} 
    done

    return 0
}

syslog() {
        echo $* > ${DBG_LOG_DEVICE}
}

save_msg() {
    syslog $*
}


cd ${UPDATA_PATH}

#check version
full_image_filename="${DEVICE_ALIAS}version"
version_remote_file="${UPDATA_PATH}/version"

. $version_remote_file # 打开/tmp/update/version文件

COREMD5_S=`echo $COREMD5|cut -c1-32` #使用cut命令截取COREMD5中第1到第32个字节的数据,赋予COREMD5_S,但COREMD5 从哪里获取的？ 由上面的语句“. $version_remote_file”获取！,也就是version
BASEMD5_S=`echo $BASEMD5|cut -c1-32`
APP1MD5_S=`echo $APP1MD5|cut -c1-32`
WWWMD5_S=`echo $WWWMD5|cut -c1-32`
LOGOMD5_S=`echo $LOGOMD5|cut -c1-32`

echo "COREMD5_S = $COREMD5_S"
echo "BASEMD5_S = $BASEMD5_S"
echo "APP1MD5_S = $APP1MD5_S"
echo "WWWMD5_S = $WWWMD5_S"
echo "LOGOMD5_S = $LOGOMD5_S"
#echo $APP2MD5_S


#cp  /usr/sbin/flash_eraseall  ${UPDATA_PATH}/  -f
#cp  /usr/sbin/flashcp  ${UPDATA_PATH}/   -f
#cp  /usr/sbin/flash_eraseall  /tmp/  -f
#cp  /usr/sbin/flashcp  /tmp/   -f

if [ -f $LOCAL_VERSION_FILE ] # -f: 判断是否是文件,/data/version
then	
##	versioncheck $version_remote_file 2> ${DBG_LOG_DEVICE} > ${DBG_LOG_DEVICE}
##	retVersion=$?

	. $LOCAL_VERSION_FILE
	COREMD5_D=`echo $COREMD5|cut -c1-32`
	BASEMD5_D=`echo $BASEMD5|cut -c1-32`
	APP1MD5_D=`echo $APP1MD5|cut -c1-32`
	WWWMD5_D=`echo $WWWMD5|cut -c1-32`
	LOGOMD5_D=`echo $LOGOMD5|cut -c1-32`

	retVersion=0

	if [ $COREMD5_S != $COREMD5_D ];then
		retVersion=$(($retVersion+1))
	fi

	if [ $BASEMD5_S != $BASEMD5_D ];then
		retVersion=$(($retVersion+2))
	fi

	if [ $APP1MD5_S != $APP1MD5_D ];then
		retVersion=$(($retVersion+4))
	fi

	if [ $WWWMD5_S != $WWWMD5_D ];then
		retVersion=$(($retVersion+8))
	fi

	if [ $LOGOMD5_S != $LOGOMD5_D ];then
		retVersion=$(($retVersion+16))
	fi

#For upgrade all
#	retVersion=15
	echo "retVersion = $retVersion"
	if [ $retVersion -gt $SETZERO ] #-gt:大于
	then
		echo "provision killapp ...... "
		/usr/local/sbin/killapp	
		#REBOOT=true;	# add by lwx, if have upgrade, must reboot system 
	else #同一个版本的软件不能升级，低于或高于当前版本是可以升级
		echo "FW is not valid for upgrade, or same version"
		echo 4 > /tmp/update.txt
		exit 1
	fi

    else
	retVersion=15 
	echo "provision killapp ...... "
	/usr/local/sbin/killapp	
	#REBOOT=true;	# add by lwx, if have upgrade, must reboot system 
fi

cd ${UPDATA_PATH}/

updateStatus=0


#echo "upgrate core ..."
full_image_filename=${firmware_prefix}${DEVICE_ALIAS}${CORE_IMAGE}.bin
local_file=`basename $full_image_filename`
full_image_filename=${full_image_filename}${firmware_postfix}
versionMask=$((${retVersion}&${CORE_MASK_BIT}))
echo $local_file
if [ $versionMask -eq $CORE_MASK_BIT ]
then
	echo "**************** UPDATE ${DEVICE_ALIAS}${CORE_IMAGE}.bin *****************"
	{
##		check_ongoing_call_status && 
		{			
			COREMD5_D=`md5sum ${UPDATA_PATH}/$local_file | awk '{printf $1}'`
			if [ $COREMD5_S = $COREMD5_D ]
			then						
  		 		stop_prog
       		 		#prompt upgrade info
		 		REBOOT=true
				UPDATE_LOCAL_VERSION=true

				flash_eraseall  /dev/mtd1 # 擦除flash 
				flashcp -v  ${UPDATA_PATH}/$local_file  /dev/mtd1 #写入flash
				updateStatus=$((${updateStatus}|${CORE_MASK_BIT}))
				echo "${DEVICE_ALIAS}${CORE_IMAGE}.bin"
			else
				echo "$full_image_filename md5 check error:"				
				echo "$COREMD5_S"
				echo "$COREMD5_D"
			fi
		}
	}
	rm -f ${UPDATA_PATH}/$local_file
else
	echo "$full_image_filename is not valid for upgrade, or same version"
fi

echo 1 > /tmp/update.txt #对应的ASCII码是49

echo "upgrate base ..."
full_image_filename=${firmware_prefix}${DEVICE_ALIAS}${BASE_IMAGE}.bin
local_file=`basename $full_image_filename`
full_image_filename=${full_image_filename}${firmware_postfix}
versionMask=$((${retVersion}&${BASE_MASK_BIT}))
echo $local_file
if [ $versionMask -eq $BASE_MASK_BIT ]
then
	echo "**************** UPDATE ${DEVICE_ALIAS}${BASE_IMAGE}.bin *****************"
	{
##		check_ongoing_call_status && 
		{			
			BASEMD5_D=`md5sum ${UPDATA_PATH}/$local_file | awk '{printf $1}'`
			if [ $BASEMD5_S = $BASEMD5_D ]
			then						
  		 		stop_prog
       		 		#prompt upgrade info
		 		REBOOT=true
				UPDATE_LOCAL_VERSION=true

				flash_eraseall  /dev/mtd2
				flashcp -v  ${UPDATA_PATH}/$local_file  /dev/mtd2
				updateStatus=$((${updateStatus}|${BASE_MASK_BIT}))
				echo "${DEVICE_ALIAS}${BASE_IMAGE}.bin"
			else
				echo "$full_image_filename md5 check error:"				
				echo "$BASEMD5_S"
				echo "$BASEMD5_D"
			fi
		}
	}
	rm -f ${UPDATA_PATH}/$local_file
else
	echo "$full_image_filename is not valid for upgrade, or same version"
fi

echo 2 > /tmp/update.txt

echo "upgrate app1 ..."
full_image_filename=${firmware_prefix}${DEVICE_ALIAS}${APP1_IMAGE}.bin
local_file=`basename $full_image_filename`
full_image_filename=${full_image_filename}${firmware_postfix}
versionMask=$((${retVersion}&${APP1_MASK_BIT}))
echo $local_file
if [ $versionMask -eq $APP1_MASK_BIT ]
then
	echo "************ UPDATE ${DEVICE_ALIAS}${APP1_IMAGE}.bin **************"
	{
	    {		
		APP1MD5_D=`md5sum ${UPDATA_PATH}/$local_file | awk '{printf $1}'`
		if [ $APP1MD5_S = $APP1MD5_D ]
		then
			stop_prog
			#prompt upgrade info
			REBOOT=true
			UPDATE_LOCAL_VERSION=true

			flash_eraseall  /dev/mtd3
			flashcp -v  ${UPDATA_PATH}/$local_file  /dev/mtd3
			updateStatus=$((${updateStatus}|${APP1_MASK_BIT}))
		else
			echo "$full_image_filename md5 check error:"
			echo "$APP1MD5_S"
			echo "$APP1MD5_D"
		fi
	    }
	}
	rm -f ${UPDATA_PATH}/$local_file
else
	echo "$full_image_filename is not valid for upgrade, or same version"
fi

echo "upgrate www ..."
full_image_filename=${firmware_prefix}${DEVICE_ALIAS}${WWW_IMAGE}.bin
local_file=`basename $full_image_filename`
full_image_filename=${full_image_filename}${firmware_postfix}
versionMask=$((${retVersion}&${WWW_MASK_BIT}))
echo $local_file
if [ $versionMask -eq $WWW_MASK_BIT ]
then
	echo "************ UPDATE ${DEVICE_ALIAS}${WWW_IMAGE}.bin **************"
	{
	    {		
		WWWMD5_D=`md5sum ${UPDATA_PATH}/$local_file | awk '{printf $1}'`
		if [ $WWWMD5_S = $WWWMD5_D ]
		then
			stop_prog
			#prompt upgrade info
			REBOOT=true
			UPDATE_LOCAL_VERSION=true

			flash_eraseall  /dev/mtd4
			flashcp -v  ${UPDATA_PATH}/$local_file  /dev/mtd4
			updateStatus=$((${updateStatus}|${WWW_MASK_BIT}))
		else
			echo "$full_image_filename md5 check error:"
			echo "$WWWMD5_S"
			echo "$WWWMD5_D"
		fi
	    }
	}
	rm -f ${UPDATA_PATH}/$local_file
else
	echo "$full_image_filename is not valid for upgrade, or same version"
fi

echo "upgrate logo ..."
full_image_filename=${firmware_prefix}${DEVICE_ALIAS}${LOGO_IMAGE}.bin
local_file=`basename $full_image_filename`
full_image_filename=${full_image_filename}${firmware_postfix}
versionMask=$((${retVersion}&${LOGO_MASK_BIT}))
echo $local_file
if [ $versionMask -eq $LOGO_MASK_BIT ]
then
	echo "************ UPDATE LOGO **************"
	{
	    {		
		LOGOMD5_D=`md5sum ${UPDATA_PATH}/$local_file | awk '{printf $1}'`
		if [ $LOGOMD5_S = $LOGOMD5_D ]
		then
			stop_prog
			#prompt upgrade info
			REBOOT=true
			UPDATE_LOCAL_VERSION=true

			flash_eraseall  /dev/mtd7
			flashcp -v  ${UPDATA_PATH}/$local_file  /dev/mtd7
			updateStatus=$((${updateStatus}|${LOGO_MASK_BIT}))
		else
			echo "$full_image_filename md5 check error:"
			echo "$LOGOMD5_S"
			echo "$LOGOMD5_D"
		fi
	    }
	}
	rm -f ${UPDATA_PATH}/$local_file
else
	echo "$full_image_filename is not valid for upgrade, or same version"
fi



echo 3 > /tmp/update.txt
if [ $UPDATE_LOCAL_VERSION ]; then
	if [ ${updateStatus} -eq ${retVersion} ]; then
		echo "update version file !"
		cp $version_remote_file $LOCAL_VERSION_FILE -f
	fi
fi


if $PROG_STOPPED; then
    #fbv /logo/upgrade5.jpg
    echo nothing to do 
fi

if $REBOOT; then
    check_ongoing_call_status && {
	    syslog "Reboot after provision" 
	    #/tmp/srst
		#killall -9 watchdog
		cd /
		echo 4 > /tmp/update.txt
		#umount ${MOUNT_PATH}/
		sleep 3
		reboot
    }
else
	syslog "No image upgraded" 
fi

#/usr/local/sbin/check.sh 
