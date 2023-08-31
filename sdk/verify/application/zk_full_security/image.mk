app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif
	@cp -rf $(APPLICATION_PATH)/zk_full_security/bin/zkgui $(IMAGE_INSTALL_DIR)/customer/zkgui
	@cp -rf $(APPLICATION_PATH)/zk_full_security/bin/MyPlayer $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/MyPlayer
	@cp -rf $(APPLICATION_PATH)/zk_full_security/lib $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_security/libdns $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_security/libsecurity $(IMAGE_INSTALL_DIR)/customer
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/lib/*
	@cp -rf $(APPLICATION_PATH)/zk_full_security/CoAPI $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_security/res $(IMAGE_INSTALL_DIR)/customer
	@cp -rfP $(APPLICATION_PATH)/zk_full_security/browser $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(APPLICATION_PATH)/zk_full_security/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/zk_full_security/demo.sh >> $(IMAGE_INSTALL_DIR)/customer/demo.sh
	@echo "nameserver 202.96.128.86" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf
	@echo "nameserver 202.96.128.143" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf
	@echo "nameserver 8.8.8.8" >> $(IMAGE_INSTALL_DIR)/rootfs/etc/resolv.conf

