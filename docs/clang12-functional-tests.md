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

Current test pair is r20:

- `images/test/allen-boot-test-5.10.43-clang12-r20.img`
- `images/test/allen-modem_driver-test-5.10.43-clang12-r20.img`

Keep accepted r17 and the stock originals as paired rollback paths. L3 c1/c2
multi-level voting, Wi-Fi association/data, cellular service, Bluetooth,
deep suspend, long-term stability and high-resolution/surface video playback
remain unverified or unresolved. Do not guess the additional L3 vote fields.
