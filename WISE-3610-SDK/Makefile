include $(TOPDIR)/rules.mk

PKG_NAME:=mbed-edge
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_SOURCES=mbed-edge-sources
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mbed-edge
    SECTION:=net
    CATEGORY:=Network
    DEPENDS:= +libstdcpp +librt +jansson +libevent2 +libmosquitto +libevent2-pthreads +mtd-utils-flash-erase +mtd-utils-nandwrite
    TITLE:=mbed-Edge
    MAINTAINER:=ARM
    URL:=http://www.arm.com/
endef

define Package/mbed/description
    mbed Edge
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR)
endef

define Build/Prepare
	cp -a $(PKG_SOURCES) $(PKG_BUILD_DIR)/$(PKG_SOURCES)

	patch $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/scripts/openwrt/arm_update_kernel.sh ./$(PKG_SOURCES)/WISE-3610-SDK/fix_kernel_update_script.patch
	patch $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/source/arm_uc_pal_linux_implementation_internal.c ./$(PKG_SOURCES)/WISE-3610-SDK/read_header_buffer_size.patch
	patch $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/pal/Examples/PlatformBSP/OpenWRT_Generic_Linux/FileSystemInit.c ./$(PKG_SOURCES)/WISE-3610-SDK/disable_pal_kcm_mount.patch

	cd $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86; \
	cmake . -DPKG_BUILD_DIR=$(PKG_BUILD_DIR) -DSKIP_LIB_CHECK=1
	# SKIP_LIB_CHECK is used to force compilation of optional components
	# that otherwise depend on a find_library check inside cmakelist.
	# If there is some way to check library exists in openwrt build system
	# we could replace this.

	cp ./$(PKG_SOURCES)/WISE-3610-SDK/mec $(PKG_BUILD_DIR)/$(PKG_SOURCES)
	cp ./$(PKG_SOURCES)/WISE-3610-SDK/sdk/scripts/mount_kcm.sh $(PKG_BUILD_DIR)
	cp ./$(PKG_SOURCES)/WISE-3610-SDK/mbed_edge_config.h $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86
	cp ./$(PKG_SOURCES)/WISE-3610-SDK/mbed_cloud_client_user_config.h $(PKG_BUILD_DIR)/$(PKG_SOURCES)
	cp ./$(PKG_SOURCES)/WISE-3610-SDK/mbed_client_user_config.h $(PKG_BUILD_DIR)/$(PKG_SOURCES)
endef

define Build/Configure
	echo Skipping configure
endef

define Build/Compile
	cd $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86; \
	make
endef

define Package/mbed-edge/install
	$(INSTALL_DIR)  $(1)/usr/sbin $(1)/etc/rc.d $(1)/etc/init.d
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86/existing/bin/pt-example  $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86/existing/bin/edge-core  $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/build/mcc-linux-x86/existing/bin/lorapt-example  $(1)/usr/sbin

	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/scripts/openwrt/arm_update_activate.sh $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/scripts/openwrt/arm_update_active_details.sh $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/scripts/arm_update_cmdline.sh $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mbed-edge-module-sources/update-client-hub/modules/pal-linux/scripts/openwrt/arm_update_kernel.sh $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mec $(1)/etc/init.d/mec
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_SOURCES)/mec $(1)/etc/rc.d/S99mec
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mount_kcm.sh $(1)/usr/sbin
endef

$(eval $(call BuildPackage,mbed-edge))
