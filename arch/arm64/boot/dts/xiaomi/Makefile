dtbo-$(CONFIG_MACH_XIAOMI_F10) += \
    sm7150-davinci-overlay.dtbo \
    sm7150-davinci_m0a-overlay.dtbo

sm7150-davinci-overlay.dtbo-base := common/qcom-base/sdmmagpie.dtb
sm7150-davinci_m0a-overlay.dtbo-base := common/qcom-base/sdmmagpie.dtb

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)

$(obj)/%.dtbo:$(src)/../../../../arm64/boot/dts/xiaomi/%.dts FORCE
	$(call if_changed_dep,dtc)
	$(call if_changed,dtbo_verify)

dtbs: $(addprefix $(obj)/,$(dtb-y)) $(addprefix $(obj)/,$(dtbo-y))
clean-files	:= *.dtb *.dtbo
