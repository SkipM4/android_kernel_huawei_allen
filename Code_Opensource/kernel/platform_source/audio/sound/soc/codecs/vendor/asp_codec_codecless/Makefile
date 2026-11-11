GCOV_PROFILE := $(CONFIG_GCOV_KERNEL)
codec_dir := $(srctree)/platform_source/audio/sound/soc/codecs/vendor/asp_codec_codecless
subdir-ccflags-y += -I$(codec_dir)/base/ -I$(codec_dir)/store/ \
	-I$(codec_dir)/platform/clt/ \
	-I$(srctree)/platform_source/audio/include/ \
	-I$(srctree)/platform_source/audio/drivers/soc_mad/

# Path initcalls must precede registration of the common component driver.
obj-y += platform/clt/

snd-soc-asp-codec-base-objs := base/asp_codec_utils.o base/asp_codec_control.o store/asp_codec_store.o

obj-y += snd-soc-asp-codec-base.o
