# PA14 safety-switch polling

## Goal
Implement the now-complete screenshot requirements with the later override: PA14 low-active safety switch uses periodic GPIO polling, never new EXTI; source only, no build/flash. PB0 defaults to HIGH-active digital ON/OFF (LOW released) with ~10 ms debounce; all new changes receive Chinese GBK comments; per latest user request, no static checking, compile-time ADC fallback; PA7 power-on check unchanged. Safety trips force six PWM pins low and latch, including IPD/commutation/fault recovery/wakeup; boot-held safety skips energizing MOS self-test. Restart requires safety released, confirmed ON/OFF release, then fresh press and no existing faults. Disable PA14 LED drive; retain PA1/PA13 fault blinking only, no SOC/gear display, but retain gear control. Preserve CP936 and user changes.

## Current Phase
Complete — safety switch now uses the existing hard-brake states/output with the restart latch preserved.

## Next Step
Hand off the applied hard-brake synchronization; do not run static checks, tests, compilation or flashing.

### Phase 4: Safety hard-brake stop
**Status:** complete
- [x] Reuse the existing hard-brake timing/output behavior for a runtime safety event.
- [x] Preserve the restart latch, original fault priority, and no-energizing boot-held safety behavior.
- [x] Add Chinese GBK comments; source edits only, no static checks/build/flash.

## Latest requirement (supersedes coast-only safety stop)
The user enabled hard braking for ON/OFF release and explicitly requests the same brake-to-stop behavior on the safety switch. Do not retain the old all-six-low coast behavior during a permitted braking interval; keep the no-restart lock and go all-low after the brake finishes or a fault requires it.

## Current recovery note
Permission now works. The stale config script stopped after partial header edits because the prior implementation was already on disk. The recovery script correct_current_sources.py successfully updated 13 source/header files in place, removed the duplicate definitions, changed PB0 to high-active/pull-down, and added Chinese comments using GBK bytes without re-encoding existing comments. No rollback of unrelated user changes.

### Phase 1: Inspect and resolve requirements
**Status:** complete
- [x] Check current diff and saved planning state.
- [x] Establish PA14 ownership, switch polarity, and ON/OFF behavior.
- [x] Resolve any hardware requirements not recoverable from source/screenshot.
- [x] Trace all motor output paths, ADC variants, and LED behavior.

### Phase 2: Implement GPIO polling and restart latch
**Status:** complete
- [x] Make focused encoding-preserving source edits.
- [x] Retain existing unrelated protections and changes.

### Phase 3: Handoff without checks
**Status:** complete
- [x] Record the applied edits for handoff; static checking, compilation and flashing were deliberately not performed, as requested.

## Decisions Made
- Screenshot supplies the agreed polling-only approach; no new external interrupt for PA14.
- Existing changes at entry are Keil UI and build outputs only; leave them untouched.

## Errors Encountered
- On permission recovery, update_config.py partially wrote headers before stopping because ENABLE_PWMOUT already called PwmOutputsEnable. Existing implementation was already on disk, so the saved no-source-changes assumption was stale. Adapting current source and merging only duplicate declarations/macros from the interrupted application; not rerunning the old scripts.
- Broad tool-location Glob under D:/ timed out; use the exact ARMCC directory from the existing Keil build log instead. No build was attempted.
- Initial reg_gpio.h lookup used HAL include path; located actual header with Glob under Device/MM32G0001/Include.
- Auto permission-classifier service (claude-sonnet-5[1m]) remained unavailable on resumption: Agent, temporary patch-script Write, and a normal byte-preserving source-edit Bash request were not executed. Firmware sources remain unchanged; wait for the service or a user-controlled permission-mode change.
