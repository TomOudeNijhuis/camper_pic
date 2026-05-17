#include <xc.h>
#include "../adc_sampler.h"
#include "../errors.h"
#include "../../mcc_generated_files/adc/adc.h"
#include "../../mcc_generated_files/system/system.h"

#define FVRA4X_ADDR 0x811AU

typedef enum {
    SAMP_IDLE = 0,
    SAMP_HOUSEHOLD,
    SAMP_STARTER,
    SAMP_MAINS
} sampler_state_t;

static volatile sampler_state_t sampler_state;
static volatile uint16_t raw_counts[3];   /* indexed by voltage_t */
static volatile bool     samples_fresh;

static uint16_t voltage_mv_cache[3];

/* Integer voltage math.
 *
 * Original (floating-point): mV = raw * fvr1 / 1023 * (10000 + 3900) / 3900
 *
 * Rearranged for integer:    mV = (raw * fvr1 * 13900) / (1023 * 3900)
 *                               = (raw * fvr1 * 13900) / 3989700
 *
 * raw is 0..1023 (10 bits), fvr1 is ~3300 (mV at 4x FVR), so the product
 * raw * fvr1 fits in 32 bits comfortably. Multiplying by 13900 keeps us
 * in 32-bit range too: 1023 * 3300 * 13900 = 4.69e10, which overflows
 * uint32_t. Split: compute (raw * fvr1) in uint32, divide by 1023, then
 * multiply by 13900, then divide by 3900. Each intermediate stays in
 * uint32 range. No float library pulled in, no deep call chain. */
#define GAIN_NUM   13900UL    /* (10000 + 3900) */
#define GAIN_DEN    3900UL

static uint16_t configMemoryRead(uint16_t addr) {
    uint8_t gie = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    NVMADRL = (uint8_t)(addr & 0xFFU);
    NVMADRH = (uint8_t)(addr >> 8);
    NVMCON1bits.NVMREGS = 1;
    NVMCON1bits.RD = 1;
    NOP();
    NOP();
    uint16_t result = ((uint16_t)NVMDATH << 8) | NVMDATL;
    INTCONbits.GIE = gie;
    return result;
}

static void start_channel(sampler_state_t s) {
    adc_channel_t ch;
    switch (s) {
    case SAMP_HOUSEHOLD: ch = HOUSEHOLD; break;
    case SAMP_STARTER:   ch = STARTER;   break;
    case SAMP_MAINS:     ch = MAINS;     break;
    default:             return;
    }
    ADC_ChannelSelect(ch);
    ADC_ConversionStart();
}

/* ISR context: fires on ADC conversion-done. */
static void on_adc_done(void) {
    uint16_t result = (uint16_t)ADC_ConversionResultGet();
    switch (sampler_state) {
    case SAMP_HOUSEHOLD:
        raw_counts[V_HOUSEHOLD] = result;
        sampler_state = SAMP_STARTER;
        start_channel(SAMP_STARTER);
        break;
    case SAMP_STARTER:
        raw_counts[V_STARTER] = result;
        sampler_state = SAMP_MAINS;
        start_channel(SAMP_MAINS);
        break;
    case SAMP_MAINS:
        raw_counts[V_MAINS] = result;
        sampler_state = SAMP_IDLE;
        samples_fresh = true;
        break;
    default:
        sampler_state = SAMP_IDLE;
        break;
    }
}

void adc_sampler_init(void) {
    sampler_state = SAMP_IDLE;
    samples_fresh = false;
    voltage_mv_cache[0] = 0;
    voltage_mv_cache[1] = 0;
    voltage_mv_cache[2] = 0;

    ADC_ConversionDoneCallbackRegister(on_adc_done);
    ADC_ConversionDoneInterruptFlagClear();
    ADC_ConversionDoneInterruptEnable();
}

void adc_sampler_start_cycle(void) {
    if (sampler_state != SAMP_IDLE) {
        return;
    }
    sampler_state = SAMP_HOUSEHOLD;
    start_channel(SAMP_HOUSEHOLD);
}

bool adc_sampler_consume(void) {
    uint16_t snapshot[3];
    uint16_t fvr1_value;
    uint8_t i;

    INTCONbits.GIE = 0;
    if (!samples_fresh) {
        INTCONbits.GIE = 1;
        return false;
    }
    snapshot[0] = raw_counts[0];
    snapshot[1] = raw_counts[1];
    snapshot[2] = raw_counts[2];
    samples_fresh = false;
    INTCONbits.GIE = 1;

    fvr1_value = configMemoryRead(FVRA4X_ADDR);
    for (i = 0; i < 3; i++) {
        uint32_t step1 = ((uint32_t)snapshot[i] * (uint32_t)fvr1_value) / 1023UL;
        uint32_t mv    = (step1 * GAIN_NUM) / GAIN_DEN;
        voltage_mv_cache[i] = (uint16_t)mv;
    }

    errors_note_adc_fresh();
    return true;
}

uint16_t adc_sampler_get_cached(voltage_t ch) {
    if (ch < 3) {
        return voltage_mv_cache[ch];
    }
    return 0;
}
