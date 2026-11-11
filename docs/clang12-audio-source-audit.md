# Audio Codec Source And Platform Audit

Date: 2026-11-11. Installed kernel remains r24, source baseline
`210a63099`, on ALN-AL00 / HWALN / Kirin 9000S, serial
`FMR0223C20002621`. This iteration changes no kernel code, configuration,
DT, firmware, ramdisk or partition. No external codec implementation is
adopted, compiled or run on the device.

The r27 follow-up below identifies a substantially better Charlotte candidate;
the r24 findings remain historical baseline evidence, not a statement that no
matching platform implementation has since been found.

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

### r27 Follow-Up: Same-Version Charlotte Codec Candidate

Repository: `Hooo1941/kernel_huawei_hepburn`, commit
`8d07a0c080f5e6d0a29aed5f61bac1f4a79001cf`, Linux 5.10.43 under `kernel`.
Its Makefile declares charlotte and the codec child Makefile selects
platform/clt through CONFIG_CHAR_IP_PLATFORM, which is enabled locally.
This is a matching-platform source candidate, not yet a certified ALN driver.

The untruncated codec subtrees contain 23 files: two Makefiles and 21 C/header
files, approximately 11,400 source lines. They include base control/utils,
CLT driver/registers/routes, and store playback, uplink, Bluetooth-FM,
sidetone, AXI2TDM and MAD paths, with GPLv2 notices on the driver sources.
Unlike the inspected Changsha implementation, all six physical page bases
and their order exactly match the firmware-derived Allen DT.

Read-only live codec of_node/reg also matches the backup's complete resource
table; no register contents are read to make this comparison.

| Page | Physical Base | Allen DT Length |
| --- | --- | --- |
| SOCCODEC | 0xFAB52000 | 0x1000 |
| ASPCFG | 0xFAB4E000 | 0x400 |
| AO_IOC | 0xFB228000 | 0x1000 |
| IOC | 0xFF800000 | 0x1000 |
| AXI2TDM | 0xFABDA000 | 0xD0 |
| MAD | 0xFAF00000 | 0x1000 |

Additional compatibility evidence and limitations:

- Compatible, component name and asp-codec-codecless-dai match the local
  machine driver. The codec supplies actual register programming and DAPM
  controls/routes, not a dummy replacement for the absent component.
- The required clk_asp_subsys / clk_asp_codec names exist in the local
  Charlotte clock provider. The candidate also requests regulator asp and
  default/idle pinctrl, consumes udp_io_config and pm_runtime_support, and
  resets/enables the codec and AXI2TDM through its CLT register definitions.
  Allen supplies both pinctrl states, udp_io_config=1 and runtime-PM support.
  Its optional micbias GPIO property is absent; do not invent one.
- Candidate soc_mad.h and ulsr.h are byte-identical to local headers. The
  audio_log.h differences inspected are logging/header conventions. Do not
  infer whole-audio compatibility: phone_pcm_config.c differs, including DDR
  mapping organization. Keep the installed-firmware-facing local PCM/DSP
  implementation rather than importing the entire external audio tree.
- The candidate's codec_debug.h include is conditional on
  CONFIG_SND_SOC_CODEC_DEBUG, currently disabled in stock/current configs.
  The ARM-PC branch also names additional absent PC files; do not enable it
  for this phone or add empty headers/functions to bypass missing features.
- Its mapping helper still uses hardcoded ioremap bases, not the supplied
  DT resources. It computes length as end - begin + sizeof(int). The
  AXI2TDM_END=0xD0 and MAD_END=0x1000 constants therefore map four bytes past
  the corresponding Allen DT ranges. This must be reconciled with actual
  register use and DT boundaries before device testing, not solved by
  enlarging the DT or blindly copying the mapping code. Other four pages
  fit within the supplied resources.
- The Makefile is reorganized around CLT/base/store, not the old V2C object
  names in the local incomplete directory. Restore coherent child linkage,
  not just old flags whose referenced files remain absent. Preserve CLT
  arch_initcall data setup and path-list initialization before component
  registration, and review the ASoC/CFI callback types and PM cleanup paths.

The fresh read-only device check remains #27, normal mode, boot_completed=1,
SELinux Enforcing and taint 4. ALSA still exposes only audio-dp. No kernel
source/config, DT, firmware, ramdisk, calibration data or partition is changed
in this follow-up; no audio functional pass or compilation is claimed.

## Next Safe Audio Step

Proposed scope: a separate codec-only import from this pinned Charlotte
candidate, approximately 11,400 source lines, followed by narrow DT-resource
and build integration. Confirm this larger driver addition before proceeding;
it is materially different from the prior one-line warning fixes. Expected
benefit is restoring the real codec-backed sound card and allowing the HAL to
initialize. Risks include compile/API incompatibility, runtime-PM/reset
behavior, audio-path failure and hardware register writes during probe.

After scope confirmation: keep r27 as immediate rollback, preserve local
PCM/DSP and original images, verify build and signed paired staging first,
then check normal boot, component/card registration, AudioPolicyManager and
silent synthetic playback. Do not initially record microphone data, change
system volume, or claim speaker/call/recording fidelity from registration.
No trial writes to unknown registers or speculative DT expansion are allowed.

Until a compatible implementation is integrated and tested, keep the audio
gap explicit and continue independent functional checks. Do not claim speaker, microphone,
call audio or recorded-video audio coverage from camera photos, hardware
video coding, a dummy sound-card registration or DP-only ALSA presence.
