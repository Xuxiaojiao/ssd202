date 032912122020.12
mkdir /var/tmp
mkdir /var/run
#echo 7M > /sys/block/zram0/disksize
#mkswap /dev/zram0
#swapon /dev/zram0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/customer_app/lib:/customer_app/libsercurity:/lib:/config/lib:/customer/lib
export TZ=CST-8
echo 12 >/sys/class/gpio/export
echo out >/sys/class/gpio/gpio12/direction
echo 0 >/sys/class/gpio/gpio12/value
echo 14 >/sys/class/gpio/export
echo out >/sys/class/gpio/gpio14/direction
echo 0 >/sys/class/gpio/gpio14/value
mount -t ubifs ubi0:ota /ota
cd /customer_app
chmod 777 zkgui
./zkgui &
#2>&1 |tee -a /ota/log.txt &
