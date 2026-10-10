# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Sensorless **BLDC 6-step (trapezoidal) motor-control firmware** for a cordless power tool (an electric wrench / impact driver — the trigger-throttle, 3 gears, CW/CCW direction, auto-stop and battery-SOC LED display logic all live in `USER/user_control.c`). Bare-metal, no RTOS.

Target MCU: **MindMotion MM32G0001A1TC**, ARM Cortex-M0, **16 KB flash / 2 KB RAM**, core from HSI at 48 MHz (`SYSCLK_HSI` in `USER/parameter.h`). This is a very small part — avoid heap, large buffers, and `printf` on any control path.

## Build / flash

There is no command-line/CI build and no test or lint setup — this is a **Keil MDK (µVision 5)** project using the **ARMCC v5 (armcc)** toolchain (`uAC6=0`).

- Build: open `MDK-ARM/BLDC-6Step.uvprojx` in Keil µVision and build, or headless with Keil on PATH: `UV4 -b "MDK-ARM/BLDC-6Step.uvprojx" -o build.log` (check `build.log` / `MDK-ARM/Objects/BLDC-6Step.build_log.htm` for results). Output image: `MDK-ARM/Objects/BLDC-6Step.axf`.
- Target name "Target 1"; preprocessor define `USE_STDPERIPH_DRIVER`; optimization `-O2`.
- Flash/debug via **MM32-LINK MINI (CMSIS-DAP)**. If using J-LINK instead, console UART config in `USER/platform.c` must change (see `USER/readme.txt`).
- `MDK-ARM/Objects/` and `MDK-ARM/Listings/` are build artifacts (committed but regenerated); don't hand-edit.

## Architecture (layers, bottom → top)

1. `Device/` — CMSIS Cortex-M0 core, MindMotion vendor HAL (StdPeriph-style `hal_*.c` / `RCC_*` `GPIO_*` `ADC_*` `TIM_*` `FLASH_*` API, legacy `u8/u16/u32` types), register headers `reg_*.h`, `startup_mm32g0001_keil.s`, `system_mm32g0001.c` (clock init). Vendor code — don't modify.
2. `DRIVE/` — thin hardware drivers. `drv_pwm.c`: TIM1 complementary 6-channel PWM (16 kHz center-aligned, 1 µs dead-time), the six commutation phase functions (`UH_VL_HPwmLOn`, …), `Start_Motor`/`Stop_Motor`/`Brake_Motor`, `PwmDutyUpdate`. `drv_adc.c`: ADC1 scan over 8 channels, **externally triggered by TIM1_CC4 (dual edge)** so sampling is phase-locked to the PWM.
3. `MC_CORE/` — the sensorless 6-step engine as a **precompiled binary library** (`001mc_core_lib_V5.lib`); only `mc_core.h` is available, no source. Does BEMF zero-cross detection, IPD (initial position detection), open→closed-loop commutation, 60° timing. Configure it at runtime by filling a `McCore_Param_t` and calling `mc_core_list_init()`; drive it from ISRs via `mc_core_adc_isr_handle()` and `mc_core_timer14_isr_handle()`; read/write shared state through the global `mc_core_list`. You cannot change its internals — only its inputs.
4. `MOTOR_CONTROL/` — app-side motor layer wrapping the core. `motor_control.c`: `mc_init()` translates the macros in `motor_config.h` into `McCore_Param_t`; `MC_Machine_State()` is the startup state machine (`MC_StateMachine_e`: STOP → MOTION(query) → POSITION(IPD/align) → RUN(closed loop) → soft/hard brake); plus block/stall detection, current-offset calibration, MOS-short check. `motor_config.h`: commutation-timing / IPD / motion / brake tunables.
5. `USER/` — top application (see init + super-loop below).
6. `SYS/` — `systick.c` microsecond/millisecond delays.

## Real-time model — the control loop lives in ISRs, not `main()`

All hard-real-time work is in `USER/mm32g0001_it.c`:
- `TIM1_BRK_UP_TRG_COM_IRQHandler` (PWM update, 16 kHz): runs `MC_Machine_State()` + `motor_block_detect()`, divides down to a **1 ms tick** (`user_list.u8_1ms_flag`) and `LED_Scan()`; the TIM1 break input latches a hardware over-current fault.
- `ADC_IRQHandler` (EOC, once per PWM period): `Get_ADC_Result()` latches BEMF U/V/W, Vbus, I-peak, MOS-NTC, VR (speed pot) and trigger ADC into `mc_core_list.Board_ADC_DATA`; one-shot peak OCP; then `mc_core_adc_isr_handle()`.
- `TIM14_IRQHandler` (12 MHz): `mc_core_timer14_isr_handle()` — commutation timing.
- `EXTI4_15_IRQHandler` (EN pin PA4 falling edge): emergency PWM-off + fault latch.

`main()` (`USER/main.c`) is only a slow **1 ms super-loop** gated on `u8_1ms_flag`: voltage/temperature/over-current/overload handling, gears, tool state machine (`user_state_control`), LED/SOC, and setting `mc_core_list.motor_en` from the tool-enable flag.

Boot order in `main()` matters: `user_para_init` → `Systick_Init` → `fly_machine_init` (samples the trigger at power-on; if already pressed, sets the `fly_machine` fault to block a run-away start) → `Bsp_Gpio_Init` → `Peripheral_Init` (ADC/PWM/TIM14) → `mc_init` → `CurrentOffsetCalibration` → `mose_check` → `read_user_data` (restore gears/auto-stop from flash) → `Interrupt_Init` (enable NVIC) → voltage prime → loop.

## Shared state (globals, extern'd via headers)

- `mc_core_list` (`Mc_core_list`, `mc_core.h`) — core engine state, latched ADC results, `MC_error`.
- `motor_control_list` (`motor_control.h`) — motor-layer state machine, PWM duty targets.
- `user_list` (`User_list_Struct`, `user_control.h`) — tool state, flags (`gToolEn`/`gToolDir`/`gToolTrigger`/`gAutoStop`), gears, voltages.
- `machine_error_hold` (`Machine_ERROR_HOLD`) — latched faults (`UC_Error_u` user faults + `MC_Error_u` motor faults).

## Conventions & gotchas

- **Source comments are Chinese in GBK/GB2312 (CP936) encoding, not UTF-8.** They look like mojibake in a UTF-8 viewer — that is expected. Preserve the original encoding when editing a file; do **not** re-save as UTF-8 or the comments corrupt. Prefer edits that don't touch comment bytes.
- **All physical thresholds are macros in `USER/parameter.h` and `MOTOR_CONTROL/motor_config.h`**, written in real units (A, V, °C, µs) and converted to ADC counts / timer ticks via board constants (`RSHUNT` 1 mΩ, `AMPLIFICATION_GAIN` 10.89, `VBUS_SHUNT_RATIO` 11×, `CURRENT_ADC_PER_A`, NTC `RT_*` tables, 12-bit ADC @ 5 V ref). Tune behavior here — don't scatter magic numbers in `.c` files. These constants must match the actual board hardware. Protections cover multi-level average/peak OCP, UVP/OVP (≈18 V battery class: OVP 24.5 V, UVP 14.8/13.5/10 V), MOS over-temp, and stall.
- Flash parameters (gears, auto-stop enable) are stored by `USER/flash_data_save.c` in **flash page 15 (0x08003C00)** using a 10-block wear-leveling ring with a `0x55AA` magic marker.
- `USER/platform.c` is vendor sample scaffolding (console/`printf`, demo LEDs) and is largely unused by the application.
- Pin and ADC-channel assignments are centralized in `USER/board.h`.
