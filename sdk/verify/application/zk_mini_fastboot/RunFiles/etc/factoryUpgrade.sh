killall MyPlayer
killall wpa_supplicant
killall udhcpc
sleep 1
umount /customer
sleep 1
/ota/otaunpack -x /ota/SStarOta.bin.gz
