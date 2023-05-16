ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
        dtbo-$(CONFIG_ARCH_BENGAL) += \
                bengal-beta-20241-overlay.dtbo \
				bengal-beta-20243-overlay.dtbo

bengal-beta-20241-overlay.dtbo-base := bengal-beta.dtb
bengal-beta-20243-overlay.dtbo-base := bengal-beta.dtb

endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files    := *.dtb *.dtbo
