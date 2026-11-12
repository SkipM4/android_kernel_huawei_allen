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

### Display Wake And USB-Connected Suspend Boundary

A separate r28 follow-up checks the power baseline before attempting deeper
sleep. The firmware exposes freeze/mem, with deep selected in mem_sleep.
Kernel suspend_stats has zero successes and failures; the root-accessible
suspend_control service also reports zero suspend attempts and total suspend
time. PowerManagerService is Asleep with no held wake-lock or display suspend
blocker, and the recorded native suspend-service locks are inactive.

The kernel's per-source active_time_ms nevertheless shows usb_wake_lock and
charge_wakelock continuously active; their active time increases across the
screen-off interval. A transient sensorhub wakeup is not retained as a
continuous blocker. Interpret active_time_ms using
`drivers/base/power/wakeup_stats.c`: it is zero for an inactive source, unlike
active_count, which counts past activations. No debugfs mount or global
tracing change is needed.

This matches the local USB/charging lifecycle: dwc3-chip.c holds its wakeup
source on device connection unless chip_usb_bc12.c sleep_allowed permits the
charger type, and releases it in stop_device. The charging manager acquires
its source in charge_start_charging and releases it in charge_stop_charging.
These observations do not establish a wake-lock leak after cable removal.
Do not force-unlock sources, change charger policy, disable SELinux or write
directly to power/state to manufacture a suspend pass with active USB.

The recoverable check wakes the display, requests sleep, leaves it Asleep for
30 seconds without issuing ADB requests, then wakes it again. A post-wake
silent AudioTrack check consumes all 96,000 frames, reports the built-in
speaker route and zero underruns, and observes card0/pcm0p RUNNING. After
release the PCM closes and the codec returns to runtime suspended, with both
playback power-down events and codec/ASP clock disable in the log. Device
boot identity and audioserver PID do not change; the test interval has no
kernel WARNING, BUG, CFI failure or panic. The original Asleep state is
restored and the temporary probe is removed. No global volume, media,
microphone, radio, power policy, kernel configuration or partition is changed.

Suspend success/failure counters remain unchanged at zero during this check;
there is no demonstrated system-suspend entry or codec system-sleep callback.
This is a display-wake/audio and runtime-PM pass, not deep-suspend certification.
Next unplug USB without attaching a charger, leave the screen off for about
60 seconds, reconnect, and compare suspend counters, suspend-service statistics,
last_resume_reason/last_suspend_time and the minimal kernel suspend/resume
log. Confirm actual suspend entry and codec callbacks before attributing any
resume failure or modifying their implementation. r28 remains installed and
the r27 paired rollback remains available.

### Unplugged Deep-Suspend Result And UFS Resume Warning

The user disconnects USB, leaves the display asleep, then reconnects it.
The same r28 boot identity remains and Android is still in normal mode with
sys.boot_completed=1. Kernel and suspend-service counters advance from zero
to one successful suspend, with no suspend/resume stage failure. The kernel
records PM: suspend entry (deep) and suspend exit, and the reported sleep time
is 71 seconds. The last wake reason is the Type-C port IRQ triggered by cable
reconnection. This is actual deep system suspend, not just display sleep.

The codec logs suspend begin/end with have_dapm=0 and is_ulsr=0, followed by
resume from asp pd. After reconnection it is runtime suspended and the normal
playback PCM is closed. These observations exercise idle codec system-sleep
callbacks, not post-resume streaming, an active-DAPM/ULSR case, microphone or
call recovery.

The cycle is not a clean suspend pass: UFS resume logs Unbalanced enable for
IRQ 54 and a kernel WARN at enable_irq, raising taint from 4 to 516. The
minimal stack is hufs_enable_all_irq -> hufs_main_resume -> hufs_resume ->
ufshcd_resume -> ufshcd_system_resume. The current IRQ action identifies the
UFS interrupt hub; it is not the Type-C wake IRQ or an audio IRQ. UFS returns
success and the system does not reboot, but do not hide or dismiss the warning
because the framework counts the suspend as successful. The r29 isolated
initial-state repair is described next.

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

## r29: Initialize UFS Hub IRQ Ownership After Registration

Change only drivers/scsi/ufs/ufshcd.c: after successful registration of the
CONFIG_SCSI_UFS_INTR_HUB IRQ, set the existing hba->is_irq_enabled to true.
The platform obtains both hba->irq and hba->ufs_intr_hub_irq from DT IRQ index
zero; the registered shared IRQ is automatically enabled. The non-hub branch
already records this state, but the hub branch leaves the zero-initialized
flag false. No new state field, platform IRQ number or core IRQ override is
needed.

On the first observed system-sleep cycle, the platform suspend disables the
hub IRQ, but the core ufshcd_disable_irq skips its own disable because the
flag is false. Core resume then enables it and marks it true; the platform's
subsequent hufs_enable_all_irq enables it again at depth zero and warns.
Record the correct initial state instead of removing the platform pair:
its nested disable keeps the hub IRQ masked until the platform has restored
the controller. Keep the existing UTR-core IRQ operations, runtime-PM,
error/reset paths, regulator/clock/reset ordering and core WARN checks.

Reference: official ALN 5.10.43 IRQ registration and PM call chains, and Linux
stable v5.10.43 drivers/scsi/ufs/ufshcd.c, which sets is_irq_enabled after
successful devm_request_irq. The upstream source is inspected at
https://github.com/gregkh/linux/blob/v5.10.43/drivers/scsi/ufs/ufshcd.c.
This applies the standard state contract to the local hub branch; it is not
an external UFS driver import or certification of all platform PM behavior.

Clang 12.0.7 ThinLTO/enforcing-CFI compilation and diff checks pass. Boot
round-trip, unchanged OEM DT/header/firmware-stub and partition-bound checks
pass. The signed modem companion has all 390 versioned imports available
with matching CRCs; certificate/signature, ext4 and metadata readback checks
pass. No configuration, ABI structure or audio-driver change is made.

Bootloader preflight reconfirms the recorded serial, ALN-AL00 identity and
unlocked state using the supported OEM queries; product/all remain unsupported.
Both boot and modem_driver flashes succeed and #29 reaches normal Android
with sys.boot_completed=1. Device-side readback over each image's exact length
matches the r29 pair; the full independent ramdisk hash matches the pre-flash
r28 baseline.

A temporary freestanding ARM64 probe uses the verified ARM64 UAPI O_DIRECT
flag, not the differing asm-generic value or a buffered fallback. It creates
only its own exclusive diagnostic file, writes 512 aligned 4096-byte blocks,
fsyncs, reads them through the same O_DIRECT descriptor and compares every
byte with the generated offset-dependent pattern. The check passes; both
hub and UTR-core IRQ counters increase. The executable and owned data are
removed. The same boot identity remains throughout the storage check.

The normal codec-backed card and populated AudioPolicyManager remain present,
and the idle PCM is closed with the codec runtime suspended. Full boot/storage
logs contain no kernel WARNING, BUG, CFI failure, panic or unbalanced IRQ
enable. Taint is 4, sig_enforce is Y and SELinux is Enforcing. These initial
checks do not exercise the reported system-sleep warning: this r29 boot still
has zero suspend successes/failures.

### Unplugged Recheck And Post-Resume Function

The user disconnects USB with the screen asleep and then reconnects. The same
#29 boot identity remains, normal Android is booted and audioserver retains
its PID. Kernel and suspend-service statistics agree on five attempts: three
successful deep suspends and two freezing-stage aborts. All device-suspend
and resume failure counters remain zero. The service reports approximately
99 seconds of cumulative sleep; the last wake reason is the Type-C IRQ on
cable reconnection. The other two completed wakeups are attributed to the
SoC RTC, with a separate configuration diagnostic described next.

The first successful cycle and both subsequent device-sleep cycles log UFS
platform suspend/resume returning zero and idle codec suspend begin/end,
have_dapm=0/is_ulsr=0 and resume from asp pd. No unbalanced enable appears.
The complete captured boot/suspend/resume logs contain no kernel WARNING,
BUG, CFI failure or panic; taint remains 4. This resolves the observed UFS
hub initial-state warning, including the decisive first resume, not just a
later cycle that could have corrected the old flag as a side effect.

The two failed attempts explicitly log Freezing of tasks aborted, with
eventpoll as the last active wakeup source. kernel/power/process.c distinguishes
this pm_wakeup_pending path from tasks refusing to freeze: it aborts promptly
before device suspend. Do not attribute those counters to a failed UFS or
codec callback, lengthen freezer timeouts or disable wakeups to clear them.

After actual system resume, the native probe repeats its 512 aligned-block
O_DIRECT write/fsync/read/byte-compare check successfully. UFS hub and core
IRQ counters increase. A shell-identity-validated AudioTrack probe consumes
all 96,000 generated silent 48 kHz stereo frames, reports the logical built-in
speaker route and zero underruns, and observes the normal PCM RUNNING with
the codec runtime active. After stop/release the PCM closes and the codec
returns to runtime suspended. Both playback power-down events and codec/ASP
clock disable are present. No global volume, media-library, microphone,
radio, power-policy, SELinux, kernel configuration or partition change is made.
The probes and their exclusive owned files are removed.

The same device/audio-service identity remains throughout the post-resume
checks. Full logs still have no kernel WARN/BUG/CFI failure/panic or unbalanced
IRQ enable; taint stays 4, sig_enforce is Y and SELinux remains Enforcing.
No new kernel build or flash is needed for this confirmation. Retain r28 as
the immediate paired rollback. These are short idle deep-sleep and resume
checks; active-DAPM/ULSR, calls/microphone, audible post-resume fidelity,
long-duration sleep/idle-power parity and every wakeup source remain outside
the proven scope.

### Separate RTC Wake-IRQ Contract Gap

Two RTC wakeups log Resume caused by misconfigured IRQ 228 vendor-soc-rtc.
Read-only IRQ attribution confirms the SoC RTC action; the RTC device's wakeup
policy is enabled. The message is emitted by kernel/irq/chip.c irq_may_run
when a no-suspend IRQ is also configured as a wakeup source but is not armed
through the normal wake-IRQ path. It is not a kernel WARN splat and does not
change taint, but it identifies a separate driver contract mismatch.

platform_source/basicplatform/drivers/rtc/rtc_pmic_spmi.c request_rtc_irq
registers the SoC IRQ with IRQF_NO_SUSPEND, then rtc_suspend enables wake on
that same IRQ and rtc_resume disables wake.
`Documentation/power/suspend-and-interrupts.rst` explicitly states that
IRQF_NO_SUSPEND and enable_irq_wake must not be used together for the same
device. The PMU IRQ
has a separate registration and purpose; do not change both just because
they share this helper.

Next audit the SoC RTC handler's late/noirq dependencies, then adapt only its
registration to the documented wake-IRQ contract while preserving alarm
programming and the existing wake-enable pair. Do not silence the generic
diagnostic or disable RTC alarms. No RTC source, IRQ flag or alarm is modified
in this r29 validation round.

## r30: Use The Normal SoC RTC Wake-IRQ Path

Change only platform_source/basicplatform/drivers/rtc/rtc_pmic_spmi.c:
request the SoC RTC IRQ with flags zero instead of IRQF_NO_SUSPEND. Keep the
PMU RTC registration, the DT-derived level trigger, SoC alarm register
programming, device wakeup policy and the existing rtc_suspend/rtc_resume
enable_irq_wake/disable_irq_wake pair unchanged. No clock, RTC time, secure
offset/calibration, firmware, device tree, global alarm or power-off alarm
setting is changed.

The call-chain audit finds no dependency on running the SoC handler during
the late/noirq phase. Its MMIO is mapped in rtc_init_cfg for the device's
lifetime; RTC PM does not unmap it or gate its clock. soc_rtc_interrupt only
reads/acknowledges the alarm status and calls rtc_update_irq. That core call
queues irqwork to process timer events; this is not a periodic clock tick
required to make the suspend/resume sequence progress. soc_rtc_read_time
independently reads the counter. Delaying the handler until normal resume
therefore preserves the alarm notification rather than deleting it.

Reference: the local official 5.10.43 RTC handler/PM and RTC core workqueue
contracts, kernel/irq/pm.c and kernel/irq/chip.c, and
`Documentation/power/suspend-and-interrupts.rst`. The PM core must be able to
arm the SoC IRQ as a wake source and defer its ordinary handler; NO_SUSPEND
bypasses that path and causes the observed misconfigured-IRQ diagnostic.
Do not replace the flag with IRQF_COND_SUSPEND: this IRQ is not a demonstrated
shared interrupt with a separate no-suspend user. The PMU interrupt has a
different handler/purpose and is outside this isolated repair.

Before the change, read-only checks confirm that RTC time advances and device
wakeup policy is enabled on #29. Current boot/modem_driver readback matches
the retained r29 pair, which is the immediate rollback.

Clang 12.0.7 ThinLTO/enforcing-CFI compilation and diff checks pass. Boot
round-trip verifies the kernel, unchanged firmware stub/OEM DT/header fields
and partition bounds. All 390 signed modem imports are available with matching
CRCs; certificate/signature, ext4 and module metadata readback pass. No new
API, configuration or ABI structure is introduced by the flag correction.

Supported bootloader preflight reconfirms the recorded serial, ALN-AL00
identity and unlocked state; product/all queries are unsupported as before.
Paired boot/modem_driver flash succeeds and #30 reaches normal Android with
sys.boot_completed=1. Device-side readback matches each retained r30 image
over its exact length; the independent ramdisk still matches the pre-flash
r29 baseline. The signed modem module is loaded with sig_enforce=Y and
SELinux Enforcing.

Read-only since_epoch samples show normal RTC time progression after boot.
The RTC's wakeup policy remains enabled. Both distinct actions are registered:
vendor-soc-rtc on its unchanged DT-derived level IRQ and vendor-pmu-rtc on
the PMU IRQ. Full early logs contain no kernel WARNING, BUG, CFI failure,
panic or unbalanced enable, and taint stays 4. No RTC time, calibration,
alarm setting, global trace or SELinux policy is changed for these checks.

The initial post-flash check has zero RTC IRQ counts and zero suspend
successes/failures. An actual RTC-attributed deep wake therefore remains
pending; USB-only resume and the absence of an RTC event are not proof that
RTC wakeup works or that its diagnostic is fixed. Next disconnect USB without
attaching a charger, leave the screen asleep for about two minutes and
reconnect. Inspect every completed cycle's wake reason and RTC handler/core
notification, not just the final Type-C wake reason. Require an actual RTC
event with preserved alarm delivery and no misconfigured-IRQ diagnostic
before marking this path passed. Preserve r29 for paired rollback if wakeup,
alarm handling or storage/audio after resume regresses.

### Actual RTC Wake And Post-Resume Playback

After the user disconnects USB with the display asleep and reconnects, the
same #30 boot identity remains in normal Android with sys.boot_completed=1.
Kernel and suspend-service counters agree on four successful deep suspends,
approximately 226 seconds of cumulative sleep and zero failures at every
freeze, device-suspend and resume stage. This includes the first real resume
after the flag change, not merely later cycles in an already-resumed system.

Two cycles explicitly log pm_system_irq_wakeup for IRQ 228 vendor-soc-rtc,
followed by soc_rtc_interrupt and the ordinary Resume caused by IRQ 228
message. The SoC RTC IRQ count advances from zero to two; the distinct PMU
RTC IRQ count stays zero. The unchanged handler acknowledges status and
calls rtc_update_irq; the subsequent suspend programs the next RTC alarm.
There is no misconfigured-IRQ diagnostic, missing handler invocation or
repeating IRQ storm in the captured interval. RTC wakeup policy stays enabled.
No direct RTC alarm/time write, replacement of a user's alarm or synthetic
wakeup is needed for this confirmation.

The final wake is the Type-C port IRQ on reconnection. The remaining cycle is
unknown in the generic suspend-service wake list; the OEM low-power log
identifies IPC_AO_IOM3_ACPU_MBX1 from SENSORHUB to ACPU. Do not count that
cycle as an additional RTC wake or infer a resume failure from the missing
generic attribution. All four cycles have successful UFS platform callbacks
and idle codec suspend/resume, have_dapm=0/is_ulsr=0 and resume from asp pd.
The UFS unbalanced-enable warning remains absent.

A shell-UID/package-validated AudioTrack check after actual resume streams
generated silent 48 kHz stereo 16-bit PCM, with a small silence tail to avoid
emptying the streaming buffer while observing the target. All 96,000 target
frames are consumed with zero underruns and the built-in speaker route. PCM
is RUNNING and codec runtime active during playback. After track release and
audio-output standby, PCM closes, codec/ASP clocks disable and runtime status
returns to suspended with the original auto policy and 200 ms delay. Device,
audioserver and audio HAL identities remain unchanged.

The temporary helper's initial attribution check rejects an inherited android
context before opening an AudioTrack. Inspecting the installed framework
confirms that ContextImpl.getOpPackageName uses AttributionSource, not the
legacy mOpPackageName field. Correcting only the helper's in-process shell
attribution and explicitly exiting its helper threads produces the completed
check; no service identity, permission, SELinux policy or kernel change is
used to bypass attribution. The temporary helpers and complete logs are
removed after recording these results.

The complete retained-for-analysis boot/resume/playback logs contain no kernel
WARNING, BUG, CFI failure, panic, unbalanced enable or misconfigured IRQ;
taint remains 4, module sig_enforce is Y and SELinux is Enforcing. No new
kernel build, flash, ramdisk, DT, calibration or global volume change is
made. The r29 post-resume O_DIRECT test is not repeated or relabeled as an
r30 data-integrity test without a new storage failure.

This passes the observed SoC RTC wake-IRQ contract repair and short idle deep
resume, including post-resume playback/release. It does not certify every
userspace alarm delivery, power-off PMU alarms, long-duration sleep/idle power,
active-DAPM/ULSR suspend, microphone/calls or audible post-resume fidelity.
Keep the tested r29 pair as immediate rollback; isolate one remaining path
before changing another subsystem or expanding the codec import.

### Basic Microphone Capture And Release

The next isolated r30 check targets AUDIO_SOURCE_MIC, not telephony, remote
submix, speech recognition or playback capture. Read-only preflight confirms
the bottom built-in input's published 48 kHz/mono capability, the shell's
existing RECORD_AUDIO and RECORD_BACKGROUND_AUDIO grants, MODE_NORMAL,
unchanged disabled sensor privacy, and microphone mute false for switch,
API, restrictions and system sources. No other AudioRecord client is active.
No permission grant, app-op override, mute/volume change or call is needed.

A temporary UID 2000/com.android.shell-attributed AudioRecord marks its
capture privacy-sensitive and reads two seconds of 48 kHz mono S16 PCM into
a reused in-memory buffer. It produces only aggregate sample count, nonzero
count, peak and RMS; raw samples are not saved, transmitted or interpreted as
speech, and the buffer is cleared on release. All 96,000 requested samples
arrive without negative read results; the data contains nonzero samples and
does not reach the S16 clipping endpoints during this interval.

AudioPolicy reports the active MIC client at UID 2000 and input address
bottom; AudioFlinger identifies the same process/session, a 48 kHz mono
built-in-microphone stream and an active track with Sil=n. AudioService
records start/stop with the shell package and not silenced. RECORD_AUDIO
app-op usage is allow for the approximately two-second capture, not a forced
permission bypass. These checks rule out an all-zero permission-silenced
client as the explanation for this short result.

The normal card0/pcm0c substream is RUNNING with advancing hardware/application
pointers. Its actual hardware parameters are 48 kHz S16_LE, six interleaved
channels, 960-frame periods and a 7680-frame buffer. The HAL supplies one
channel to the caller. Preserve that existing PCM/DSP contract; a mono API
request does not imply that the kernel should be changed to capture a single
hardware channel, nor does this check identify every physical microphone.

After stop/release, the capture PCM and hw_params close, the policy input and
active record client disappear, and codec/ASP clock-disable events are present.
Runtime status returns to suspended with the original auto/200 ms policy.
The same boot identity, audioserver and audio HAL remain; MODE_NORMAL,
microphone mute and sensor privacy are unchanged. Complete boot/capture logs
contain no kernel WARNING, BUG, CFI failure, panic, unbalanced enable or
misconfigured IRQ; taint stays 4, module sig_enforce=Y and SELinux Enforcing.
No kernel change, build, flash, firmware/DT/calibration change or audio file is
introduced. The temporary probe and complete logs are removed after analysis.

This passes basic unsilenced MIC streaming and capture resource release on
the resumed r30 system. Ambient nonzero PCM is not a known acoustic stimulus
and does not certify speech intelligibility, gain, noise, distortion, each
physical microphone, camera/back-mic selection, voice-call/sidetone, duplex
AEC or active-audio system sleep. The early AudioFlinger record-thread
snapshot has no usable timestamp statistics and reports timestamp errors.
That observation prompts one separately scoped two-second timestamp check,
not an unrelated repetition of passed playback or deep-sleep tests.

During that capture, AudioRecord.getTimestamp with TIMEBASE_MONOTONIC first
returns unavailable twice, then succeeds nineteen times. Frame positions
advance from 7680 to 96000, corresponding times advance, and neither value
regresses. All requested samples arrive and capture again closes/autosuspends
without an audio-service restart. This validates short application timestamp
availability/progression after startup, not precision or its hardware origin.

The mid-capture AudioFlinger snapshot still has zero valid HAL timestamp
statistics and accumulating timestamp errors. A working application timestamp
may use a server-side position/time when hardware capture positioning is
unavailable; it does not resolve this separate HAL-side gap. Sustained capture
clock accuracy, hardware capture latency and audio/video sync remain untested.
Do not infer a codec/PCM timestamp root cause or add a fabricated timestamp
implementation. Next audit the actual HAL capture-position/ALSA boundary
before changing a driver; acoustic and per-microphone route checks remain
separate from that audit.

The subsequent read-only boundary audit finds that the stock primary HAL's
normal input initialization leaves get_capture_position unimplemented. The
loaded V7_0 HIDL implementation checks that slot and returns NOT_SUPPORTED
without querying ALSA when it is null. This attributes the observed HAL-side
capability gap to unchanged stock userspace, not a demonstrated kernel ioctl
failure. The standard AudioFlinger server timestamp/error paths explain how
application timestamps can progress while HAL timestamp statistics have no
valid samples. See the HAL boundary section of clang12-audio-source-audit.md
for the binary/source evidence and limits. No new microphone recording,
timing probe, driver change, build or flash is needed for this attribution.
Physical acoustic response, individual microphone routing and precise capture
timing remain outside the completed checks.

### User-Observed System Recording

Following the request for a local system-recorder check, the user reports
normal recording on the installed r30 candidate. Record this separately as a
basic user-observed recording pass, not merely nonzero synthetic/probe PCM.
No recording content is requested, received or analyzed by the agent, and no
additional sampling, build, flash or device-setting change is made to record
the result. It does not establish measured fidelity, exact recorder routing,
each physical microphone, call/sidetone, hardware capture timing or audio/video
sync. The previously identified stock HAL timestamp capability gap remains.

### Remaining r30 Runtime Diagnostics And Test Coverage

A subsequent read-only runtime audit finds recurring ordinary warning/error
messages; the logs are not entirely clean. The retained kernel ring covers
approximately 32 minutes of later runtime, not early boot. It contains no
kernel WARNING splat, BUG, CFI failure, panic, unbalanced-enable or
misconfigured-IRQ marker. Recent logcat contains no fatal-exception, native
crash or ANR marker. The same r30 boot and audio services remain active,
taint is 4, and the four successful deep suspends still have zero recorded
freeze/suspend/resume failures. These are bounded observations, not complete
startup or long-duration stability certification.

- Battery bci_get_get_ocv_by_cap reports zero OCV and a failed voltage lookup;
  bci_get_capacity_map_by_basp also reports failure repeatedly. Battery status,
  capacity, voltage and temperature properties remain readable. The capacity
  mapping failure does not establish that all battery sensing is broken.
- Thermal-daemon repeatedly rejects shelltemp results 0 and 5 and reuses the
  previous temperature. Some physical thermal zones remain readable. Audit
  input mapping, virtual sensors and OEM units before interpreting individual
  zone values or performing heavy load/charging stress; no overheat or root
  cause is established by this snapshot.
- CPU migration threads repeatedly report IRQ267 set affinity failed(-22).
  The IRQ is attributed to PCI-MSI PCIe PME/aerdrv, not RTC, UFS or Wi-Fi.
  Requested affinity spans the online CPUs, but effective affinity is empty
  and the sampled interrupt counts are zero. Audit the MSI domain/parent
  affinity contract; functional impact remains unconfirmed.
- RtgSchedIpcFile cannot open a per-process /proc/<pid>/rtg interface, which
  is absent in the root snapshot. CONFIG_SCHED_RTG and CONFIG_FFRT_RTG are
  enabled, while CONFIG_SCHED_CGROUP_RTG is not. This is an OEM interface gap,
  not evidence that all RTG scheduling support is disabled.
- Screen-off gesture lookup failures, property/access denials and OEM
  statistics-interface errors remain. One AudioFlinger record-read latency
  diagnostic is present despite user-confirmed normal basic recording;
  sustained recording latency/dropouts remain untested. Wi-Fi diagnostic
  upload errors explicitly refer to an inactive device and do not establish
  failure of the previously passing foreground scan/association path.

Earlier L3 c1/c2 multilevel-voting, calibration-access and inherited DAPM MUX
diagnostics remain unresolved according to their separate audit records.
Early messages have rolled out of this ring, so this runtime check cannot
establish their disappearance. The stock HAL capture-position capability
gap also remains; no fabricated kernel timestamp or security bypass is added.

Prioritize practical cellular calls/data, real Wi-Fi internet/transfer and
Bluetooth pairing/headset audio, then charging/battery behavior and overnight
unplugged idle. Audit battery/thermal errors before heavy stress. Current-r30
camera video/audio sync, sustained recording/playback, active-audio sleep,
individual microphones and long-duration storage/load remain beyond short
checks. GNSS, NFC, biometric unlock, proximity/auto brightness, vibration and
USB accessories lack comprehensive recorded validation; untested does not
mean broken. Earlier-version passes retain their original scope.

This audit changes documentation only: no driver, build, flash, permission,
SELinux, firmware, calibration, ramdisk or user-recording change is made.
Raw diagnostic logs are removed after retaining these conclusions.

## r31: Respect The Disabled Optional Battery OCV Provider

### Cause And Change

The r30 battery errors are traced to an optional-provider contract mismatch,
not demonstrated corruption of a battery curve or missing sensor samples.
The original retained running-device DT and current live DT both contain
huawei,battery_ocv with status=disabled and no ocv_table property. The linked
driver is present, but no platform device is instantiated or bound. Its
global device pointer therefore remains null, and battery_ocv_get_ocv_by_cap
returns -ENODEV before reading a table. BCI nevertheless requests the zero
capacity OCV on every monitoring cycle, reports two errors and returns the
unchanged current capacity. The r30 baseline reproduces this repeatedly while
Battery and battery_gauge expose real measurements.

Only platform_source/basicplatform/drivers/power/platform/bci_battery.c changes.
During existing DT parsing, a private BASP OCV flag uses the build option and
standard OF compatible-node availability; the node reference is released.
Disabled/absent providers skip the optional capacity remapping and OCV-based
zero/one-percent threshold updates. Existing gauge-derived capacity, DT
low-voltage thresholds, debounce, capacity checks and charging control remain
unchanged. Enabled providers still enter the original lookup path, including
its error reporting if probing or data lookup fails. The flag reflects DT
availability, not transient probe success, so an enabled-but-failing driver
is not silently treated as intentionally disabled.

No provider node is enabled, no replacement OCV table is invented, and no
firmware, battery calibration, permissions, SELinux policy or independent
Magisk ramdisk is modified. The scope is the unsupported consumer request,
not implementing a new aging curve or changing the underlying coulomb driver.

### Validation And Limits

Source diff/whitespace review and the existing Clang 12 build pass. CFI,
forced module signing and SELinux configuration remain enabled. The signed
matching modem module has no missing versioned imports or CRC mismatches;
signature/certificate, filesystem metadata and image round-trip checks pass.
The image retains the original DT container and boot layout. Before flashing,
device-side digests match the retained r30 boot/modem_driver rollback pair.

Supported bootloader queries reconfirm ALN-AL00 and unlocked state; product/all
queries remain unsupported. Both r31 partition flashes succeed and #31 reaches
normal Android with sys.boot_completed=1. Device-side digests match both new
images, and the full independent ramdisk digest matches the r30 baseline.
The signed modem module is loaded, sig_enforce=Y and SELinux remains Enforcing.

The complete early/observation ring begins at boot time zero. It records
basp_ocv_enabled:0, successful BCI probing and continuing capacity work through
more than one hundred cycles without either original OCV/capacity lookup
failure. Battery voltage/current/temperature update and match battery_gauge
in the snapshots; capacity remains consistent. Parsed low-voltage thresholds
remain present. No kernel WARNING splat, BUG, CFI failure, panic, unbalanced
enable or misconfigured-IRQ marker is found; taint stays 4. The device boot
identity and audioserver remain stable during observation.

This passes the disabled-provider path and normal short monitoring, not the
enabled-provider runtime path, low-battery cutoff, aging/capacity accuracy,
charging-mode safety or long-duration power parity. No deliberate low-battery
discharge, heavy stress, new audio capture or repeated unrelated functional
tests are performed. Earlier r30 functional checks retain their original scope.
PCIe IRQ affinity errors and both inherited DAPM MUX diagnostics are still
observed. The prior thermal-service fallback and missing RTG interface are
not repaired by this change; audit thermal inputs/units next before stress.
r30 is the immediate paired rollback. Original backups, accepted r17 and the
latest r27-r31 pairs are retained; temporary build/audit logs are removed.

### Read-Only Thermal Calculation Audit

The follow-up reproduces both stock Thermal-daemon CalcTemp rejections during
passive physical-temperature reads without stress or service restart. A
config/ELF audit identifies calculated shell slots, not thermal-zone indices,
and locates rejection when a computed result exceeds its maximum input or
falls below the valid lower bound. Board 13479 has no named group in the
located thermald.xml candidate; its expected default coefficients can reach
that branch even with valid constant inputs. This is not a replay of live
history or proof of every userspace lookup. The default group does not use
DDR or the zero raw-battery nodes. A separate unimplemented raw-temperature
callback/ignored-return gap is recorded, not patched as the shell root cause.
See clang12-thermal-source-audit.md for evidence, limitations and the decision
not to alter coefficients, fabricate temperatures or loosen protection.
r31 remains installed; no kernel build/flash or new functional certification
is introduced. Fallback freshness/thermal safety remain unresolved; next
isolate the separately observed PCIe PME/AER MSI affinity failure.

### Read-Only PCIe MSI Affinity Audit

IRQ267 belongs to the MSI allocation of root port 0000:00:00.0 beneath
f8000000.pci, with PCIe PME/aerdrv actions. The live DT's MSI resource index 1
maps SPI94 to GIC ID126, found as IRQ83 in IRQ sysfs with effective affinity
CPU0; the logical IRQ267 effective mask remains empty. The DesignWare host
installs a chained handler on that physical IRQ. Chained descriptors with
zero counts are omitted by show_interrupts, so the visible IRQ92 pcie-msi
entry must not be mistaken for this root port's line. IRQ267 has zero sampled
activity; these snapshots do not certify delivery, migration or error recovery.

The local DesignWare host's dw_pci_msi_set_affinity unconditionally returns
-EINVAL. Its function matches Linux upstream stable v5.10.43 after whitespace
normalization. MSI domain affinity delegation reaches this logical bottom
chip, not a GIC parent-data entry. The bottom domain is a standalone linear
domain; the Kport driver separately requests the aggregated physical interrupt
and dispatches it through dw_handle_msi_irq. Consequently, changing one
logical vector's affinity is not implemented by this driver topology.

With CONFIG_GENERIC_IRQ_EFFECTIVE_AFF_MASK enabled and the effective mask
empty, irq_needs_fixup falls back to the requested mask. That mask includes
the hot-unplugged CPUs, so migration calls the unsupported setter and reports
the observed -22. The empty-mask fallback and unsupported setter also exist
in upstream v5.10.43; the error does not establish a new compiler regression
or failure of RTC/UFS interrupts. Source provenance:
https://github.com/gregkh/linux/blob/v5.10.43/drivers/pci/controller/dwc/pcie-designware-host.c
https://github.com/gregkh/linux/blob/v5.10.43/kernel/irq/cpuhotplug.c

No fake successful affinity return, effective-mask fabrication, hard-coded
CPU pinning, generic migration suppression or CPU-hotplug disable is added.
Per-vector support would require a correct shared-line/domain contract, not
merely delegating to a nonexistent GIC parent-data entry. Physical delivery
and parent migration need their own bounded hardware check before such a
change; no PCIe error injection or CPU-online/offline writes occur here.
This step changes documentation only and leaves r31 installed. The separate
raw-battery-temperature callback gap is a candidate for the next isolated
API/unit audit, not a proven cause of the shell calculation rejection.

## r32: Separate Raw Battery Temperature From Status

### Cause And Interface Change

The raw-temperature follow-up confirms the separate API gap. The aggregate
bat_raw_temp caller discards coul_drv_battery_temperature_permille's failure,
and the active coul_merge_ops has no corresponding callback. Per-battery
retrieval also conflates signed temperature data and status, returning a
successful zero when that battery's driver is unregistered. The r31 baseline
shows zero aggregate/second-battery raw nodes alongside the working BATT_0
node and ordinary battery temperature.

The live coul_merge DT has cap_ratio_1=0. The existing temperature policy
selects BATT_0 even when other properties support two-battery aggregation;
do not introduce averaging, maximum selection or a new charging policy.
The existing coul_core_get_raw_battery_temperature path calls
coul_battery_retry_temp_permille. Its get_bat_temp result is represented in
millidegrees by the existing PERMILLAGE conversion. This unit does not imply
millidegree sensor resolution. Existing board/shielding/acquisition defaults
and temperature compensation remain unchanged.

Changes are confined to coul_drv.h, coul_merge_drv.h, coul_drv.c,
coul_merge_drv.c, coul_merge.c and battery_thermal.c. The internal raw getter
and aggregate callback now return status with a separate int output value.
The missing aggregate callback delegates to the existing BATT_0 raw getter.
An unregistered battery returns -ENODEV without assigning a temperature;
missing operations retain error reporting. The three thermal get_temp
callbacks return the getter status directly instead of publishing initialized
data on failure. Zero and negative temperatures remain valid signed values,
not error codes. Caller-validated output pointers remain an internal contract;
no speculative fallback or artificial precision is added.

All local declarations, callback assignments and call sites are reviewed and
updated together. These interfaces/registration functions are not exported
module symbols; the callback slot layout does not change. No DT, calibration,
firmware, permissions, SELinux, charging threshold or independent ramdisk
change is made. Existing thermal-zone registration/names remain intact.

### Validation And Limits

Diff/whitespace review and the existing Clang 12 build pass with the original
security configuration. The matched modem module has no missing versioned
imports or CRC mismatches; signature/certificate, ext4 metadata and boot
round-trip/DT preservation checks pass. The installed r31 pair matches its
retained rollback images before flashing. Supported OEM bootloader queries
reconfirm ALN-AL00 and unlocked state; product/all remain unsupported.

Both r32 flashes succeed and #32 reaches normal Android with boot completed.
Device-side digests match boot/modem_driver, and the full independent ramdisk
digest is unchanged. The signed modem is loaded with sig_enforce=Y and
SELinux Enforcing. Six passive reads show bat_raw_temp and bat0_raw_temp
returning the same BATT_0 millidegree value, consistent with the ordinary
Battery/battery_gauge snapshots. They no longer expose the aggregate's old
initialized zero. Reading the existing bat1_raw_temp file instead reports
No such device (ENODEV), confirming the unregistered-provider error path.

The complete early/observation ring starts at boot time zero and contains no
kernel WARNING splat, BUG, CFI failure, panic, unbalanced-enable or
misconfigured-IRQ marker. Taint remains 4, and boot/audioserver/thermal-daemon
identities remain stable during observation. Battery presence/health and
normal properties remain readable, and r31's OCV lookup errors do not recur.
Recent logcat has no fatal-exception/native-crash/ANR marker. PCIe affinity
errors and both inherited DAPM MUX diagnostics remain observed.

This validates active BATT_0 callback wiring, signed-value transport by source
inspection and the actual missing-BATT_1 status path. No cold/zero-degree
stimulus, sensor fault injection, dual-battery hardware, charging stress or
full thermal-accuracy test is performed. Acquisition fallback behavior below
this API is not redesigned or certified. The stock shell calculation guard
is unchanged; absence of its message in a short recent log is not a repair
claim. Earlier functional passes retain their original version and scope.
r31 is the immediate paired rollback. Original backups, accepted r17 and the
latest r28-r32 pairs remain; superseded r27 test artifacts and temporary
build/audit logs are removed after validation.

## r32 Follow-up: RTG Interface Source Gap

Read-only inspection confirms that /proc/<pid>/rtg is absent for the current
system_server as well as PID 1; their ffrt endpoints remain present. The
proc gate is HW_RTG_SCHED, not SCHED_CGROUP_RTG. The saved stock options are
dropped through FRAME_RTG's dependency on PERF_CTRL, whose published Kconfig
and driver traversal are disconnected and whose perf_ctrl.c is missing.
The final ELF independently confirms the omitted rtg handlers.

Selected installed 64/32-bit library disassembly matches the local enable,
configuration and frame-frequency ioctl encodings and payload layouts. No
commands are injected, and no runtime enable, scheduling performance or
complete ABI compatibility is claimed. The known pinned Hepburn reference
also lacks the performance-control driver. No configuration, permissions,
SELinux or partitions change; the device stays on #32 with boot completed,
Enforcing and taint 4. No unrelated functional passes are repeated.

See `docs/clang12-rtg-source-audit.md` for source locations, privilege checks
and the restoration boundary. Locate and audit a complete matching provider
or a justified independent dependency model before attempting a separate
frame-scheduler iteration; an empty proc file is not a repair.

## Performance-Control Dispatcher: Compile-Only Preparation

The subsequent source step restores the pinned Kirin 9000 Linux 5.10.43
misc-device dispatcher and connects its existing build rules. It maps 47
command IDs to Allen's local handlers, preserving the legacy unsupported
SET_TASK_UTIL slot. With PERF_CTRL enabled and frame/statistics/DDR options
disabled, the dispatcher and existing scheduler-query objects compile.
The full enabled build fails at the next missing thermal_perf_ctrl.c provider;
this is not a successful enabled-kernel build or runtime functional pass.

Read-only live DT inspection confirms eight tsens entries with GPU at index
3. The old platform's thermal query candidate reads only indices 0-2 and
also masks some cooling-power lookup errors with zero values. It is not
imported. The default fragment keeps PERF_CTRL and the separate frame/DDR
features disabled, with no partitions, device settings, permissions or
thermal policy changed. Device #32 remains installed; no new paired test
images are staged. See `docs/clang12-rtg-source-audit.md` for sources and the
next bounded query-provider audit.

After returning to PERF_CTRL disabled, the complete baseline build passes.
There are no exported-symbol additions/removals or CRC changes relative to
the saved r32 baseline. Enforcing CFI and forced module signing remain
configured. This does not validate the enabled feature or certify a new
boot/modem pair; no device reboot or unrelated functional retest is performed.

## Thermal Performance Queries: Enabled-Build Preparation

The missing thermal_perf_ctrl.c provider is implemented against Allen's
existing IPA APIs. Live DT inspection identifies three CPU clusters plus
GPU, with g_ipa_sensor_num set from the four-actor configuration. The maximum
query follows that existing IPA range, not the older candidate's fixed three
indices. Existing IPA and system_h acquisitions supply millidegrees; no
conversion, compensation, clipping or protection policy is added.

The live SoC and board thermal zones each bind all four CPU/GPU cooling
devices. Query code now returns backend errors rather than leaving failed
power fields at a successful zero. Valid zero power remains data when the
backend succeeds. Fixed 24/20-byte query payloads are preserved.

The initial full PERF_CTRL-enabled build succeeds. The standard compat
capability-word path is then corrected to store four bytes rather than eight,
and the inactive HWP backend's successful no-op is excluded from dispatch.
The sampled stock 32/64-bit related-TID and render request encodings agree;
this is not exhaustive ABI or runtime compatibility certification.

This is source/build preparation only: device #32, its partitions, ramdisk,
permissions, SELinux and user settings remain untouched. Native/compat query
results, device-node accessibility, cooling failure transport and whole
performance-control behavior await runtime checks. No earlier audio/network
or suspend pass is reused as certification of the unbooted candidate. See
`docs/clang12-rtg-source-audit.md` for provenance and the testing boundary.

Final Clang build and checkpatch pass after the dispatcher changes. Final
ELF inspection confirms both thermal callbacks and driver initialization,
the four-byte compat capability store and the NULL inactive-HWP slot.
The fresh modem module's 390 versioned imports have no missing exports or
CRC mismatches; the complete kernel export set and CRCs are unchanged from
the disabled baseline. CFI and forced signing remain configured. No new
paired test images are packaged or installed; these results do not certify
native/compat runtime transport, module loading or successful boot.

## r36: Performance Queries And Early-Boot Failure Comparison

The completed PERF_CTRL/thermal-query build #36 is installed as a boot and
signed matching modem_driver pair. Packaging, ABI/signature and filesystem
metadata checks pass. Device-side partition readback matches the candidates;
the independent Magisk ramdisk remains unchanged. Normal Android completes
boot, modem_driver is live, taint is 4, signature enforcement is Y and
SELinux remains Enforcing. r32's installed pair is verified before replacement
and retained as immediate rollback.

Temporary native AArch64 and 32-bit ARM root probes use only GET_DEV_CAP,
GET_IPA_STAT and GET_THERMAL_CDEV_POWER. Both capability encodings, fixed-width
temperature/frequency-limit payloads and both valid cooling zones succeed.
Negative/out-of-range zone selectors return EINVAL, with no copied output;
all buffer canaries pass. The standard compat capability writes exactly four
bytes without changing the adjacent sentinel; legacy compat writes eight.
No mutable scheduler commands, backend fault injection or power/load stress
are used. See docs/clang12-rtg-source-audit.md for values and accuracy limits.

The node retains system:system 0660 and hisi_perf_ctrl_device labeling.
Ordinary shell open returns EACCES. Root transport success is not proof of
normal system/app caller accessibility or full performance-control ABI.
FRAME_RTG/HW_RTG_SCHED stay disabled and /proc/1/rtg remains absent. Earlier
audio/network/suspend passes are not repeated or promoted to #36 certification.

### Real Native Crash: Stock Vendor-Init Write-Protection Action

The full early logcat crash buffer contains an init-named SIGSEGV with fault
address 0x14. The matching time-zero kernel ring identifies that PID as the
u:r:vendor_init:s0 subcontext, not PID 1. Immediately preceding messages are
hw_init's SYS_WP reports and do_with_ufs failing to open a path with ENOENT.
PID 1 restarts the subcontext and reports failure receiving the result of
sys_wp_init_action, from post-fs in /vendor/etc/init/init.platform.rc:26.
The crash helper cannot execute in that context, and no new tombstone is
available for this event. Absence of a tombstone is not absence of a crash.

This finding warrants a targeted baseline comparison. The known r32 pair is
restored; its full early ring and crash buffer reproduce the same action,
UFS open failure, subcontext restart and SIGSEGV at 0x14. r36 is restored and
its second boot reproduces the same inherited action failure while Android
again completes boot. No WARNING, BUG, CFI failure, Oops or kernel panic is
found in either r36 early ring or the r32 comparison. This establishes the
failure predates the restored PERF_CTRL candidate, not its exact root cause
or lack of security impact. The write-protection path is not changed, disabled
or bypassed. Next isolate the exact missing UFS path and stock action contract
through read-only inspection before choosing any kernel-side change.

### Other Stack Dumps And Remaining Boundaries

Repeated modem/network startup stacks originate in __rtnl_unlock's OEM
duration diagnostic, and are present in the r32 comparison too. Source
inspection finds start_hold_time initialized only by rtnl_lock, not successful
rtnl_trylock or rtnl_lock_killable, and subtraction uses local_clock across
potential CPU migration. These durations are not reliable proof of actual
multi-second lock ownership; accounting needs a separate fix, not suppressed
warnings or changed network synchronization. The Wi-Fi allocation stack is
from an unconditional dump_stack in wiphy_new_nm, not a WARNING or CFI trap.

Previously attributed PCIe MSI affinity EINVAL remains. Calibration access,
shell-temperature calculation guards, multi-level L3 voting and full frame
RTG are not repaired by the new query provider. No charging, recording,
caller scheduling, calibration, firmware, DT, permission or user-setting
change occurs here. Cleanup removes temporary host/device probes and full
logs, and superseded r28 test artifacts; original backups, accepted r17 and
the latest r29-r32/r36 pairs remain.

## r36 Follow-up: Stock Write-Protection Action Context

Read-only inspection of the installed init, its embedded function symbols
and the live restarted vendor_init subcontext identifies the ENOENT target
as /proc/sys_wp_soft. This is not a per-device UFS sysfs-path mismatch.
The published Allen source lacks that interface; the retained original
ARM64 kernel Image and stock configuration lack the old implementation too.
No original-kernel boot test is performed to infer further runtime behavior.

Both protection success/failure branches call InitPropertySet to report
ro.odm.sys.wp. The main-init permission lookup uses a process-local property
information mapping. Two bounded pointer-word reads per process show PID 1
has a mapped property-info area, while vendor_init has NULL; both callbacks
resolve to InitPropertySet. The lookup reads mapping offset 0x14, consistent
with the already captured SIGSEGV. No action is invoked, process attached or
memory modified to obtain this evidence. Event registers/PC remain unavailable.

Thus the missing kernel interface and userspace callback context are separate
defects. A dummy proc/status implementation would not fix both branches and
would misrepresent protection. The available 4.14 software-WP reference
requires actual BIO write interception and validated partition mapping; it
is not imported into Allen's dynamic-partition 5.10 stack. The matching OEM
init/plugin source is absent, so no binary, boot script, property override or
hardware WP flag is changed. See docs/clang12-system-wp-source-audit.md for
source provenance, call-chain evidence and the proper repair boundary.

Device remains #36 with boot completed, taint 4, SELinux Enforcing and forced
module signing. No rebuild/reboot/flash or previously passing functional
test is needed for this source-only audit. Temporary device copies and host
binary/source/disassembly/logs are removed; r32 remains paired rollback.

## r37: Correct RTNL Diagnostic Acquisition Intervals

The isolated change in net/core/rtnetlink.c starts the existing hold-time
interval after successful killable, trylock and final-refcount acquisitions,
as well as normal rtnl_lock. It uses ktime_get_ns for acquisition and release.
No lock/refcount return semantics, warning threshold, deferred work, netlink
dump locking or public interface changes. See docs/clang12-early-diagnostics.md
for the exact source contracts and uninstrumented raw-mutex boundary.

checkpatch and static diff checks pass. Full Clang #37 build and final ELF
success-gate/clock/diagnostic checks pass. The complete export set and every
CRC agree with r36. The signed matching modem module's 390 versioned imports
have no missing exports or CRC mismatches; signature and filesystem metadata
readback pass. Enforcing CFI, ThinLTO and forced signing remain configured.

The installed r36 rollback pair matches the retained images. Since its ring
has rotated past early boot, one normal reboot captures a same-task time-zero
baseline with 24 RTNL duration reports. The killable register_netdev path
reports durations above two seconds using a stale earlier acquisition time.
No earlier audio/network/suspend pass is repeated as part of this comparison.

Paired r37 boot/modem_driver flash succeeds after serial/model/unlock and
actual partition checks. Normal Android completes boot. Device-side hashes
match both installed images, and the independent ramdisk hash is unchanged.
The matching modem module is live, taint is 4, SELinux is Enforcing and module
signature enforcement is Y. Audioserver and SurfaceFlinger remain running.

Complete startup and post-query rings have zero RTNL duration reports, versus
24 in the r36 baseline. No WARNING, BUG, CFI failure, Oops or kernel panic
appears. The remaining single stack is wiphy_new_nm's already attributed
unconditional dump_stack, not an RTNL duration warning. Read-only ip link
enumeration returns 45 entries; IPv4 and IPv6 all-table route dumps succeed.
No interface, network selection, scheduling setting or data-transfer changes
are made. Successful enumeration is not cellular/Wi-Fi internet certification.

The crash buffer still contains the inherited vendor_init SIGSEGV at 0x14,
and init reports the same sys_wp_init_action failure. This is not repaired,
suppressed or promoted to a clean-userspace claim. No earlier PERF_CTRL query,
audio, capture or suspend result is reused as full r37 certification.

No artificial long RTNL hold or failed-acquisition stress is injected.
Final ELF gates support unchanged failure returns and success-only timestamps;
the original over-threshold diagnostic remains active in code but is not
positively triggered in this bounded runtime check. No additional reboot is
needed after this unchanged candidate passes its targeted validation.
r36 remains immediate paired rollback. Temporary full logs/disassembly and
superseded r29 artifacts are removed; originals, accepted r17 and the latest
r30-r32/r36-r37 pairs remain within the workspace limit.

## r49 Wi-Fi Association And External Endpoint Boundary

This follow-up uses the already installed #49, without another kernel/config/
DT change, build or flash. The r49 timer repair, signed matching pair and
released-slot/global lifecycle results are in clang12-rtg-source-audit.md.
The user explicitly authorizes normal passwordless unlock and stock Settings
operation. No lock credential is bypassed, no network is added and no saved
credentials or calibration parameters are read or changed.

### Correct Shell Attribution

The older temporary networking probe creates com.android.shell from a system
Context. With no active network it does not reach the capabilities query;
after association that query throws `Package android does not belong to 2000`.
RuntimeInit then terminates the uncaught-exception process, yielding exit 137.
Logcat attributes the failure to ConnectivityManager's package validation,
not an OOM, Wi-Fi-driver fault or kernel crash. It is the same system-derived
AppOps mismatch recorded in the r24 Bluetooth probe.

A task-owned copy uses the installed shell ApplicationInfo, ActivityThread's
process-local bound identity and ContextImpl.createAppContext/LoadedApk.
Before a connectivity query it verifies UID 2000, package com.android.shell
and OpPackageName com.android.shell. This reuses the AOSP Android 12 context
contracts cited in the r24 record. It does not spoof another application's
identity, install an APK or change AppOps, permissions or SELinux policy.
The corrected probe successfully queries the real network capabilities.

### Association And Bounded Endpoint Checks

Wi-Fi starts OFF with no active default network. Normal foreground stock
Wi-Fi settings enable the adapter; its existing saved network automatically
associates and becomes a VALIDATED default Wi-Fi network. No personal network
identifier is retained in the durable record.

The probe uses Network.getAllByName and Network.openConnection so DNS and
HTTPS remain bound to that Wi-Fi network rather than an assumed default
route. Redirects and cache reuse are disabled; connect/read timeouts and
response-length bounds are present, with ordinary certificate validation.
Eight integrity-checked rounds are planned, not completed:

- A pinned public kernel Makefile at raw.githubusercontent.com resolves DNS
  but resets the connection during TLS handshake.
- The same verified content at cdn.jsdelivr.net also resolves DNS and resets
  during TLS. No proxy, certificate or firewall workaround is introduced.
- A public Linux 5.10.42-to-5.10.43 incremental patch at the Tsinghua mirror is
  verified by direct host HTTPS, XZ decoding and its expected version diff.
  The phone resolves the mirror, completes default TLS and receives HTTP 403,
  so its expected status/content/integrity check stops without claiming a
  successful file transfer.

These results establish association, a validated route, bound DNS and a
default-TLS HTTP response. They do not establish sustained transfer or exact
payload integrity. Host download success is not substituted for a phone
transfer. The two connection resets and mirror 403 are endpoint observations;
without further route/server evidence they are not attributed to the kernel,
RF calibration or a proven external filtering mechanism. The user then
narrows completion to local kernel/Git closeout, so no additional endpoints
or network settings are tried to manufacture a pass.

### Restoration And Limits

Overlapping root snapshots cover the complete enable/association/probe/
restoration interval without a log gap, despite later ring rotation. No
actual kernel WARN, BUG, Oops, panic or CFI failure occurs. AVCs are retained
and classified, not described as absent: shell property/idmap/proc/sysfs
access, background logserver/storage/DRM/TEE/AOD denials and Wi-Fi-domain
property/block-directory access coexist with successful association and
the HTTPS response. No denial is fixed by weakening protection. The existing
calibration permission/write-on-sync-failure contract remains independently
open; network connectivity alone does not certify RF calibration.

Wi-Fi is restored OFF, with no default network. Mobile-data, Bluetooth,
airplane, always-scan, location, automatic-brightness mode and timeout settings
match their baseline. Home foreground, Asleep and keyguard showing are
restored. The device remains on the same #49 boot, SELinux Enforcing, taint 4,
signature enforcement Y and the matching modem-only module baseline. Original
stopped tracing instances are untouched; only the task-owned device DEX and
consumed host diagnostics are removed. No user data or recorded media is
transferred.

This is a local scoped functional checkpoint, not full networking or release
acceptance. Verified transfer, cellular with a supplied SIM, Bluetooth peer
audio, charging/overnight power and manual audio/video fidelity remain for
the next stage, alongside the documented source and ABI boundaries. Per the
latest user request, no release package, exported artifacts or publication
is created.
