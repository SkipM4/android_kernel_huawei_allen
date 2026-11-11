# Allen Kernel Bring-Up

Build the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux
before attempting newer kernels. Project safety rules are in `AGENTS.md`.

## Current Status

Clang 12.0.7 builds a signed, ABI-matched boot/modem_driver pair with enforcing
CFI. r16 reaches the Huawei home screen on two boots, but fails after about an
hour when storaged exercises a hyperframe direct-I/O hook. r17 supplies typed
CFI entries for the audited hyperframe callbacks, reaches the home screen on
two boots and passes actual direct-I/O read/write checks. Its second boot runs
continuously for over 65 minutes without panic or restart, beyond r16's known
failure window. r17 is the first accepted self-built normal-boot baseline and
is retained as the tested rollback pair. This establishes the boot goal, not
complete OEM feature parity or production stability. See `docs/clang12-working-baseline.md` for
reproduction, the accepted image pair, rollback and remaining limitations.

Use the stock-derived compatibility build in `docs/stock-module-compat.md`
for paired device tests, not the initial released-configuration build below.
See `docs/clang12-cfi-iterations.md` for each attributed failure and recovery.

Functional follow-up reaches r20: static sensorhub route-lock initialization
and ION reuse eligibility repairs pass paired flash and targeted sensor/ION,
storage, USB and display sleep/wake checks. r20 repairs video decoder IRQ-flags
writeback and passes named hardware AVC/HEVC decode and live accel/gyro/mag
event tests. r20 is the recorded hardware-functional reference, not full certification.
The user reports preliminary front/rear camera switching and photos working
on r19. Multi-level L3 voting remains unresolved.
See `docs/clang12-functional-tests.md` for results and the next isolated fix.

The current installed test candidate is r30: r21 retains full early logs,
r22 initializes the TCP recovery rule lock, and r23 uses sleeping GPIO reads
in the satellite init thread. r24 removes invalid PD sysfs executable bits
without changing its effective creation mode or store callback. r25 keeps ION
as the sole owner of its existing proc interfaces, removing duplicate memcheck
registration without changing their ABI. r26 removes duplicate version
declarations from combined TCPC/NPU modules, keeping the active version values.
Paired readback, real ION allocation/mapping/accounting and registration checks
pass. r27 reserves the PRMEM physical database during the existing early
memblock stage without delaying write protection or changing its selected
physical range. It boots normally twice with no WARNING in either full early
log; taint falls from 516 to 4 (the recorded CPU-feature S bit remains).
Database capacity/accounting, Reserved page ownership and protected SELinux
pools pass targeted checks, not adversarial protection certification.
See `docs/clang12-early-diagnostics.md` for attribution and
test limits. Accepted r17 and the latest r26-r30 pairs are retained; r29 is
the immediate r30 rollback and r27 is the pre-audio rollback.
r28 imports the audited Linux 5.10.43 Hepburn/Charlotte CLT codec, absent from
the ALN archive, and maps its six pages within the existing Allen DT resources.
Local PCM/DSP, DT, firmware, calibration and independent Magisk ramdisk remain
unchanged. The normal codec-backed sound card and audio policy manager now
register on two normal boots. Silent 48 kHz stereo playback consumes all
96,000 frames with zero underruns and a built-in speaker route. A follow-up
observes the real PCM running, then closing, balanced playback power events
and codec autosuspend after clock disable. No kernel WARNING, BUG, CFI failure
or panic is found in the captured full boot/test logs; taint remains 4.
The user also confirms normal audible playback in the system sound settings
on r28; this is a basic user-observed playback pass, not fidelity certification.
A 30-second screen-off/wake check also passes silent playback and codec runtime
release without device/audio-service restart. USB/charging wakeup sources stay
active and system-suspend counters do not advance while the cable is attached.
The subsequent unplugged check confirms one 71-second deep suspend and codec
power-down/restore without reboot, but UFS resume emits an unbalanced hub IRQ
enable warning and taint rises to 516. r29 supplies the missing initial
is_irq_enabled state after successful hub IRQ registration. Its paired boot,
readback and actual O_DIRECT write/read/compare checks pass with UFS IRQ
activity, no kernel WARNING and taint 4. The first unplugged recheck and two
later deep resumes no longer emit the UFS warning; taint stays 4. Post-resume
silent playback, PCM/clock release and actual O_DIRECT write/read/compare
also pass without device/audio-service restart. Two eventpoll wakeups abort
freezing before device suspend, not a UFS/codec resume failure. RTC wakeups
expose a separate NO_SUSPEND/wake-IRQ contract conflict; see
`docs/clang12-functional-tests.md` for the tested scope and next isolated step.
r30 applies a SoC-only RTC IRQ flag correction after auditing the handler's
MMIO lifetime and workqueue notification path. It preserves the PMU RTC,
alarm programming and wake-enable pair. Compilation, signed paired flash,
readback and read-only RTC time progression pass; wakeup remains enabled,
boot logs have no kernel WARNING and taint is 4. The unplugged recheck completes
four deep suspends with approximately 226 seconds of cumulative sleep and no
suspend/resume failure. Two wakes are attributed to the SoC RTC, with its
ordinary handler executed and no misconfigured-IRQ diagnostic. UFS and idle
codec callbacks recover normally; post-resume silent playback consumes all
96,000 target frames without underruns, then the PCM closes and codec clocks
disable with runtime autosuspend. Device and audio-service identities remain
unchanged; full logs have no kernel WARNING and taint stays 4. r29 remains the
immediate paired rollback. This validates short idle RTC/deep wake, not all
alarm delivery cases or long-duration power parity.
An r30 follow-up also passes a short MIC-source capture: the correctly
attributed shell client receives 96,000 mono 48 kHz samples from the bottom
built-in microphone, with nonzero PCM and no client silencing. The unchanged
PCM/DSP path captures six S16_LE channels at 48 kHz underneath the mono HAL
stream. Capture stops and closes, codec clocks disable and runtime autosuspend
returns without restarting the device or audio services. For the probe, only
aggregate statistics are retained; no audio file is saved and no permission
or privacy setting is changed. A targeted follow-up obtains advancing
application capture timestamps after startup without regression. HAL-side
timestamp statistics remain unavailable. A read-only boundary audit identifies an unimplemented
get_capture_position callback in the unchanged stock primary HAL's normal
input initialization; the loaded HIDL layer returns NOT_SUPPORTED without an
ALSA timestamp query. No kernel timestamp workaround is justified by that
result. The user subsequently confirms normal recording in the system
recorder on r30. This adds a basic user-observed recording pass; no recorded
content is requested or transferred. These checks are not microphone
fidelity/channel or hardware timestamp certification.
The later r30 runtime audit still finds recurring battery OCV/capacity lookup
failures, thermal-service fallback, PCIe PME/AER MSI affinity failures and a
missing per-process RTG interface. Their root causes and functional impact
are not yet established; see the remaining-runtime-diagnostics section of
`docs/clang12-functional-tests.md` for coverage limits and prioritized tests.
Two source-inherited unconnected MUX diagnostics remain. Audible fidelity,
individual microphone mapping, hardware capture timing, calls/sidetone and
active-audio/long-duration deep suspend are not certified; see
`docs/clang12-audio-source-audit.md` and `docs/clang12-functional-tests.md`.
A correctly attributed shell-context probe passes Bluetooth ON/OFF on r24;
named hardware AVC/HEVC encoders each complete thirty synthetic frames and
EOS, with VENC IRQ activity. Pairing/peer transfer, video decode fidelity,
camera recording and full audio functionality remain outside these checks; see
`docs/clang12-functional-tests.md` for the exact scope.
Wi-Fi foreground scanning passes on r26 after explicitly waking the device
and foregrounding native settings; scanner results and successful driver
completion agree. Its existing saved network automatically associates and
obtains DHCP configuration. The rejected shell scan probe is not a hardware
failure; explicit external connectivity and sustained transfer remain untested.
Calibration-partition open errors (-EACCES) coexist with these successes and
need separate lifecycle diagnosis without relaxing protection.

## Layout

- `Code_Opensource/kernel/`: official sources with targeted bring-up fixes.
- `configs/allen-stock-5.10.43.config`: configuration read from the running phone.
- `tools/toolchains/clang-r416183b1/`: local Android Clang 12.0.7 toolchain.
- `tools/host/bin/bc`: local build dependency, extracted from the Arch package.
- `out/allen-clang12/`: independent kernel output directory; not tracked.
- `out/allen-clang12-stock-compat/`: stock-derived paired-test output; not tracked.
- `images/original/`: firmware device-tree backup; not tracked.
- `docs/bringup-baseline.md`: device baseline, toolchain provenance and risks.
- `docs/stock-module-compat.md`: installed modem ABI and signing compatibility.
- `docs/clang12-r2-flash-test.md`: r2 CFI boot failure, verified rollback and next fix.
- `docs/clang12-cfi-iterations.md`: targeted CFI fixes and subsequent paired tests.
- `docs/clang12-working-baseline.md`: accepted r17 boot baseline and test scope.
- `docs/clang12-functional-tests.md`: functional repairs, checks and audio/BT gaps.
- `docs/clang12-early-diagnostics.md`: r21-r27 log, lock, GPIO, PD, registration and PRMEM fixes.
- `docs/clang12-audio-source-audit.md`: codec source audit and scoped r28 CLT integration.

The original source archive, `boot.img` and `ramdisk-magisk.img` remain at the
project root and are excluded from Git. Never overwrite them.

## Build

From `/home/data/Allen`:

```sh
scripts/build-allen.sh merge_charlotte_defconfig
scripts/build-allen.sh
```

The target and compiler revision come from the official README and Makefile.
The wrapper uses LLVM tools and the integrated assembler, not the host GCC for
ARM64. `JOBS` overrides the default eight build jobs; `OUTPUT_DIR` selects an
independent output directory. `LOCALVERSION=` keeps the
stock `5.10.43` release string instead of adding a Git suffix; this alone does
not establish module ABI compatibility.
ThinLTO links use two backend jobs and a bounded cache to avoid the observed
Full LTO memory failure and excessive cache growth.

Additional make targets can be passed directly, for example:

```sh
scripts/build-allen.sh Image.lz4
```

Full output goes to `.tmp/build-clang12.log`. Inspect only the relevant error
context, record durable conclusions, then delete the log when no longer needed.
The release configuration selects LZ4, so the default kernel output is
`out/allen-clang12/arch/arm64/boot/Image.lz4`. The README's generic `Image.gz`
example does not describe the actual default selected by this configuration.

## Device And Images

The verified device serial is `FMR0223C20002621`. Always pass `-s` to ADB and
fastboot. The user confirms `HWALN` is the device's abbreviation for HW Allen.

The device has separate `boot` and `ramdisk` partitions. The original boot image
uses Android header version 2 and includes a small firmware ramdisk stub; the
Magisk image uses header version 0 and belongs to the separate ramdisk partition.
Preserve the stock boot structure, stub and device tree when replacing its
kernel. Do not merge the Magisk ramdisk into boot.

Configuration success or compilation success is not a boot or module-ABI test.
Do not flash until the candidate image, target partition, recovery path and
fastboot identity have been verified according to `AGENTS.md`.

The initial released-config kernel does not satisfy the installed modem module's
symbol CRCs or CFI imports. See `docs/stock-module-compat.md` for the separate
stock-derived configuration, verified OEM public certificate and ABI checker.

The ThinLTO/CFI `r2` candidate is a matching boot/modem_driver pair, not a
boot-only replacement. Both test images and their rollback requirements are
recorded in that document. The paired flash test reaches a MAS block-completion
CFI panic before Android starts; both originals are restored and normal boot is
verified. See `docs/clang12-r2-flash-test.md`. Do not reuse r2 as a working kernel.
Subsequent fixes reach normal Android; r16's later direct-I/O CFI failure is
repaired in the accepted r17 baseline. Originals are restored after failed
tests; functional candidates later replace the installed r17 while its pair
is retained as a rollback baseline. See
`docs/clang12-cfi-iterations.md` for the complete failure and recovery history.

For future versions, create a new boot candidate from the uncompressed Image:

```sh
scripts/repack-allen-boot.sh out/allen-clang12-stock-compat/arch/arm64/boot/Image \
  images/test/allen-boot-test-5.10.43-clang12-r27.img
```

The repacker refuses to overwrite an existing image and validates the actual
packed components. A corresponding ABI-matched, signed modem-driver filesystem
image is still required; a successful repack does not establish successful boot.

Stage that matching module partition from the same kernel output directory:

```sh
scripts/stage-allen-modem.sh out/allen-clang12-stock-compat \
  images/test/allen-modem_driver-test-5.10.43-clang12-r27.img
```

Use a new versioned path for each iteration. This produces the companion signed
`.ko` and an ext4 image cloned from the verified original partition, checking
ABI imports, the embedded signing certificate, CMS signature, filesystem
integrity, ownership/mode and SELinux-label readback. It refuses existing output
paths and cleans its owned temporary files. These are staging checks, not a
runtime-CFI or boot guarantee; never flash this partition without its paired boot.
