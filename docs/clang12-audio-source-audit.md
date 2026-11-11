# Audio Codec Source And Platform Audit

Date: 2026-11-11. Installed kernel remains r24, source baseline
`210a63099`, on ALN-AL00 / HWALN / Kirin 9000S, serial
`FMR0223C20002621`. This iteration changes no kernel code, configuration,
DT, firmware, ramdisk or partition. No external codec implementation is
adopted, compiled or run on the device.

## Local Binding And Source Gap

The original firmware-derived running DT retains enabled nodes:

- `asp_codec_codecless@FAB52000`, compatible
  `hisilicon,asp-codec-codecless`, with six physical reg tuples.
- `analog_less_machine`, compatible `hisilicon,analog-less-machine`.
- `soc_mad`, compatible `hisilicon,soc-mad`.
- `asp_pcm_soc_dsp_config`, compatible `soc-dsp-phone-pcm-config`.

The actual `analog_less_machine.c` selects component name
`asp-codec-codecless` and DAI name `asp-codec-codecless-dai` for its links.
Its -EPROBE_DEFER return is not a reason to force success or substitute a
dummy codec: the absent component still needs to program real hardware and
provide the expected mixer/path contracts.

The original stock config enables SND_SOC_ASP_CODEC_V2C, AXI2TDM and MAD_V2.
The current parent codecs/Kconfig does not source the child definitions,
and codecs/Makefile does not descend into the codec directory. More
importantly, both the extracted tree and the original official
ALN_HarmonyOS4.2.0_Code_Opensource.tar.gz contain only Makefile and Kconfig
in `platform_source/audio/sound/soc/codecs/vendor/asp_codec_codecless`.
This is not an extraction omission that can be fixed by enabling an option.
The local soc_mad implementation exists; do not infer that all auxiliary
audio sources are missing merely from the codec gap.

## Pinned External Candidates

### Same-Generation Kirin 9000S Release

Repository: `HK416AAA/HUAWEI_Kirin9000S_Kernel`, commit
`991c4dd5c219f90fa0575086bf565c991b3ab2ee`, Linux 5.10.43.
Its same codec directory likewise contains only Kconfig and Makefile.
It does not supply the missing implementation. This is a source-coverage
comparison, not a certification of every device/release difference.

### Kirin 9000 Release

Repository: `HK416AAA/HUAWEI_Kirin9000_Kernel`, commit
`170c9e979789b1078c1ecdf3816a1766334998fd`, Linux 5.10.43.
Its platform_source/audio has a Baltimore directory containing interface.txt;
the scoped, untruncated audio/sound Git tree has no asp_codec entries.
The complete root recursive GitHub response is truncated and must not be
used as proof of global absence. This inspected subtree is not a source
provider for the missing V2C codec.

### Newer Changsha Codec Implementation

Repository: `5-Super-Rookie-5/hongmengkernel`, commit
`297b33b07b56adb3fa53baf618fdbc16ad9fd447`, Linux 5.10.97 at
`kernel/linux-5.10-lts`. It contains actual codec C sources under
`platform_source/audio/sound/soc/codecs/vendor/asp_codec_codecless`, including
base, store, audio_pinctrl and platform/chs.

Reject a direct import for Allen:

- Makefile selects platform/chs when VENDOR_CHIP_PREFIX is `changsha`;
  CHS does not mean Charlotte-S. Charlotte has a separate platform/clt rule,
  but that implementation is not present in the inspected directory.
- The CHS register header uses SOCCODEC/ASPCFG/AXI2TDM/MAD base constants
  0xFD090000 / 0xFD0BE000 / 0xFD0BD000 / 0xFDA80000 through clear_31_bit.
  The Allen DT instead begins with 0xFAB52000 / 0xFAB4E000 and includes
  0xFABDA000 / 0xFAF00000. Clearing an address bit does not reconcile these
  different register layouts or prove equivalent hardware.
- CHS declares ten real register pages plus a virtual page, including
  multiple smart-PA and haptic blocks; the Allen node has six reg tuples.
  The old V2C mapping and reset/path semantics cannot be inferred by deleting
  extra pages or changing the new constants to the old addresses.
- The generic compatible and DAI strings match the old machine driver's
  strings. That alone is insufficient evidence of platform compatibility.
  Common control code also uses a newer platform-ops/data layout, so it is
  not an independent drop-in replacement for the absent original driver.

References are inspected as source data only. They are not copied into the
kernel tree, and no external build scripts are executed.

## Next Safe Audio Step

Locate a V2C/Charlotte implementation with its utility/control/store,
playback/uplink/Bluetooth-FM, AXI2TDM and MAD paths and headers. Compare the
DT resource table, clocks/regulators, reset sequence, firmware messages,
DAI names, mixer controls and suspend/runtime-PM semantics before restoring
Kconfig/Makefile linkage. Reconstructing a driver from incompatible register
tables or stock binary bodies is not a small compatibility fix.

Until that implementation is available, keep the audio gap explicit and
continue independent functional checks. Do not claim speaker, microphone,
call audio or recorded-video audio coverage from camera photos, hardware
video coding, a dummy sound-card registration or DP-only ALSA presence.
