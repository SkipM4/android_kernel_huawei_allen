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

## r28: Approved Codec-Only Integration

The user approves the separate codec import/adaptation, compilation and
recoverable paired flash. Import the 23 audited files from the pinned
Hepburn/Charlotte commit into vendor/asp_codec_codecless/{base,platform/clt,store}.
Retain their GPLv2 notices. Do not import the external PCM/DSP implementation,
other-platform or PC branches, firmware, device tree or calibration data.

Local adaptations:

- Connect the existing SND_SOC_ASP_CODEC_CODECLESS option to the child
  directory. Make it built-in-only and dependent on CHAR_IP_PLATFORM,
  reflecting the supported CLT implementation and its ordered initcalls.
  The current option remains y; the incomplete old V2C Kconfig is not sourced.
- Curate both Makefiles for CLT/phone, using subdir-ccflags-y for common
  includes. Link the CLT data and all path registrations before the common
  component driver. The final ELF confirms arch_initcall data setup, then
  path device_initcalls, component registration and the late machine driver.
  No speculative sound-card reordering is needed.
- Replace hardcoded mapping lengths with ordered platform IORESOURCE_MEM
  resources. Validate the six expected page bases at this DT boundary and
  map exactly each supplied length with devm_ioremap. Clamp the last u32
  register offset to the resource boundary; the AXI2TDM definitions end at
  0xCC and the MAD direct definitions fit inside its 0x1000-byte resource.
  Do not claim the pages exclusively: the existing clock/MAD drivers share
  them. Do not expand the DT to accommodate the reference's extra four bytes.
- Pass pdev to the mapping helper and remove manual unmapping and the empty
  platform remove callback. Devres unregisters the component before releasing
  its mappings. Preserve the original DSP-intercore register locking protocol
  and PM/reset/DAPM behavior; it is not a redundant host RMW lock.

The first build exposes a Kbuild include-root error; correcting the child
Makefile's source-root path resolves it without header stubs. Clang 12.0.7
ThinLTO/CFI compilation, staged diff checks, boot round-trip, and signed modem
staging pass. All 390 versioned modem imports are present with matching CRCs.
These static checks do not certify runtime power management or all audio paths.

The approved paired flash and a normal reboot both reach the system on #28.
The normal codec-backed card and populated audio policy manager now register.
Silent playback consumes all frames with the logical built-in speaker route;
the post-reboot lifecycle check observes a running PCM, then closure and codec
runtime autosuspend with balanced path power events and clock disable.
Readback matches both retained r28 images and the unchanged ramdisk baseline.
No kernel WARN splat, BUG, CFI failure or panic is found in the captured full
boot/test logs; SELinux Enforcing, forced module signing and taint 4 remain.

Two ASoC unconnected-mux diagnostics remain for MDM_5G_UP_L_MUX and
SIDETONE_PAG_DIN_MUX. Comparison against the pinned reference confirms that
the platform route file and these widget definitions are semantically
unchanged by the import. The CLT ultrasound route uses the right MDM 5G mux;
neither the left mux nor sidetone mux is connected there. Do not infer missing
Allen hardware routes from symmetric names, remove the controls to silence
messages, or claim call/sidetone correctness from normal playback.

For device results and limitations see the r28 section of
clang12-functional-tests.md. The user subsequently confirms that playback
in the system sound settings sounds normal on r28. This supplies the basic
audible-output observation missing from the silent probe, not a measurement
of fidelity or validation of other audio paths.
For the r28 integration step, keep r27 as the immediate paired rollback and
preserve the original images and independent Magisk ramdisk. Microphone,
audible fidelity, call/sidetone paths and recorded-video audio require their
own meaningful checks; silent playback is not full audio certification.

The r30 follow-up now passes basic unsilenced MIC-source streaming through the
same imported codec and unchanged local PCM/DSP: a mono 48 kHz client receives
all requested samples over the six-channel S16 hardware capture path, then
the PCM closes and codec clocks/runtime power release. No recording is saved.
Application capture timestamps advance after startup in a targeted follow-up,
but HAL-side hardware timing statistics remain unavailable. Physical microphone
mapping, acoustic fidelity, hardware capture timing, calls and active-audio
sleep remain outside that result. See the r30 capture section of
clang12-functional-tests.md; the current paired rollback is recorded in README.

## r30: Read-Only HAL Capture-Position Boundary Audit

The r30 MIC check receives nonzero PCM and advancing application timestamps,
but AudioFlinger reports no valid HAL capture timestamps and accumulating
timestamp errors. Do not infer a PCM/codec failure from those counters alone.
Read-only process mappings identify the actual primary HAL and V7_0 HIDL
implementation loaded by the unchanged audio service. The analysis uses
verified read-only ELF copies, not an unrelated repository's HAL binary.
The separate file-read denial is adbd to vendor_file, file:getattr; existing
Magisk root reads suffice without changing SELinux, labels or permissions.

In /vendor/lib64/hw/audio.primary.hisi.so, the normal adev_open_input_stream
path allocates a zeroed input structure and installs its normal read and
other supported methods, but leaves get_capture_position unimplemented.
The audio_stream_in ABI places that callback after the fourteen common
stream methods and set_gain/read/get_input_frames_lost. The loaded HIDL
StreamIn::getCapturePositionImpl checks the corresponding offset, returns
Result::NOT_SUPPORTED when it is null, and calls a populated callback only
otherwise. Its behavior matches the AOSP Android 12 implementation. The
initial candidate that directly returns -ENOSYS belongs to an offload output
table, not this normal input; exclude it from MIC-path evidence.

Finding a pcm_get_htimestamp import in the primary HAL or a capture-position
helper in loaded libalsautils is insufficient: neither establishes that the
normal MIC callback invokes it. The primary HAL's direct htimestamp call is
in its output-position path. TinyALSA can synchronize status/control and
retrieve PCM status timestamps, but this missing input callback prevents
that query through the audited HAL entry point. No failed capture timestamp
ioctl is demonstrated here.

The standard AudioFlinger RecordThread updates LOCATION_SERVER from frames
read and the read-completion time before obtaining a HAL capture position.
A failed/unsupported HAL query increments the timestamp verifier's error
counter without deleting the server timestamp. This explains the observed
application/HAL distinction; it is not proof of hardware-origin precision.
The local ALSA core owns STATUS/SYNC_PTR and default system timestamp updates;
absence of a driver-specific get_time_info hook alone does not establish a
missing status ioctl or justify adding a synthetic timestamp hook.

References: AOSP android-12.0.0_r1 libhardware include/hardware/audio.h,
hardware/interfaces audio/core/all-versions/default/StreamIn.cpp, and
frameworks/av media/libaudiohal/impl/StreamHalHidl.cpp and
services/audioflinger/Threads.cpp; compare the relevant contracts against the
loaded ELF implementations rather than transplanting code by version alone.
Kernel references are the unchanged official 5.10.43 audio_pcm_hifi.c and
sound/core/pcm_native.c/pcm_lib.c, not a new codec timestamp implementation.

This resolves the layer attribution, not the unimplemented stock HAL feature
or exact hardware capture timing. No HAL source is supplied by the codec
candidate; do not patch proprietary binaries, inject a replacement HAL,
suppress verifier errors, synthesize positions, alter PCM capabilities or
relax SELinux to manufacture a pass. No new capture, kernel change, build,
flash or service restart occurs during this audit. r30 remains installed and
the temporary ELF/disassembly/reference copies are removed after analysis.
The subsequent local system-recorder check is reported normal by the user.
This adds basic user-observed recording validation without requesting or
receiving audio content; it does not certify measured fidelity, recorder route
mapping or each physical microphone. No repeated sampling or device change
is needed to record that report. Call and hardware-timing work still require
their own evidence and scope.
