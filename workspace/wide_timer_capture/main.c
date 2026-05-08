
#include <stdbool.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_sysctl.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

struct pwm_timer_config
{
    const uint32_t channel;
    const uint32_t base;
    const uint32_t subtimer;
    const uint32_t gpioPinNumber;
    const uint32_t gpioPinFunction;
    const uint32_t gpioSysCtlPeripheralId;
    const uint32_t sysctlPeripheralId;
    const uint32_t gpioBase;
};

struct pwm_timer_data
{
    uint32_t frequency;
    float duty;
    uint64_t load;
    uint64_t match;
    uint32_t load_low;
    uint32_t load_high;
    uint32_t match_low;
    uint32_t match_high;
};

struct pwm_timer
{
    const struct pwm_timer_config* config;
    struct pwm_timer_data* data;
};

void pwmTimer_init(struct pwm_timer* timer)
{
    if( !SysCtlPeripheralReady(timer->config->gpioSysCtlPeripheralId) )
    {
        SysCtlPeripheralEnable(timer->config->gpioSysCtlPeripheralId);
    }

    if( !SysCtlPeripheralReady(timer->config->sysctlPeripheralId) )
    {
        SysCtlPeripheralEnable(timer->config->sysctlPeripheralId);
    }

    GPIOPinConfigure(timer->config->gpioPinFunction);

    GPIOPinTypeTimer(timer->config->gpioBase, timer->config->gpioPinNumber);

    TimerClockSourceSet(timer->config->base, TIMER_CLOCK_SYSTEM);

    TimerConfigure(timer->config->base, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM );
}

void pwmTimer_enable(struct pwm_timer* timer)
{
    TimerEnable(timer->config->base, timer->config->subtimer);
}

void pwmTimer_disable(struct pwm_timer* timer)
{
    TimerDisable(timer->config->base, timer->config->subtimer);
}

void pwmTimer_setFrequency(struct pwm_timer* timer, uint32_t frequency)
{
    uint64_t load;
    uint64_t match;
    uint64_t sysFrequency;

    sysFrequency = 80000000UL;

    timer->data->frequency = frequency;

    load = (uint64_t)(((double)sysFrequency/(double)timer->data->frequency) - 1.0);

    match = (uint64_t)((double)load - ( (double)load * (double)timer->data->duty ));

    timer->data->load = load;

    timer->data->match = match;

    timer->data->load_low = (load & 0xFFFFFFFFULL);
    timer->data->load_high = ( load >> 32 ) & 0xFFFFULL;
    timer->data->match_low = (match & 0xFFFFFFFFULL);
    timer->data->match_high = ( match >> 32 ) & 0xFFFFULL;

    TimerLoadSet(timer->config->base , timer->config->subtimer, (uint32_t)(load & 0xFFFFFFFFULL));

    TimerPrescaleSet(timer->config->base , timer->config->subtimer, (uint32_t)((load >> 32) & 0xFFFFULL));

    TimerMatchSet(timer->config->base, timer->config->subtimer, (uint32_t)(match&0xFFFFFFFFULL));

    TimerPrescaleMatchSet(timer->config->base, timer->config->subtimer, (uint32_t)((match >> 32) & 0xFFFFULL));
}

void pwmTimer_setDuty(struct pwm_timer* timer, float duty)
{
    uint64_t load;
    uint64_t match;
    uint64_t sysFrequency;

    sysFrequency = 80000000ULL;

    timer->data->duty = duty;

    load = (uint64_t)(((double)sysFrequency/(double)timer->data->frequency) - 1.0);

    match = (uint64_t)((double)load - ( (double)load * (double)timer->data->duty ));

    timer->data->load = load;

    timer->data->match = match;

    TimerLoadSet(timer->config->base , timer->config->subtimer, (uint32_t)(load & 0xFFFFFFFFULL));

    TimerPrescaleSet(timer->config->base , timer->config->subtimer, (uint32_t)((load >> 32) & 0xFFFFULL));

    TimerMatchSet(timer->config->base, timer->config->subtimer, (uint32_t)(match&0xFFFFFFFFULL));

    TimerPrescaleMatchSet(timer->config->base, timer->config->subtimer, (uint32_t)((match >> 32) & 0xFFFFULL));
}

const struct pwm_timer_config timer_1_config = { .base = WTIMER0_BASE,
                                           .channel = 0,
                                           .gpioPinFunction = GPIO_PC4_WT0CCP0,
                                           .gpioPinNumber = GPIO_PIN_4,
                                           .subtimer = TIMER_A,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_WTIMER0,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOC,
                                           .gpioBase = GPIO_PORTC_BASE, };

const struct pwm_timer_config timer_2_config = { .base = WTIMER0_BASE,
                                           .channel = 1,
                                           .gpioPinFunction = GPIO_PC5_WT0CCP1,
                                           .gpioPinNumber = GPIO_PIN_5,
                                           .subtimer = TIMER_B,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_WTIMER0,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOC,
                                           .gpioBase = GPIO_PORTC_BASE, };

const struct pwm_timer_config timer_3_config = { .base = WTIMER1_BASE,
                                           .channel = 2,
                                           .gpioPinFunction = GPIO_PC6_WT1CCP0,
                                           .gpioPinNumber = GPIO_PIN_6,
                                           .subtimer = TIMER_A,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_WTIMER1,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOC,
                                           .gpioBase = GPIO_PORTC_BASE, };

const struct pwm_timer_config timer_4_config = { .base = WTIMER1_BASE,
                                           .channel = 0,
                                           .gpioPinFunction = GPIO_PC7_WT1CCP1,
                                           .gpioPinNumber = GPIO_PIN_7,
                                           .subtimer = TIMER_B,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_WTIMER1,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOC,
                                           .gpioBase = GPIO_PORTC_BASE, };

volatile uint32_t risingEdge;
volatile uint32_t previousRisingEdge;
volatile uint32_t fallingEdge;
volatile uint32_t edgeCaptureState = 0;
volatile uint32_t capturePeriod;
volatile uint32_t captureDuty;

void wtimer3ISR()
{
    uint32_t intStatus;
    uint32_t timestamp;

    intStatus = TimerIntStatus(WTIMER3_BASE, true);
    TimerIntClear(WTIMER3_BASE, intStatus);
    timestamp = TimerValueGet(WTIMER3_BASE, TIMER_A);

    if( intStatus & TIMER_CAPA_EVENT )
    {
        if( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) )
        {
            risingEdge = timestamp;
            edgeCaptureState = 1;
        }
        else
        {
            fallingEdge = timestamp;
            edgeCaptureState = 2;
        }

        if(edgeCaptureState == 2)
        {
            captureDuty = risingEdge - fallingEdge;

            capturePeriod =  previousRisingEdge - risingEdge;

            edgeCaptureState = 0;

            previousRisingEdge = risingEdge;
        }
    }
}

struct pwm_timer_data timer_1_data;
struct pwm_timer_data timer_2_data;
struct pwm_timer_data timer_3_data;
struct pwm_timer_data timer_4_data;

struct pwm_timer timer_1 = { .config = &timer_1_config, .data = &timer_1_data, };
struct pwm_timer timer_2 = { .config = &timer_2_config, .data = &timer_2_data, };
struct pwm_timer timer_3 = { .config = &timer_3_config, .data = &timer_3_data, };
struct pwm_timer timer_4 = { .config = &timer_4_config, .data = &timer_4_data, };

uint32_t frequency[4];
float duty[4];

int main(void)
{
    SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_2_5 );

    pwmTimer_init(&timer_1);
    pwmTimer_init(&timer_2);
    pwmTimer_init(&timer_3);
    pwmTimer_init(&timer_4);

    pwmTimer_disable(&timer_1);
    pwmTimer_setFrequency(&timer_1, 12750U);
    pwmTimer_setDuty(&timer_1, 0.1267f);
    pwmTimer_enable(&timer_1);

    pwmTimer_disable(&timer_2);
    pwmTimer_setFrequency(&timer_2, 37500U);
    pwmTimer_setDuty(&timer_2, 0.45f);
    pwmTimer_enable(&timer_2);

    pwmTimer_disable(&timer_3);
    pwmTimer_setFrequency(&timer_3, 75000U);
    pwmTimer_setDuty(&timer_3, 0.90f);
    pwmTimer_enable(&timer_3);

    pwmTimer_disable(&timer_4);
    pwmTimer_setFrequency(&timer_4, 8000U);
    pwmTimer_setDuty(&timer_4, 0.25f);
    pwmTimer_enable(&timer_4);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER3));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

    GPIOPinConfigure(GPIO_PD2_WT3CCP0);

    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_2);

    TimerConfigure(WTIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME);

    TimerControlEvent(WTIMER3_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);

    TimerLoadSet(WTIMER3_BASE, TIMER_A, 0xFFFFFFFFU);

    TimerIntEnable(WTIMER3_BASE, TIMER_CAPA_EVENT);

    TimerEnable(WTIMER3_BASE, TIMER_A);

    IntRegister(INT_WTIMER3A, wtimer3ISR);

    IntEnable(INT_WTIMER3A);

    IntMasterEnable();

    frequency[0] = 10000;
    frequency[1] = 20000;
    frequency[2] = 30000;
    frequency[3] = 40000;

    duty[0] = 0.25f;
    duty[1] = 0.5f;
    duty[2] = 0.65f;
    duty[3] = 0.85f;

    while(1)
    {
        pwmTimer_setFrequency(&timer_1, frequency[0]);
        pwmTimer_setDuty(&timer_1, duty[0]);
        pwmTimer_setFrequency(&timer_2, frequency[1]);
        pwmTimer_setDuty(&timer_2, duty[1]);
        pwmTimer_setFrequency(&timer_3, frequency[2]);
        pwmTimer_setDuty(&timer_3, duty[2]);
        pwmTimer_setFrequency(&timer_4, frequency[3]);
        pwmTimer_setDuty(&timer_4, duty[3]);
        SysCtlDelay(80000000/3);
    }
}
