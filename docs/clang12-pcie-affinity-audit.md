# r41 PCIe Interrupt Topology And Delivery

## Existing Affinity Diagnosis

The earlier read-only audit in clang12-functional-tests.md attributes
IRQ267's -EINVAL to the DesignWare logical MSI affinity setter, also
unimplemented in upstream Linux v5.10.43. The logical bottom MSI domain is
separate from its physical GIC dispatch line; it is not a GIC parent-data
entry to which arbitrary per-vector affinity may simply be delegated.
The empty effective-mask fallback explains the attempted migration when
requested masks include offlined CPUs. No success stub, fabricated mask,
CPU-hotplug suppression or hard-coded affinity change is introduced.

## Live r41 Topology

Read-only sysfs identifies these devices beneath f8000000.pci:

- Root port 0000:00:00.0 binds pcieport and reports IRQ267. Its actions are
  PCIe PME/aerdrv, logical PCI-MSI hardware vector 0.
- Endpoint 0000:01:00.0 binds hi110x_pci and reports IRQ82. Its live action
  is hisi_ete_intx, physical GICv3 hardware IRQ125.
- IRQ83 is the separate GICv3 hardware IRQ126 line used for root-port MSI
  dispatch. Its chained descriptor has no ordinary named action, as in
  the prior host/domain audit; zero show_interrupts activity must not be
  treated as proof that the descriptor is absent.

The host is not an unused root port that can be disabled to hide the
diagnostic: its connectivity endpoint is present. Nor is IRQ267 the
endpoint's normal data/control interrupt. The local connectivity
platform/oal/pcie/pcie_linux.c registration path requests hisi_ete_intx on
pci_dev->irq when the endpoint uses its non-MSI ETE path. The live binding
and IRQ actions establish that this is the current endpoint route, not
merely a compile-time alternative.

A five-second idle observation leaves endpoint IRQ82 at 22 cumulative
deliveries and both root MSI counters at zero. That idle snapshot alone
does not validate fresh delivery or justify treating zero as a failure.

## Bounded Endpoint Activation Check

The installed r41 device is initially Wi-Fi OFF and asleep/keyguard-locked.
The supported cmd wifi set-wifi-enabled interface temporarily enables Wi-Fi,
observes fifteen two-second samples, then disables it in cleanup. No network
is added or explicitly selected; no DNS/data transfer, scan-policy/location
change, explicit calibration-write ioctl, PCIe fault injection, affinity write or forced
CPU hotplug is performed. Existing saved-network policy is not modified.

Over approximately 32 seconds, Wi-Fi reports disabled -> enabled -> disabled.
IRQ82 increases from 22 to 137 during activation and 144 after deactivation,
122 new physical INTx deliveries in total. IRQ83 and logical IRQ267 remain
zero. The retained kernel log covers the complete interval and contains
45 existing IRQ267 affinity-refusal messages while the independent endpoint
interrupt path continues delivering. No WARNING, BUG, panic or CFI-failure
marker appears in that interval.

This proves fresh endpoint interrupt delivery on r41 during normal radio
activation/deactivation, not external networking, packet fidelity, INTx
migration, root-port PME/AER event delivery or PCIe error recovery. The
affinity-refusal messages do not establish that the endpoint's GIC INTx
route has failed; the two routes must remain distinguished. No logical MSI
affinity behavior is claimed successful or silently bypassed.

The boot ID is unchanged. Wi-Fi is restored to OFF, display remains Asleep,
SELinux is Enforcing, taint is 4, PRMEM is enabled and forced module signature
verification remains Y. No kernel edit, build, flash or independent-ramdisk
change occurs. Task-owned counter samples and the full log are consumed and
removed after recording the result.

## Remaining Acceptance Boundary

Root-port PME/AER delivery and shared-line migration remain unverified; zero
events in this test cannot certify either. A bounded legitimate root-port
power/wake event with its correct hardware/driver prerequisites is required
before implementing affinity routing or asserting root-service recovery.
Do not inject a PCIe fault or alter shared-line power/CPU routing merely to
obtain a passing counter. The broader external-radio/power checks and final
whole-device signoff remain open.
