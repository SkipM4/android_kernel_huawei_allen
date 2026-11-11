# Clang 12 Functional Follow-Up

## r18: Sensorhub Route Lock Lifetime (2026-11-11)

Base: accepted r17, source fixes at `81ad87b1a`. Target remains ALN-AL00 /
HWALN, serial `FMR0223C20002621`. At the start of this work r17 is running
normally with over four hours of uptime, Magisk root and Enforcing SELinux.

The previously recorded `package_route_tbl+0x68` bad-spinlock warning points
at the first route's buffer lock. With CONFIG_INPUTHUB_30, `init_locks()` is
called from the IOMCU_BOOT_PRE notifier, whereas the statically allocated
routes can already be accessed independently. Initialize all eight buffer
locks alongside their existing static wait queues and atomic flags, using
the kernel's `__SPIN_LOCK_UNLOCKED`/`ATOMIC_INIT` definitions. Remove the
later loop that would reinitialize these live locks. No table layout,
sensor command, firmware interface or CFI policy changes.

Reference: this tree's Linux 5.10.43 `include/linux/spinlock_types.h`, route
table definition and inputhub boot-notifier call chain; no external code.

Checks:

- `git diff --check`, Clang 12.0.7 ThinLTO build and paired image staging pass.
- All eight buffer locks in the final ELF have unlocked raw state, correct
  SPINLOCK_MAGIC and initial debug owners, including the reported offset.
- Companion modem imports: 390, zero missing exports or CRC mismatches;
  CMS verification, ext4 integrity, labels and boot structure checks pass.
- Paired boot/modem_driver flash succeeds and build #18 reaches normal
  `boot_completed=1`. The independent Magisk ramdisk is not flashed.
- First available root log starts at 13.56 seconds and has no route-buffer
  bad-spinlock report. This is partial early-log coverage, not proof that
  every early boot warning has disappeared. Taint remains 516 (S/W).
- Sensor service lists 27 hardware sensors. A real ALS `get_data` read
  returns a sample; the HAL also records fresh ALS events. Accel/gyro/mag
  open-and-read probes time out without interval configuration; their
  original disabled states are restored. This does not establish failure
  of those physical sensors, nor complete sensor coverage.

The camera service enumerates seven devices (two public), but camera launch
stops at the first-use privacy/permission consent screen. No consent is
accepted on the user's behalf, no image is captured, and camera preview
is explicitly unverified. A user-approved first launch is required before
testing this path further.

New attributed nonfatal findings in the #18 log:

- `allocator@2.0-s -> ion_ioctl -> ion_alloc -> task_judge` reports a bad
  rwlock. `ion_buffer_create()` calls the reuse helper for every heap, but
  reuse_config_lock is initialized only for reuse-enabled heaps. The helper
  also returns an uninitialized pointer on a whitelist miss. Next restrict
  it to reuse-enabled heaps and return NULL for a miss; keep normal
  allocation and enabled-heap reuse behavior intact.

- IRQ 113 reports `vfmw_isr` returning with interrupts enabled. This video
  decoder ISR issue is separate from both lock fixes and needs its own
  call-chain investigation; do not suppress the core IRQ warning.

L3 c1/c2 remains unsupported multi-level voting, not missing DT data. No
hardware register encodings, frequency table columns or DT values are guessed.
Keep the accepted r17 boot/module pair and stock originals as rollback paths.

## r19: ION Reuse Eligibility (2026-11-11)

Base: r18 sensor-route repair, commit `3ace804a9`. Change only `ion.c`:
`ion_buffer_reuse()` returns NULL before touching reuse state for heaps
without ION_HEAP_FLAG_FREE_BUF_REUSE, and returns NULL instead of a garbage
pointer when the allocating process is absent from the reuse whitelist.
`ion_buffer_create()` already interprets NULL as a reuse miss and performs
normal allocation. Reuse-enabled heaps retain their initialization, lists,
whitelist and allocation behavior. No blanket lock initialization, policy
change or disabled optimization is needed.

Reference: official Linux 5.10.43 source's ION heap flags, reuse initialization
and buffer-creation contract. This is a local OEM extension repair, not a
claim that the extension exists in upstream Linux.

Checks:

- Clang 12.0.7 ThinLTO build, diff checks and paired boot/modem staging pass;
  390 module imports match, CMS/metadata/ext4/boot checks pass.
- Paired flash reaches build #19 in normal Android. Full boot/modem_driver
  readback matches r19; the independent Magisk ramdisk remains unchanged.
- A temporary ARM64 syscall probe uses this tree's ION UAPI, queries the
  ordinary system-RAM heap (`sys_heap`), and completes sixteen 64-KiB
  allocate/map/write/read/unmap/close rounds as a non-whitelisted process.
  No secure/reserved heap, reuse whitelist or debugfs setting is changed.
  This verifies the normal fallback, not whitelist-hit recycling under load.
- USB transfer and a 2-MiB actual O_DIRECT write/read/hash comparison pass.
  The owned diagnostic binary and data directory are removed afterward.
- The first available root log starts at 13.50 seconds, covering the time
  of r18's ION fault; neither it nor the test log reports bad route spinlock,
  bad ION rwlock, CFI failure or panic. The vfmw IRQ warning still occurs.
- Wi-Fi enables, but acquires no IPv4/default route, so association and data
  transport remain unverified. Restore the original disabled Wi-Fi state.
- Display sleep and wake requests succeed. Attached USB/ADB prevents these
  checks from proving deep system suspend or idle-power parity.
- CFI configuration is unchanged, module sig_enforce is Y, SELinux remains
  Enforcing and the signed modem module is loaded. Taint remains 516.
- A normal reboot again reaches build #19, `boot_completed=1` and the Huawei
  home launcher, with the signed module loaded and SELinux Enforcing. Its
  captured log again has no bad route spinlock, ION rwlock, CFI failure or
  panic, but still has the video ISR warning. This is short-run verification,
  not a repeat of r17's longer acceptance observation or stress certification.

Current installed candidate:

- `images/test/allen-boot-test-5.10.43-clang12-r19.img`
- `images/test/allen-modem_driver-test-5.10.43-clang12-r19.img`

Keep these paired. Retain accepted r17 and the stock boot/modem_driver
originals; if this candidate fails later, attribute the recovery pstore to
its build before restoring both r17 images (or both originals). Do not touch
the separate ramdisk or user-data partitions. Keep only the recent five
candidate pairs plus the best accepted baseline; processed diagnostics are
removed rather than committed.

At the r19 automated-test checkpoint camera preview awaited first-use consent. Physical touch,
cellular calls/data, Bluetooth, full sensor coverage, hardware video playback,
long-term stress and deep suspend remain outside this round's proven results.

The separate video ISR warning has also been narrowed down: its power-lock
callback takes saved IRQ flags by value. `spin_lock_irqsave()` changes only
that callee's local copy, while the ISR restores its unchanged zero flags.
Next change the lock callback to return or write back the real saved flags;
do not mask the warning or disable interrupts as a workaround.

## r20: Preserve Video Decoder IRQ Flags (2026-11-11)

Base: r19, commit `cade288e6`. The user reports physical front/rear camera
switching and still-image capture working on r19 after first-use consent.
This is user-observed preliminary camera coverage, not automated validation
of every mode, video recording, camera stress or later candidates. No user
photo or video is accessed for kernel diagnostics.

The attributed IRQ 113 warning is `vfmw_isr` returning with IRQs enabled.
Its power-lock callback takes flags by value, so `spin_lock_irqsave()` only
updates the callee's copy; the ISR then unlocks with its original zero flags.
Change the callback to accept `unsigned long *`, pass the ISR's flag address
and save through that pointer. Keep the unlock callback, power-off check,
lock, IRQ registration, device handling and dispatch logic unchanged.

Files: `vfmw_intf.h`, `vfmw_intf.c`, and `vcodec_vdec.c` in the existing
platform_source/vcodec/vdec implementation. Reference: Linux 5.10.43's
standard paired spin_lock_irqsave/spin_unlock_irqrestore contract and the
official ALN source call chain. No external driver code or hardcoded IRQ
state is introduced. These are internal C callback types; update both sides
together, retaining enforcing CFI.

Checks:

- Diff checks, Clang 12.0.7 ThinLTO compilation and paired staging pass.
  The companion module's 390 imports match with no missing exports or CRC
  mismatches; CMS, filesystem/metadata and boot-format verification pass.
- Paired flash reaches normal Android on build #20. Full boot/modem_driver
  hashes match the staged pair; the independent Magisk ramdisk is unchanged.
- A temporary Java MediaCodec/MediaExtractor probe explicitly selects
  `OMX.hisi.video.decoder.avc` and `OMX.hisi.video.decoder.hevc`, not a
  software fallback. Each decodes a synthetic 640x360, 30-fps, 90-frame clip
  to EOS with 90 nonempty, CPU-readable output buffers. The AVC output has
  padded height 368; this test does not claim surface presentation or exact
  pixel comparison. The attributed VDEC IRQ 113 count increases during the
  two decodes, confirming hardware interrupt activity.
- First available root log covers the previously observed warning window
  (starts at 13.54 seconds). Neither it nor the hardware-decoding log reports
  the ISR-enabled-interrupts warning, BUG, WARNING, CFI failure or panic.
  Taint remains 516 (S/W); earlier boot warnings are not fully captured.
- A temporary SensorManager probe registers accel/gyro/mag listeners at
  20 Hz using the shell package context. Each named hardware sensor delivers
  three timestamp-advancing HAL events. All listeners are unregistered when
  the probe exits; no sysfs settings or calibration data are changed.
  This fills the event-delivery gap from the r18 sysfs probe, not physical
  motion, precision/calibration or full sensor-mode certification.
- Runtime SELinux is Enforcing, module sig_enforce is Y and the matching
  signed modem module is loaded. CFI configuration is unchanged.
- A normal reboot again reaches build #20, `boot_completed=1` and the Huawei
  home launcher. Its captured log, starting at 13.52 seconds, again has no
  ISR IRQ-state warning, BUG, WARNING, CFI failure or panic. This remains
  short-run validation, not complete early-log or long-term stress coverage.

The tests use ignored host Android SDK platform 30 r03 / build-tools 30.0.3
(official dl.google.com/android/repository archives) with the local JDK,
and FFmpeg-generated clips. The SDK jars are retained as test tools; owned
temporary sources, bytecode, clips and full logs are removed. No APK,
persistent service, media-library entry or user-media access is introduced.

The retained r20 functional-reference pair is:

- `images/test/allen-boot-test-5.10.43-clang12-r20.img`
- `images/test/allen-modem_driver-test-5.10.43-clang12-r20.img`

Keep accepted r17 and the stock originals as paired rollback paths. L3 c1/c2
multi-level voting, Wi-Fi association/data, cellular service, Bluetooth,
deep suspend, long-term stability and high-resolution/surface video playback
remain unverified or unresolved. Do not guess the additional L3 vote fields.

## r23 Audio And Bluetooth Follow-Up (2026-11-11)

r23 reaches normal Android with the GPIO initialization repair; r20 remains
the named hardware-video/sensor reference. Neither of the following probes
establishes a functional pass, and neither leads to speculative driver or
SELinux changes:

- System `svc bluetooth enable` and its restore command exit 137. Attributed
  AndroidRuntime logs show a NullPointerException in AppOps runtime-access
  reporting through BluetoothAdapter, not a kernel panic. Bluetooth init
  activity occurs, but the manager reports STATE_ON timeout. The observed
  adapter and persistent setting remain OFF/0, their original state. No
  discovery or pairing is requested. A proper attributed app/UI test and
  service/HAL diagnosis are still needed before calling Bluetooth working
  or assigning this to a specific driver defect.
- A temporary AudioTrack probe requests one second of silent 48-kHz stereo
  PCM and per-track mute, without recording or changing system volume.
  getMinBufferSize returns -1 before track creation, under shell and a root
  diagnostic attempt. The probe reports that failure rather than substituting
  a guessed buffer size or claiming playback. Temporary device DEX files are
  removed after each attempt.
- Read-only diagnosis finds AudioPolicyManager=0x0 and only audio-dp in
  /proc/asound/cards. analog_less_machine repeatedly returns -EPROBE_DEFER
  during card registration; fw_hifi exists by the time of inspection, so
  initial pre-device-node firmware-open errors alone do not explain the gap.
- Compared with original running-stock configuration, the generated config
  omits SND_SOC_ASP_CODEC_V2C, SND_SOC_ASP_CODEC_AXI2TDM and
  SND_SOC_ASP_CODEC_MAD_V2. The child Kconfig defines them but is not sourced
  by codecs/Kconfig. Its asp_codec_codecless directory contains only Kconfig
  and Makefile, not the referenced asp_codec_v2/utils/control/path sources.
  The machine driver cannot bind an absent codec merely by changing a return
  value, reordering its probe, or enabling unresolved build rules.

Next obtain and compare a compatible codec implementation and its DT/DAI,
register and firmware contracts before restoring these configuration options.
GitHub indexed-code searches for asp_codec_v2.c / asp_codec_v2 return no
results; this does not prove that no compatible release exists. No external
audio implementation has been adopted. Audio, microphone/call paths and
Bluetooth remain open functionality gaps, alongside Wi-Fi association,
multi-level L3 voting and long-duration/deep-suspend testing.

## r24 Bluetooth Control And Hardware Encode (2026-11-11)

No kernel/config/DT change or flash is required for this follow-up. Installed
build remains #24. The missing-audio-source/platform audit is recorded in
`clang12-audio-source-audit.md`; no incompatible external codec is adopted.

### Correctly Attributed Bluetooth Probe

The earlier standalone `svc` failure is not a valid negative hardware test:
AppOps reporting lacks a normal bound application, and createPackageContext
from a system context inherits its AppOps package `android`. A first temporary
probe with only bound application fields still reports `Package android does
not belong to 2000`, confirming the second identity mismatch rather than a
driver diagnosis. Its observed state remains OFF.

Use a temporary app_process probe under UID 2000 with the installed
com.android.shell ApplicationInfo, an application ContextImpl with that same
AppOps package, and process-local ActivityThread application identity.
Validate package UID and OpPackageName before any adapter change. This uses
the existing shell identity, not another application's identity, an AppOps
policy override, SELinux permissive mode, an APK install or service spoofing.
Reference: AOSP Android 12.0.0_r1 ActivityThread.java currentPackageName /
currentOpPackageName and ContextImpl.java createAppContext semantics. These
references guide the temporary probe, not a kernel change.

The corrected probe returns successfully: Bluetooth reaches STATE_ON and
stays enabled for three seconds, then returns to its original STATE_OFF.
Persistent bluetooth_on remains 0. No discovery, pairing, nearby-device
enumeration, audio routing or peer data transfer is requested. Adapter
enable/disable is now a functional pass; these other Bluetooth paths remain
untested. Do not retain the earlier STATE_ON timeout as proof of a hardware
failure after a command was immediately followed by a failed restore path.

### Named Hardware Video Encoders

A temporary MediaCodec probe uses the same validated shell context and
explicitly selects OMX.hisi.video.encoder.avc / OMX.hisi.video.encoder.hevc.
Both are present as hardware-accelerated encoders; no software fallback is
allowed. Their advertised capabilities support 320x240 byte-buffer YUV420
semiplanar input (color format 21).

Each encodes thirty synthetic video-only frames, at configured 30 fps and
256 kbit/s, to thirty distinct matching output presentation timestamps and
output EOS. Input uses generated moving luma and neutral chroma, never camera,
microphone or user media. No media-library file is created. This checks
configured encoder processing and output completion, not bitrate accuracy,
bitstream decode/pixel fidelity, surfaces, camera recording or microphone
capture.

VENC registers the generic IRQ name DT_device only while its driver resources
are active; an idle grep for `venc` is not a reliable counter query. Host
snapshots during the configured codecs attribute IRQ 115 (GICv3 hardware IRQ
229) to DT_device, with sixty interrupt increments across the two encodes.
Both codecs are released, and temporary device DEX files/directories are
removed after the tests.

The post-test full root log retains the same four known early warnings and
no BUG, CFI failure, panic or enabled-IRQ warning. Android remains booted on
#24 with the matching modem module, sig_enforce=Y and SELinux Enforcing;
taint remains 516. Wi-Fi and Bluetooth settings remain OFF. These short
functional checks do not replace deep suspend, peer-radio transfer,
long-duration stress or full OEM hardware certification.

## r25 Wi-Fi Scan Request And Calibration Access Follow-Up

This diagnostic runs on #25 before the metadata-only r26 flash. Wi-Fi is
initially disabled with scan-always-available set; do not change the latter
policy, location setting, saved networks or calibration data for this test.

- A shell cmd wifi start-scan exits zero both with Wi-Fi disabled and after
  a temporary enable. Repeated list-scan-results queries for eighteen seconds
  return No scan results in both cases. The enabled state and two wlan
  interfaces appear, but command success alone is not a hardware scan pass.
- The enabled interval's kernel log shows a scheduled PNO scan starting,
  but no request-specific foreground wal_cfg80211_scan callback is found.
  This does not establish whether the shell request was forwarded, policy
  denied it, the vendor service took another path or radio results were absent.
  A scanner dump taken in the disabled state shows empty results and default
  state machines; it is not evidence of the enabled-state scan outcome.
- Kernel NVE reads from the Wi-Fi HAL context log open errors with fd
  fffffff3 (-EACCES). Source hisi_conn_nve_read opens
  /dev/block/by-name/conn_calidata through ksys_open, with the caller's
  credentials; set_fs(KERNEL_DS) does not supply kernel-thread credentials.
  The source's generic NVRAM/customization warnings are not individually
  attributed as fatal or treated as the established scan-failure cause.
- Read-only inspection after the r26 reboot finds that same partition at
  /dev/block/sdd5, mode 0600 root/root and type certification_device. The
  Wi-Fi HAL and wificond run as UID/GID 1010. No matching Wi-Fi/conn/NV AVC
  is found in the r25 enabled test interval. These facts warrant tracing
  credentials/capabilities and kernel-owned calibration access; they do not
  prove a particular SELinux rule is missing or justify chmod, blanket
  permissions, caller-credential overrides or permissive mode.
- Wi-Fi is restored to its original OFF/0 state; Bluetooth remains OFF/0
  and location remains disabled. No SSID/BSSID list is retained or printed,
  no network is added/explicitly joined and no calibration partition is read
  for content, written or flashed. No BUG, CFI failure or panic appears in
  the captured test interval. Temporary probes and diagnostic logs are removed
  after recording the durable evidence.

Next use request/service-side evidence to distinguish scan API/policy behavior
from driver delivery, and audit the calibration read initialization context.
Wi-Fi association/data transfer remains unverified. This does not replace
the missing compatible audio codec implementation, Bluetooth peer tests or
the separate multi-level L3/PRMEM lifecycle work.

## r26: Foreground Native Wi-Fi Scan And Association

The continued shell-context probe validates UID 2000 and OpPackageName
com.android.shell, but startScan returns false with Wi-Fi both OFF and ON.
No foreground single-scan request appears in the corresponding scanner dump.
The existing CHANGE_WIFI_STATE/WIFI_SCAN AppOps report allow; location is OFF,
idle mode is false and scanning availability is enabled. The installed vendor
service's exact rejection gate is not attributed. AOSP Android 12.0.0_r1
WifiServiceImpl / WifiShellCommand show why a zero shell-command exit code
does not prove scan acceptance; they are interface references, not proof that
the installed implementation follows every same gate.

The first native-settings attempt also leaves the display asleep and the
scanner showing only PNO requests. Repeat with the device explicitly awakened
and the native Wi-Fi settings activity in the foreground, temporarily enabling
Wi-Fi while leaving location and scan-always-available unchanged:

- WifiScanningService records settings-attributed single-scan requests and
  fresh result batches (17, 22, 24 and 15 results), not just an old cache.
- wificond reaches wal_cfg80211_scan; matching hmac scan-completion events
  report status 0. Foreground scan delivery and result return pass on #26.
- The existing saved-network policy automatically associates during this
  enabled interval. The primary client reports supplicant COMPLETED with
  populated DHCP configuration. No password is supplied and no network is
  added or explicitly selected. Association/DHCP pass; explicit external
  connectivity, DNS, throughput and sustained transfer remain untested.
- Calibration open -EACCES messages still occur during successful scanning
  and association, including HAL and supplicant callers. This rules out
  treating those errors alone as proof of scan failure; calibration fidelity
  and the correct kernel-owned access lifecycle still need separate work.
- No BUG, CFI failure or panic appears. Wi-Fi returns to its original OFF/0,
  location remains OFF/0 and no SELinux, AppOps, block-node permission or
  calibration-content changes are made. Nearby network identifiers are not
  included in this record; diagnostic dumps are removed after analysis.

This resolves the earlier inconclusive foreground scan/association checks,
not every radio function or the standalone shell probe's vendor-service gate.

## r28: Codec Registration, Silent Playback And Runtime PM

The approved codec-only integration uses the Linux 5.10.43 Charlotte source
and DT-boundary adaptation documented in clang12-audio-source-audit.md.
The local PCM/DSP implementation, device tree, firmware, calibration data and
independent Magisk ramdisk are not changed. Keep the r27 boot/modem_driver
pair as the immediate pre-audio rollback; do not mix pair versions.

### Build And Paired Boot

- Clang 12.0.7 ThinLTO with enforcing CFI builds the codec as built-in code.
  Final ELF initcalls place CLT data setup before path registration, then the
  common component driver and late machine driver. Boot round-trip and modem
  signature/certificate/filesystem checks pass; all 390 versioned modem imports
  have matching exports and CRCs. No debug stubs or unrelated audio import is
  used to bypass compilation.
- Preflight confirms the recorded serial and bootloader ALN-AL00 identity
  and unlocked state. This bootloader does not support getvar product/all;
  the documented OEM identity queries supply the supported check. Both boot
  and modem_driver flashes succeed. The first boot and a normal reboot reach
  sys.boot_completed=1 on #28, normal mode, with Magisk root available.
- The normal analog-less-machine-card is card 0 with its eight PCM devices;
  audio-dp is card 1. AudioPolicyManager is populated on both boots instead of
  the r27 null manager. Audioserver is running. Device-side readback hashes
  over each retained image's exact length match the r28 boot/modem_driver
  pair, and the entire ramdisk hash matches its pre-flash baseline.
- Full early logs from both boots and the captured playback interval contain
  no kernel WARNING, BUG, CFI failure or panic. Taint remains 4, module
  sig_enforce remains Y, and SELinux remains Enforcing. Early fw_hifi open
  retries are followed by successful DSP loading/verification and the loaded
  marker; those early retries alone are not proof of DSP failure.

### Playback And Release

A temporary AudioTrack probe validates UID 2000 and OpPackageName
com.android.shell, streams two seconds of generated zero-valued 48 kHz stereo
16-bit PCM and mutes only its own track. It does not change global volume,
request microphone capture, play user media or create a media-library file.
The original smoke check and the post-reboot lifecycle check each consume
all 96,000 written frames, report the built-in speaker route and zero underruns.
Track stop/release completes and the temporary device DEX is removed.

The lifecycle check also observes the actual card0/pcm0p substream RUNNING
with hardware/application pointers while the probe plays, then closed after
release. Codec runtime_status changes from suspended to active and returns
to suspended; runtime control remains auto with the existing 200 ms delay.
The log pairs both playback PRE_PMU events with POST_PMD events, followed by
codec and ASP-subsystem clock disable. Audioserver retains the same PID.
This verifies short playback startup/shutdown and codec runtime autosuspend,
not system suspend, deep sleep, long-duration stability or audible output.

The user subsequently reports normal playback from the system sound settings
on r28. Record this separately as a user-observed basic audible playback pass,
not an instrumented audio test. No waveform, level, frequency response or
distortion measurement is taken, and no additional kernel change, build or
flash is needed for this confirmation.

### Remaining Gaps

The two boot diagnostics, mux MDM_5G_UP_L_MUX has no paths and mux
SIDETONE_PAG_DIN_MUX has no paths, originate in the pinned source's route
definitions. CLT connects the right MDM 5G mux for the ultrasound path, but
does not connect the left mux or the sidetone mux. These are ASoC diagnostics,
not kernel WARN splats, and coexist with the tested normal playback path.
Do not delete widgets, suppress messages or invent routes to clear them.
Their intended platform routes and firmware control contract need separate
evidence before a sidetone/call fix.

Registration, logical speaker routing and silent frame consumption alone do
not prove audible output; the user's system-settings check now confirms basic
audible playback. Measured fidelity, microphone capture, calls, sidetone,
Bluetooth audio, USB/DP audio, video recording with audio and system/deep
suspend remain unverified. Next isolate one of these paths with its own
meaningful check before expanding the codec import or attributing remaining
audio-path errors.
