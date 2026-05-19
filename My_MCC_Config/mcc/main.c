/*
 * Camper PIC application entry point.
 *
 * Layered firmware:
 *   - EUSART1 (MCC, interrupt-driven 64-byte FIFOs) feeds protocol.c
 *   - protocol.c assembles frames from the RX-complete ISR
 *   - commands.c dispatches opcodes from the main loop
 *   - adc_sampler.c walks ADC channels via the ADC done ISR
 *   - telemetry.c streams snapshots fast (1 s) when subscribed, slow
 *     (60 s) when idle; emits async OP_EVENT on error rising edges
 *   - errors.c maintains a sticky 16-bit error word with hysteresis
 *
 * Timer_Callback runs in ISR context and stays short: ticks the
 * heartbeat/scheduler counters, kicks the ADC sampler periodically,
 * and runs the neopixel blink runner. No ADC reads, no waiting.
 */

#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/system/system.h"
#include "drivers/neopixels.h"
#include "drivers/power.h"
#include "drivers/protocol.h"
#include "drivers/commands.h"
#include "drivers/adc_sampler.h"
#include "drivers/telemetry.h"
#include "drivers/errors.h"

#define STARTUP_OVERRULE_TIME   100U
#define ADC_SAMPLE_EVERY_TICKS    5U   /* TMR0 = ~100 ms; sample every ~500 ms */

static volatile uint8_t  adc_sample_divider;
static volatile bool     neopixel_tick_pending;

static void Timer_Callback(void) {
    neopixel_overrule_tick_isr();
    telemetry_tick_isr();
    errors_tick_isr();
    protocol_tick_isr();

    if (++adc_sample_divider >= ADC_SAMPLE_EVERY_TICKS) {
        adc_sample_divider = 0;
        adc_sampler_start_cycle();
    }

    /* Run the bit-banged WS2812 protocol from main, not from the ISR.
     * Each invocation takes ~180 us with GIE=0; doing it in main lets
     * the EUSART RX ISR preempt and keeps the 2-byte hardware FIFO
     * from overflowing during a UART burst. */
    neopixel_tick_pending = true;
}

int main(void) {
    uint8_t switch1_prev = 1;
    uint8_t switch2_prev = 1;
    protocol_frame_t frame;
    uint8_t nack_reason;

    SYSTEM_Initialize();
    powerInitialize();

    SWITCH1_SetDigitalInput();
    SWITCH2_SetDigitalInput();
    NEOPIXEL_SetDigitalOutput();
    INT_OUT_SetDigitalOutput();

    EUSART1_Enable();

    adc_sampler_init();
    protocol_init();
    errors_init();
    telemetry_init();

    TMR0_PeriodMatchCallbackRegister(Timer_Callback);

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    neopixel_overrule(STARTUP_OVERRULE_TIME);
    neopixel_setcolor(1, TEAL, 5);
    neopixel_setcolor(2, TEAL, 5);

    /* Bit-bang the captured PCON0 byte on INT_OUT so the scope can read it
     * even if the UART event never makes it out. Scheme (MSB first):
     *   - 50 ms HIGH start pulse
     *   - 10 ms LOW gap
     *   - per bit: HIGH for 5 ms (=0) or 25 ms (=1), then 10 ms LOW gap
     *   - 50 ms HIGH end pulse
     * A '1' bit is visibly ~5x wider than a '0'. */
    {
        uint8_t pcon = errors_get_boot_pcon0();
        int8_t i;
        INT_OUT_LAT = 1; __delay_ms(50); INT_OUT_LAT = 0; __delay_ms(10);
        for (i = 7; i >= 0; i--) {
            INT_OUT_LAT = 1;
            if ((pcon >> i) & 1U) {
                __delay_ms(25);
            } else {
                __delay_ms(5);
            }
            INT_OUT_LAT = 0;
            __delay_ms(10);
        }
        INT_OUT_LAT = 1; __delay_ms(50); INT_OUT_LAT = 0;
    }

    while (1) {
        if (protocol_poll(&frame)) {
            commands_dispatch(&frame);
        }

        if (protocol_take_pending_nack(&nack_reason)) {
            protocol_send_nack(0xFF, nack_reason);
            if (nack_reason == NACK_BAD_CRC) {
                errors_set(ERR_PROTOCOL_CRC);
            }
        }

        if (protocol_take_pending_overrun()) {
            errors_set(ERR_PROTOCOL_OVERRUN);
        }

        if (SWITCH1_PORT == 0 && switch1_prev == 1) {
            switch1_prev = 0;
            setPump(getPumpState() ? 0 : 1);
        } else if (SWITCH1_PORT == 1) {
            switch1_prev = 1;
        }

        if (SWITCH2_PORT == 0 && switch2_prev == 1) {
            switch2_prev = 0;
            if (getHouseholdState() == OFF) {
                setHousehold(1);
                errors_arm_household_switch();
            } else {
                setHousehold(0);
                errors_disarm_household_switch();
            }
        } else if (SWITCH2_PORT == 1) {
            switch2_prev = 1;
        }

        adc_sampler_consume();
        errors_run_detectors();

        if (neopixel_tick_pending) {
            neopixel_tick_pending = false;
            neopixel_runner();
        }

        if (!neopixel_is_overruled()) {
            neopixel_setcolor(1, (getHouseholdState() == ON) ? GREEN : BLACK, 0);
            neopixel_setcolor(2, getPumpState() ? GREEN : BLACK, 0);
        }

        telemetry_run();
    }
}
