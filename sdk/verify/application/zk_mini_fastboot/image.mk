app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif
	@rm -rf $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs/customer_app/location
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs/customer_app/libsercurity
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer/source
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer/weather
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/d.m4v $(IMAGE_INSTALL_DIR)/media/d.m4v
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/RunFirst/* $(IMAGE_INSTALL_DIR)/rootfs/customer_app/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/RunFirst/* $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/libsercurity/* $(IMAGE_INSTALL_DIR)/rootfs/customer_app/libsercurity/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/RunLater/* $(IMAGE_INSTALL_DIR)/customer/lib/
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib/*
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/lib/*
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/RunLater $(IMAGE_INSTALL_DIR)/customer/res
	@cp  $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/ca-certificates.crt $(IMAGE_INSTALL_DIR)/rootfs/customer_app/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/weather/* $(IMAGE_INSTALL_DIR)/customer/weather
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/demo.sh >> $(IMAGE_INSTALL_DIR)/rootfs/etc/init.sh
	@cp $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/location.json $(IMAGE_INSTALL_DIR)/rootfs/customer_app/location/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/zkgui $(IMAGE_INSTALL_DIR)/rootfs/customer_app/zkgui
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/tp_write $(IMAGE_INSTALL_DIR)/rootfs/customer_app/tp_write
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/MyPlayer $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/wpa* $(IMAGE_INSTALL_DIR)/customer/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/ntpdate $(IMAGE_INSTALL_DIR)/customer/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/sendUart $(IMAGE_INSTALL_DIR)/customer/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/uartPowerOff $(IMAGE_INSTALL_DIR)/customer/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/wpa_supplicant.conf $(IMAGE_INSTALL_DIR)/appconfigs/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/wpa_supplicant.conf $(IMAGE_INSTALL_DIR)/appconfigs/wpa_supplicant.conf_bak
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/tp_write
	#@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/sendUart
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/wpa_cli
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/ntpdate
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/wpa_supplicant
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@rm -rf $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/location
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/libsercurity
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer_bak/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer_bak/source
	@mkdir -p $(IMAGE_INSTALL_DIR)/customer_bak/weather
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/RunFirst/* $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/RunFirst/* $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/libsercurity/* $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/libsercurity/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/lib/RunLater/* $(IMAGE_INSTALL_DIR)/customer_bak/lib/
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib/*
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/lib/*
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/RunLater $(IMAGE_INSTALL_DIR)/customer_bak/res
	@cp  $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/ca-certificates.crt $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/res/weather/* $(IMAGE_INSTALL_DIR)/customer_bak/weather
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/etc/* $(IMAGE_INSTALL_DIR)/rootfs_bak/etc/
	@cat $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/demo.sh >> $(IMAGE_INSTALL_DIR)/rootfs_bak/etc/init.sh
	@cp $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/location.json $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/location/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/zkgui $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/zkgui
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/tp_write $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/tp_write
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/MyPlayer $(IMAGE_INSTALL_DIR)/customer_bak/MyPlayer
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/wpa* $(IMAGE_INSTALL_DIR)/customer_bak/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/ntpdate $(IMAGE_INSTALL_DIR)/customer_bak/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/sendUart $(IMAGE_INSTALL_DIR)/customer_bak/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/bin/uartPowerOff $(IMAGE_INSTALL_DIR)/customer_bak/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/wpa_supplicant.conf $(IMAGE_INSTALL_DIR)/appconfigs/
	@cp -rf $(APPLICATION_PATH)/zk_mini_fastboot/RunFiles/wpa_supplicant.conf $(IMAGE_INSTALL_DIR)/appconfigs/wpa_supplicant.conf_bak
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/tp_write
	#@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/sendUart
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/wpa_cli
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/ntpdate
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/wpa_supplicant
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer_bak/MyPlayer
