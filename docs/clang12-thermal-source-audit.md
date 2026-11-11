# r31 Thermal Calculation Boundary Audit

## Scope And Sources

The installed r31 kernel repeatedly logs stock userspace Thermal-daemon
CalcTemp shelltemp[0]/[5] rejections. This read-only audit inspects the
unchanged /system/bin/thermal-daemon ELF, its library/descriptor names,
/hw_product/etc/hwpg/thermald.xml, thermal sysfs values and local driver source.
No process memory, user content or sensor emulation is collected. The device
remains in the same normal r31 boot with stable thermal-daemon/audioserver,
taint 4, sig_enforce=Y and SELinux Enforcing.

## Configuration And Error Meaning

The daemon resolves configuration through the stock cust policy and matches
ro.board.boardid, retaining a default group when no specific group matches.
The observed board ID is 13479. Among the searched policy directories, only
the retained hw_product thermald.xml candidate is found. Its groups name
13468/13469, 13485/13487/13489 and default, not 13479. The expected selection
is therefore default; live in-memory coefficients are not independently read.

That group defines seven calculated shell results. shell_coef0 is named
shell_front and shell_coef5 is named shell_back. The log indices identify
these calculation slots, not thermal_zone0/5 and not two failed physical
sensors. Their configured inputs are charger, pa_1, rfboard, pa_2, system_h,
pa_s, dcxo0 and usb_port, with Battery also used by the separate bias path.
These configured NTC inputs use millidegree values. Do not apply that unit
assumption to every OEM thermal zone, including the separate CPU/GPU zones.

In the inspected ELF, the calculation at 0x151b8 accumulates coefficient
weighted history, divides by 100000 and adds the configured constant. Its
decision at 0x1532c-0x15334 rejects a result above the maximum input-history
temperature or below -40000. The branch at 0x155e8 retrieves the last stored
result and emits the specific abnormal-shelltemp message. This is distinct
from the later smoothing/step-range diagnostics and from a kernel WARN splat.

The default coefficients themselves can exercise this guard with valid
inputs: if every history sample is 25000, slot 0 has weight sum 91611 and
constant 2962, yielding 25864; slot 5 has weight sum 93079 and constant 3227,
yielding 26496. Both exceed the maximum input 25000. This is illustrative
static arithmetic, not a replay of the daemon's live history or a calibrated
temperature measurement. It shows that a broken sensor is not necessary to
produce the message; it does not prove which history caused every rejection.

## Passive Observation And Separate Gaps

Eight passive snapshots span a complete seven-sample, five-second history
window without stress or service restart. The configured physical inputs,
Battery and published shell temperatures are readable; physical values
change during the interval while both calculation rejections recur. The
daemon retains open descriptors for several real thermal inputs, including
the Battery thermal zone. No root-cause claim relies on root readability
alone establishing every userspace lookup or SELinux permission.

DDR's -40 and zero-valued bat_raw_temp/bat1_raw_temp/supply nodes are not
referenced by the default shell coefficients, so they are not established
causes of these shell-result rejections. A separate source gap exists:
battery_thermal_get_temp ignores the return from
coul_drv_battery_temperature_permille, while the active coul_merge_ops does
not supply get_battery_temp_permille. Its zero-initialized result can therefore
be exposed as a successful raw-temperature read. This requires its own API,
aggregation and unit audit; do not substitute a different sensor or multiply
a coarse temperature merely to eliminate a zero. The currently configured
shell battery path uses Battery, not this raw node.

## Decision And Next Step

There is no demonstrated kernel temperature-read failure justifying a driver
patch for these CalcTemp messages. Do not alter stock coefficients, raise
the calculation bound, restart/replace the thermal daemon, fabricate values
or disable thermal protection. Fallback freshness and behavior under real
charging/load remain unresolved, so heavy stress is still deferred.

This step changes documentation only; it does not claim restored thermal
accuracy or full power safety. Next isolate the independently observed
PCIe PME/AER MSI affinity failure before another kernel iteration. Raw ELF,
config copies, disassembly and logs are removed after retaining this audit.

## r32 Follow-Up

The separate raw-temperature API gap is subsequently repaired in r32 with
status/output separation, the existing BATT_0 callback and direct thermal
error propagation. Valid BATT_0 reads and the unregistered BATT_1 ENODEV path
pass targeted checks; the stock shell calculation coefficients/guard and
lower-level acquisition defaults remain unchanged. See the r32 section of
clang12-functional-tests.md for source scope, validation and remaining limits.

## r41 Synchronized Cold-Window Follow-Up

Ten passive snapshots over approximately 47 seconds cover more than a full
seven-sample history window. The same daemon PID remains running. All eight
configured physical inputs and Battery remain readable; the maximum sampled
physical input rises from 24715 to 24905 mC. Published shell_front remains
24632 mC and shell_back remains 26295 mC. The fresh log contains 20 CalcTemp
rejections, ten for each slot, reporting cached front 24632 and back 26296.
The one-millidegree back difference is observed, not attributed to a specific
rounding/bias path without publication evidence.

An earlier log reports a different cached front value, 24795. Thus the
observable cache is not permanently fixed, but the two observations do not
establish an accepted calculation or explain the update. Do not compare
older log values with current sysfs to infer a synchronized bias pipeline.
Search the exact `is abnormal` diagnostic: generic error/failed filters can
miss it even though the daemon emits it at error level.

No stress, sensor/control write, service restart or kernel change occurs in
this follow-up. This cold window does not certify freshness, recovery or
protection during charging/warming. Next audit publication/scene selection
and observe a bounded normal warming/recovery window when appropriate;
there is still no demonstrated kernel acquisition failure justifying a
temperature-unit or ADC patch for these messages.
