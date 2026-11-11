# Bring-up Completion Plan

## Acceptance Rule

User requests stepwise repair and validation until all required work is
complete. Baseline is installed r37 on bringup/clang12 at e920e7568; immediate
paired rollback is r36. Each kernel iteration stays isolated, with matching
boot/modem_driver validation, unchanged independent ramdisk unless separately
required, retained security controls and accurate functional scope.

An item is complete only after its real acceptance condition passes. Source
availability, missing peers/SIMs and manual checks are external prerequisites,
not successful tests. A harmless/inapplicable diagnostic may be classified
instead of suppressed, but its functional uncertainty must be resolved before
closing the corresponding item. No dummy interface, fake success, protection
bypass, speculative broad backport or implementation-mirroring test suite.

## Work Queue

| Step | Work | Acceptance Condition | State |
| --- | --- | --- | --- |
| 01 | Wi-Fi external networking | Existing-network association, network-bound DNS/TLS and verified transfer; classify calibration access separately | Needs Unlocked Foreground UI |
| 02 | Cellular service | With an active SIM: registration, data and user-confirmed incoming/outgoing audio call; no guessed dial target | Needs SIM/Manual Check |
| 03 | Bluetooth peer audio | Pair/connect a supplied headset, playback/capture route and reconnect checks | Needs Peer/Manual Check |
| 04 | Frame RTG | Audit configuration/task/Binder/module ABI; restore real interface and validate intended caller behavior | String-Copy Repair Checked; Lifecycle Audit Working |
| 05 | L3 multi-level voting | Match DT/firmware vote contract, repair missing implementation and validate real level changes | Pending |
| 06 | Thermal daemon fallback | Resolve calculated-result/source/unit contract and validate without disabling thermal protection | Pending |
| 07 | Wi-Fi calibration access | Identify caller/lifecycle/permission contract; prove correct access or justified non-use without widening protection | Pending |
| 08 | Vendor-init WP action | Matching userspace/protection source, correct process-context reporting and genuine protection behavior | Needs Matching Source |
| 09 | PCIe affinity/delivery | Verify physical/logical interrupt topology and classify functional impact before any affinity implementation | Pending |
| 10 | Charging/standby | Appropriate charger and battery state; charging transitions, temperatures, overnight power and wake stability | Needs Hardware/Time |
| 11 | Camera/audio integration | User-confirmed front/rear recording, audio-video sync, microphone routes and calls/headset integration | Needs Manual Check |
| 12 | Final regression/security | Installed final pair, ABI/signatures/readback, enforcing controls and scoped regression with every required item closed | Pending |

Existing bounded passes include storage/display/touch/USB, hardware video
codec exercises, basic sensors, audible settings playback and recorder user
reports, bounded RTC/deep suspend, PERF_CTRL queries and repaired RTNL timing.
They are not automatically recertified on each new kernel. Repeat or extend
only when a relevant change, fault or uncovered requirement justifies it.

## Current Prerequisites

Read-only r37 connectivity baseline has no active default network, Wi-Fi OFF,
mobile data enabled and both SIM slots ABSENT. One saved Wi-Fi network exists.
USB is supplying power; battery reports full at the snapshot, which is not
enough to validate charging progression. The matching OEM init/plugin source
is absent, as documented in clang12-system-wp-source-audit.md.

A bounded Wi-Fi enable attempt does not establish a default network within
60 seconds. Service logs show screenOn=false; window policy shows the device
is keyguard-locked. Wi-Fi OFF and the original mobile/airplane/scan/location
settings are restored. A foreground unlocked settings check is needed before
classifying this result as a driver issue. No external transfer is attempted
without an active Wi-Fi network; no lock-screen bypass is used.

The follow-up snapshot still shows the same locked/asleep device and absent
SIMs. Continue source work while these manual prerequisites wait. The frame
RTG stock-feature closure changes only its five original flags in a temporary
configuration. Its affected objects compile, and three ignored nested user
copies are repaired. Runtime scheduling remains disabled on r37; enable/timer
lifecycle, authorization, complete link and module compatibility must pass
before exposing the restored interface.

Long-term records omit personal network/subscription identifiers, temporary
checksums and complete logs. Task-owned host/device probes and downloaded
test data are removed after results are recorded. Workspace remains below
20 GB; original backups and immediate rollback are preserved.
