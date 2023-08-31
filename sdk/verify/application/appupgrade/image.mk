app_bak:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif
	@echo "########make appupgrade ########"
	@rm -rf $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib
	@cp -rf $(APPLICATION_PATH)/appupgrade/bin/zkgui_1024600 $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/zkgui
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/zkgui
	@cp -rf $(APPLICATION_PATH)/appupgrade/lib $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app
	@cp -rf $(APPLICATION_PATH)/appupgrade/libsercurity $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app
	@$(TOOLCHAIN_REL)strip --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app/lib/*
	@cp -rf $(APPLICATION_PATH)/appupgrade/res $(IMAGE_INSTALL_DIR)/rootfs_bak/customer_app
	@cp -rf $(APPLICATION_PATH)/appupgrade/etc/* $(IMAGE_INSTALL_DIR)/rootfs_bak/etc/
	@cp -rf $(APPLICATION_PATH)/appupgrade/fbdev.ini $(IMAGE_INSTALL_DIR)/rootfs_bak/sstar_configs/fbdev.ini
	@cp -rf $(APPLICATION_PATH)/appupgrade/init.sh $(IMAGE_INSTALL_DIR)/rootfs_bak/etc/init.sh
	# @cat $(APPLICATION_PATH)/appupgrade/init.sh >> $(IMAGE_INSTALL_DIR)/rootfs_bak/etc/init.sh
	# @cp -rf $(APPLICATION_PATH)/appupgrade/SStarOta.bin.gz $(IMAGE_INSTALL_DIR)/ota/
	@cp -rf $(APPLICATION_PATH)/appupgrade/d.m4v $(IMAGE_INSTALL_DIR)/media/d.m4v
	# @echo "#1234567890qwertyuiop" >> $(IMAGE_INSTALL_DIR)/customer/demo.sh
	

