if [ $OTA_STATUS -eq 0 ]; then
        if [ `/etc/fw_printenv -n use_bak_rootfs` -eq 1 ]; then
			echo "change to rootfs"
			/etc/fw_setenv use_bak_rootfs 0
		else
			echo "change to rootfs_bak"
			/etc/fw_setenv use_bak_rootfs 1
		fi
fi 
