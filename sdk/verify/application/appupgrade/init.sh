insmod /lib/modules/4.9.84/nls_utf8.ko
# kernel_mod_list

insmod /lib/modules/4.9.84/mhal.ko
# misc_mod_list

insmod /lib/modules/4.9.84/mi_common.ko
major=`cat /proc/devices | busybox awk "\\$2==\""mi"\" {print \\$1}"\n`
minor=0
insmod /lib/modules/4.9.84/mi_sys.ko cmdQBufSize=128 logBufSize=0 default_config_path=/sstar_configs
if [ $? -eq 0 ]; then
  busybox mknod /dev/mi_sys c $major $minor
   let minor++
fi

insmod /lib/modules/4.9.84/mi_gfx.ko
if [ $? -eq 0 ]; then
  busybox mknod /dev/mi_gfx c $major $minor
   let minor++
fi

insmod /lib/modules/4.9.84/mi_divp.ko
if [ $? -eq 0 ]; then
  busybox mknod /dev/mi_divp c $major $minor
   let minor++
fi

insmod /lib/modules/4.9.84/mi_disp.ko
if [ $? -eq 0 ]; then
  busybox mknod /dev/mi_disp c $major $minor
   let minor++
fi

insmod /lib/modules/4.9.84/mi_panel.ko
if [ $? -eq 0 ]; then
  busybox mknod /dev/mi_panel c $major $minor
   let minor++
fi

#mi module
major=`cat /proc/devices | busybox awk "\\$2==\""mi_poll"\" {print \\$1}"`
busybox mknod /dev/mi_poll c $major 0
insmod /lib/modules/4.9.84/fbdev.ko default_config_path_file=/sstar_configs/fbdev.ini
# misc_mod_list_late

mdev -s
date 032912122020.12
mkdir /var/tmp
#echo 7M > /sys/block/zram0/disksize
#mkswap /dev/zram0
#swapon /dev/zram0
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/customer_app/lib:/customer_app/libsercurity:/lib:/config/lib
echo 12 >/sys/class/gpio/export
echo out >/sys/class/gpio/gpio12/direction
echo 1 >/sys/class/gpio/gpio12/value
echo 4 >/sys/class/gpio/export
echo out >/sys/class/gpio/gpio4/direction
echo 1 >/sys/class/gpio/gpio4/value
echo 5 >/sys/class/gpio/export
echo out >/sys/class/gpio/gpio5/direction
echo 1 >/sys/class/gpio/gpio5/value
cd /customer_app
chmod 777 zkgui
./zkgui &
