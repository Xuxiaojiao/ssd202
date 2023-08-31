app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/bin/zkgui $(IMAGE_INSTALL_DIR)/customer/zkgui
	#@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/bin/zkgui_bgz $(IMAGE_INSTALL_DIR)/customer/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/zkgui
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/lib $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/libdns $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/libsercurity $(IMAGE_INSTALL_DIR)/customer
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/lib/*
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/res $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_sercurity_uclibc++/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/zk_sercurity_uclibc++/demo.sh >> $(IMAGE_INSTALL_DIR)/customer/demo.sh
	@echo "nameserver 172.19.30.11" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf
	@echo "nameserver 8.8.8.8" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf

