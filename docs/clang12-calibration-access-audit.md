# r41 Connectivity Calibration Access Audit

## Scope And Baseline

The recorded Wi-Fi HAL/supplicant-context conn_calidata open failures return
-EACCES even during successful foreground scanning and saved-network
association. This audit follows the local hi1105 NV lifecycle, compares the
retained original Allen Linux 5.10.43 machine code and performs a bounded
read-only on-device record check. It does not enable Wi-Fi, alter firmware,
RF parameters, block permissions, SELinux, ramdisk or calibration contents.

The confirmed device remains r41, with r37 as its paired rollback. The live
partition resolves to /dev/block/sdd5, mode 0600 root/root and SELinux type
certification_device. The Wi-Fi HAL runs as wifi in hal_wifi_default; root
diagnostic access is not evidence of HAL-authorized access.

## Factory And Published Lifecycle

Sources under Code_Opensource/kernel/drivers/connectivity/hi11xx/hi1105:

- common/customize/hisi_conn_nve.c allocates a zeroed RAM buffer; setup_done
  means allocation succeeded, not that disk contents are synchronized.
- wifi/chip/hisi_conn_nve_interface.c initializes a semaphore, partition
  size and RAM pointer in conn_nve_init_wifi_nvdata, then resets the sync
  flag. This function does not preload disk calibration in a kernel thread.
- A read consumer reaches conn_nve_read_prepare_process, takes g_nv_sem and
  lazily calls read_nv_to_ramdisk. Successful CRC/age selection publishes the
  synchronized RAM record; failed reads do not establish usable calibration.
- common/customize/hisi_conn_nve.c performs open/lseek/read through syscall
  wrappers in the caller's credentials. set_fs(KERNEL_DS) only changes the
  address-limit contract; it does not supply privileged kernel credentials.

The factory hisi_conn_nve_read_mp at 0xffffffc0116cb41c also calls
do_sys_open at 0xffffffc0116cb48c, then ksys_lseek and ksys_read. Its open
error maps to -ENODEV after logging the actual descriptor error. There is no
credential-switch call in this function. The factory init likewise has no
disk preload. This is an inherited API/lifecycle contract, not an established
new Clang permission regression. Merely replacing syscall wrappers with
filp_open/kernel_read would still use caller credentials.

## Bounded Read-Only Record Check

The local header defines two adjacent records, each 392076 bytes, with CRC
in the final four bytes and age immediately preceding it. r41 DWARF reports
struct size 0x5fb8c, and both original/r41 read instructions use that same
size and second-record offset. The confirmed block device is 1 MiB; the
probe stays within the two records, not unrelated trailing storage.

Despite its crc32c_conn_nve name, all 256 original table entries match
reflected IEEE CRC-32 polynomial 0xedb88320 with initial/final inversion,
not the Castagnoli CRC-32C polynomial. A temporary freestanding ARM64 probe
first passes the independent standard `123456789` CRC vector. In the
confirmed root context it opens O_RDONLY, uses complete bounded pread64
calls and emits aggregate classifications only. No raw bytes, calibration
parameters, CRC values or age values are transferred or persisted.

Both complete records are all zero, including their CRC trailers; neither
has a valid stored CRC. A targeted extension after that unexpected no-valid-
record result distinguishes all-zero from all-0xff or mixed records. It
finds two all-zero records and zero all-0xff records. Thus this location
contains no valid record for the examined factory/current format. This is
not evidence that a prior kernel erased it, that calibration is corrupt
elsewhere or that the radio is wholly uncalibrated. Bypassing -EACCES cannot
recover valid records from these zero-filled slots.

The source separately reads FBGAIN through external_nve_direct_access_interface
and has customization masks for optional DPD/power records. Their presence
does not certify live configuration or all RF calibration sources. The
probe invokes no driver calibration ioctl or automatic initialization; its
only partition operation is read. Its device executable is removed afterward.

## Write-Safety Boundary And Remaining Work

conn_nve_check_sync_before_write calls read_nv_to_ramdisk when unsynchronized.
For any failure it attempts conn_nve_clear_all, zeroing the RAM record and
writing initialized records to both slots. It does not distinguish an empty
partition from a permission/I/O failure or unrecognized valid format. The
factory gate also branches from a nonzero read result to conn_nve_clear_all
at 0xffffffc0117f4e84. Current permission refusal also prevents the attempted
open-for-write; it must not be removed with blanket chmod, credential
overrides or an unrestricted privileged worker.

The low-level read additionally treats only -1 as read/seek failure and
does not reject a short transfer. These are source-level safety gaps, not
observed successful runtime corruption in this audit. No automatic-clear
path or partition write is exercised to test them.

The next calibration step must establish intended optional-record usage and
the authorized reader/writer lifecycle separately. Any privileged preload
proposal must be strictly read-only and must not make the inherited
clear-on-failure path reachable with elevated authority. A genuine empty-
record result must remain distinct from transport/permission errors; do not
manufacture a valid zero record or suppress errors to claim success. External
network transfer and broader RF correctness remain unverified, so the whole
calibration acceptance item stays open with this bounded classification.
