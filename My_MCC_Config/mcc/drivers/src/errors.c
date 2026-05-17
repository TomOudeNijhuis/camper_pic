#include <xc.h>
#include "../errors.h"
#include "../power.h"
#include "../telemetry.h"

#define HYSTERESIS_SAMPLES        5U
#define HOUSEHOLD_SWITCH_TICKS  300U   /* ~30 s at 100 ms TMR0 cadence */
#define ADC_STALL_TICKS          50U   /* ~5 s */

#define VOLTAGE_STARTER_LOW_MV  11000U
#define VOLTAGE_MAINS_LOW_MV    11000U

#define WATER_LOW_THRESHOLD       25U  /* % */
#define WASTE_HIGH_THRESHOLD      75U
#define WASTE_FULL_THRESHOLD     100U

static volatile uint16_t errors_word;
static volatile uint16_t errors_last_reported;

static uint8_t hyst_household_low;
static uint8_t hyst_starter_low;
static uint8_t hyst_mains_low;
static uint8_t hyst_water_low;
static uint8_t hyst_waste_high;

static volatile uint8_t  household_switch_armed;
static volatile uint16_t household_switch_ticks;

static volatile uint16_t adc_stall_ticks;

static void errors_set_raw(uint16_t mask) {
    /* Save-and-restore GIE so this is safe when called from ISR context
     * (e.g. CRC failure or overrun from the EUSART RX ISR). Unconditionally
     * setting GIE=1 would re-enable interrupts mid-ISR and nest the stack. */
    uint8_t saved_gie = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    errors_word |= mask;
    INTCONbits.GIE = saved_gie;
}

void errors_init(void) {
    errors_word = 0;
    errors_last_reported = 0;

    hyst_household_low = 0;
    hyst_starter_low = 0;
    hyst_mains_low = 0;
    hyst_water_low = 0;
    hyst_waste_high = 0;

    household_switch_armed = 0;
    household_switch_ticks = 0;
    adc_stall_ticks = 0;

    /* Latch brown-out on boot. PCON0 bits are sticky across reset until
     * explicitly cleared by software; reading them tells us the reset cause. */
    if (PCON0bits.nBOR == 0) {
        errors_word |= ERR_BROWN_OUT;
        PCON0bits.nBOR = 1;
    }
}

void errors_set(uint16_t mask) {
    errors_set_raw(mask);
}

void errors_clear(uint16_t mask) {
    INTCONbits.GIE = 0;
    errors_word &= (uint16_t)~mask;
    /* Reset the rising-edge snapshot for cleared bits so a re-raise can
     * trigger a fresh OP_EVENT. */
    errors_last_reported &= (uint16_t)~mask;
    INTCONbits.GIE = 1;
}

uint16_t errors_get(void) {
    uint16_t v;
    INTCONbits.GIE = 0;
    v = errors_word;
    INTCONbits.GIE = 1;
    return v;
}

uint16_t errors_take_rising_edge(void) {
    uint16_t rising;
    INTCONbits.GIE = 0;
    rising = (uint16_t)(errors_word & ~errors_last_reported);
    errors_last_reported = errors_word;
    INTCONbits.GIE = 1;
    return rising;
}

void errors_arm_household_switch(void) {
    INTCONbits.GIE = 0;
    household_switch_armed = 1;
    household_switch_ticks = HOUSEHOLD_SWITCH_TICKS;
    INTCONbits.GIE = 1;
}

void errors_disarm_household_switch(void) {
    INTCONbits.GIE = 0;
    household_switch_armed = 0;
    INTCONbits.GIE = 1;
}

void errors_tick_isr(void) {
    if (household_switch_armed && household_switch_ticks > 0) {
        household_switch_ticks--;
    }
    if (adc_stall_ticks < ADC_STALL_TICKS) {
        adc_stall_ticks++;
    }
}

void errors_note_adc_fresh(void) {
    adc_stall_ticks = 0;
    /* Clear the *working* stall condition; the latched error bit stays set
     * until the host clears it. */
    INTCONbits.GIE = 0;
    /* nothing else to clear here */
    INTCONbits.GIE = 1;
}

static void update_rule(uint16_t err_bit, bool condition, uint8_t *counter) {
    if (condition) {
        if (*counter < HYSTERESIS_SAMPLES) {
            (*counter)++;
            if (*counter == HYSTERESIS_SAMPLES) {
                errors_set_raw(err_bit);
            }
        }
    } else {
        *counter = 0;
    }
}

void errors_run_detectors(void) {
    uint16_t v_household = getVoltage(V_HOUSEHOLD);
    uint16_t v_starter   = getVoltage(V_STARTER);
    uint16_t v_mains     = getVoltage(V_MAINS);
    uint8_t  water       = telemetry_get_water_cached();
    uint8_t  waste       = telemetry_get_waste_cached();
    household_state_t hh = getHouseholdState();

    update_rule(ERR_VOLTAGE_HOUSEHOLD_LOW,
                (v_household > 0) && (v_household < GEL_ERROR_TRESHOLD),
                &hyst_household_low);
    update_rule(ERR_VOLTAGE_STARTER_LOW,
                (v_starter > 0) && (v_starter < VOLTAGE_STARTER_LOW_MV),
                &hyst_starter_low);
    update_rule(ERR_VOLTAGE_MAINS_LOW,
                (hh == ON) && (v_mains > 0) && (v_mains < VOLTAGE_MAINS_LOW_MV),
                &hyst_mains_low);
    update_rule(ERR_WATER_LOW, (water <= WATER_LOW_THRESHOLD), &hyst_water_low);
    update_rule(ERR_WASTE_HIGH, (waste >= WASTE_HIGH_THRESHOLD), &hyst_waste_high);

    if (waste >= WASTE_FULL_THRESHOLD) {
        errors_set_raw(ERR_WASTE_FULL);
    }

    /* Household switch watchdog: getHouseholdState() now mirrors the
     * marker directly, so the real success signal is the voltage
     * actually rising above the ON threshold. If the relay never
     * confirms within ~30 s we pulse OFF to disengage and latch the
     * error so the host can show the failure. */
    if (household_switch_armed) {
        if (v_household > GEL_ACTIVE_TRESHOLD) {
            errors_disarm_household_switch();
        } else if (household_switch_ticks == 0) {
            setHousehold(0);
            errors_set_raw(ERR_HOUSEHOLD_SWITCH_FAILED);
            errors_disarm_household_switch();
        }
    }

    if (adc_stall_ticks >= ADC_STALL_TICKS) {
        errors_set_raw(ERR_ADC_STUCK);
    }
}
