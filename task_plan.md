# PA14 safety-switch polling

## Goal
Implement the now-complete screenshot requirements with the later override: PA14 low-active safety switch uses periodic GPIO polling, never new EXTI; source only, no build/flash. PB0 defaults to low-active digital ON/OFF with ~10 ms debounce, compile-time ADC fallback; PA7 power-on check unchanged. Safety trips force six PWM pins low and latch, including IPD/commutation/fault recovery/wakeup; boot-held safety skips energizing MOS self-test. Restart requires safety released, confirmed ON/OFF release, then fresh press and no existing faults. Disable PA14 LED drive; retain PA1/PA13 fault blinking only, no SOC/gear display, but retain gear control. Preserve CP936 and user changes.

## Current Phase
Phase 2 — implement digital/ADC input selection, polled safety interlock, and fault-only LEDs.

## Next Step
Apply byte-preserving source edits using Python because Edit/Write cannot preserve existing CP936 comment bytes.

### Phase 1: Inspect and resolve requirements
**Status:** complete
- [x] Check current diff and saved planning state.
- [x] Establish PA14 ownership, switch polarity, and ON/OFF behavior.
- [x] Resolve any hardware requirements not recoverable from source/screenshot.
- [x] Trace all motor output paths, ADC variants, and LED behavior.

### Phase 2: Implement GPIO polling and restart latch
**Status:** in_progress
- [ ] Make focused encoding-preserving source edits.
- [ ] Retain existing unrelated protections and changes.

### Phase 3: Static verification and handoff
**Status:** pending
- [ ] Check paths for stop, latch, release, repress, boot held, and bouncing inputs.
- [ ] Check diff, encoding, and lack of new EXTI setup; do not build or flash.

## Decisions Made
- Screenshot supplies the agreed polling-only approach; no new external interrupt for PA14.
- Existing changes at entry are Keil UI and build outputs only; leave them untouched.

## Errors Encountered
- Broad tool-location Glob under D:/ timed out; use the exact ARMCC directory from the existing Keil build log instead. No build was attempted.
- Initial reg_gpio.h lookup used HAL include path; located actual header with Glob under Device/MM32G0001/Include.
- Auto permission-classifier service (claude-sonnet-5[1m]) remained unavailable on resumption: Agent, temporary patch-script Write, and a normal byte-preserving source-edit Bash request were not executed. Firmware sources remain unchanged; wait for the service or a user-controlled permission-mode change.
