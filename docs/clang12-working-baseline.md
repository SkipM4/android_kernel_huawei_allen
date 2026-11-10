# Accepted Clang 12 Normal-Boot Baseline

## Identity And Source

- Accepted on 2026-11-11: r17, Linux 5.10.43, on ALN-AL00 / HWALN (HW Allen).
- Target serial: `FMR0223C20002621`; board ALN, Kirin 9000S / platform 8425L.
- Source: supplied official ALN HarmonyOS 4.2 sources, branch `bringup/clang12`.
- Source-fix commit: `81ad87b1a`, including the earlier targeted bring-up fixes.
- Running build: `#17 SMP PREEMPT Wed Nov 11 01:35:56 CST 2026`.
- Compiler: official Android Clang/LLD 12.0.7, local clang-r416183b1.

This meets the self-built kernel normal-system-boot goal. It is not a claim of
complete firmware feature parity or a production/long-term stress certificate.
No stock rollback is performed after the original acceptance checks. The r17
pair remains retained as a proven rollback baseline. Subsequent functional
work installs r20 as a newer test candidate; see `docs/clang12-functional-tests.md`
for its targeted checks and outstanding hardware-validation limits.

## Proven Checks

- Initial flash boot and one normal reboot both reach sys.boot_completed=1
  on build #17 and show the actual Huawei home launcher.
- The second boot runs continuously for over 65 minutes with an unchanged
  boot ID, beyond the attributed r16 storaged panic at about 60 minutes.
- Three random-file O_DIRECT write/read/hash rounds pass; another direct-I/O
  check passes after reboot. Owned files and diagnostic probes are removed.
- Encrypted F2FS /data is read-write, user 0 CE storage is available, and
  SurfaceFlinger/activity/power services and the signed modem module run.
- Full boot/modem_driver readback matches the staged pair; the mounted module
  matches the signed companion and the separate Magisk ramdisk is unchanged.
- Runtime CFI/shadow is enforcing, CFI permissive is unset, module signatures
  are forced SHA512, sig_enforce is Y and SELinux remains Enforcing.
- io_uring setup/close succeeds for the existing permitted UID 1013. UID 0
  correctly receives EPERM. The kernel UID gate and policy are not modified.

The direct-I/O test matters: r16's successful buffered checks and two boots
did not exercise the hyperframe callback that later panicked. r17 supplies
standard typed canonical CFI entries, retaining the original assembly bodies.
See `docs/clang12-cfi-iterations.md` for the fault attribution and each repair.

## Accepted Images

Keep these together and preserve r17 as the best successful baseline even
when it falls outside the five-most-recent candidate window:

- `images/test/allen-boot-test-5.10.43-clang12-r17.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r17.img`.
- Signed companion: `images/test/allen-modem_driver-test-5.10.43-clang12-r17.ko`.

The boot retains header v2, the firmware stub, complete OEM DTB container and
legacy LZ4 compression. It does not incorporate the separate Magisk ramdisk.
The modem image is the dedicated read-only ext4 module filesystem, not modem
firmware or a user-data partition. Its 390 versioned imports match this
kernel, with no missing exports or CRC mismatches; CMS and metadata checks
pass. Do not mix the accepted boot with the incompatible stock module image.
Unlocked-device transfer/readback is verified, not OEM AVB signing equivalence.

## Reproduction

From `/home/data/Allen`, use the stock config and compatibility fragment:

```sh
mkdir -p out/allen-clang12-stock-compat
Code_Opensource/kernel/scripts/kconfig/merge_config.sh -m \
  -O "$PWD/out/allen-clang12-stock-compat" \
  configs/allen-stock-5.10.43.config configs/allen-stock-compat.fragment
OUTPUT_DIR=out/allen-clang12-stock-compat scripts/build-allen.sh olddefconfig
OUTPUT_DIR=out/allen-clang12-stock-compat scripts/build-allen.sh
```

The local toolchain, host dependencies, independent output directory and
bounded ThinLTO build are described in `docs/bringup-baseline.md` and
`docs/stock-module-compat.md`. Keep local private signing keys in ignored
output, never in Git. Rebuild only for a real source/configuration change.

Stage future candidates with a new versioned name using the repack and modem
staging scripts in README. Both must come from the same output. The scripts
refuse existing paths and verify image structure, bounds, module ABI, signing
certificate, CMS signature, ext4 integrity and SELinux metadata. Passing those
checks does not replace a new candidate's runtime-CFI and boot validation.

## Paired Stock Rollback

If a future candidate fails, first collect the current recovery pstore fault,
identify its build and verify the tested partition readback. Do not mistake
old pstore or embedded bootloader logs for the new failure.

After serial-pinned fastboot devices/getvar product/getvar all and OEM
model/unlock checks confirm ALN-AL00 and unlocked state, restore both originals:

```sh
fastboot -s FMR0223C20002621 flash boot \
  images/backup/allen-stock-5.10.43-20250805-boot.img
fastboot -s FMR0223C20002621 flash modem_driver \
  images/backup/allen-stock-5.10.43-20250805-modem_driver.img
fastboot -s FMR0223C20002621 reboot
```

Standard product/all getvars are unsupported on this bootloader; the recorded
OEM model/unlock checks establish identity. Verify normal boot_completed=1,
the original BiSheng kernel, loaded stock module, Enforcing SELinux and full
boot/modem_driver/unchanged-ramdisk readback after rollback. Never erase/format
partitions, touch user data, or flash bootloader/NV/GPT to recover this test.

## Remaining Work And Risks

- Published MIPS parsing rejects the live L3 c1/c2 multi-level frequency
  tables. Their monitors remain unavailable; r15 repairs lifetime errors,
  not multi-level voting. Healthy DDR/L3 c0 and CPU schedutil remain enabled.
- The known early sensorhub uninitialized-spinlock warning and taint 516
  (S/W) remain. Captured early AVC denials require separate policy/label
  investigation; enforcing policy was never relaxed for this bring-up.
- Published HKIP_CFI_HARDEN code and OEM PGO/code-merge inputs are absent.
  Standard CFI and available PRMEM/HKIP remain, but that OEM table hardening
  is disabled and ThinLTO replaces stock FullLTO; these are real differences.
- SQE submissions, OEM hyperframe opcodes and MIPS device unbind/removal are
  untested. Typed CFI metadata does not certify opaque assembly layout/ABI.
- Physical touch, camera, calls/cellular data, Wi-Fi association, suspend and
  power consumption are not fully validated. r15/r16 Wi-Fi enable/disable and
  injected UI input do not establish full hardware or networking coverage.

Next retain this bootable pair while validating individual hardware paths and
addressing the documented gaps, before attempting a newer-kernel migration.
