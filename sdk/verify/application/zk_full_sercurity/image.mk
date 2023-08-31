app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/bin/zkgui $(IMAGE_INSTALL_DIR)/customer/zkgui
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/bin/MyPlayer $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/lib $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/libdns $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/libsercurity $(IMAGE_INSTALL_DIR)/customer
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/lib/*
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/res $(IMAGE_INSTALL_DIR)/customer
	@cp -rfP $(APPLICATION_PATH)/zk_full_sercurity/browser $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_sercurity/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/zk_full_sercurity/demo.sh >> $(IMAGE_INSTALL_DIR)/customer/demo.sh
	@echo "nameserver 8.8.8.8" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf

