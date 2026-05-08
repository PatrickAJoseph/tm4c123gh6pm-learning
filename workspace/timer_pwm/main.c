
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
    uint32_t load;
    uint32_t match;
    uint32_t sysFrequency;

    sysFrequency = 80000000UL;

    timer->data->frequency = frequency;

    load = (uint32_t)(((double)sysFrequency/(double)timer->data->frequency) - 1.0);

    match = (uint32_t)((double)load - ( (double)load * (double)timer->data->duty ));

    TimerLoadSet(timer->config->base , timer->config->subtimer, (load & 0xFFFF));

    TimerPrescaleSet(timer->config->base , timer->config->subtimer, (load >> 16) & 0xFF);

    TimerMatchSet(timer->config->base, timer->config->subtimer, match&0xFFFF);

    TimerPrescaleMatchSet(timer->config->base, timer->config->subtimer, (match >> 16) & 0xFF);
}

void pwmTimer_setDuty(struct pwm_timer* timer, float duty)
{
    uint32_t load;
    uint32_t match;
    uint32_t sysFrequency;

    sysFrequency = 80000000U;

    timer->data->duty = duty;

    load = (uint32_t)(((double)sysFrequency/(double)timer->data->frequency) - 1.0);

    match = (uint32_t)((double)load - ( (double)load * (double)timer->data->duty ));

    TimerLoadSet(timer->config->base , timer->config->subtimer, (load & 0xFFFF));

    TimerPrescaleSet(timer->config->base , timer->config->subtimer, (load >> 16) & 0xFF);

    TimerMatchSet(timer->config->base, timer->config->subtimer, match&0xFFFF);

    TimerPrescaleMatchSet(timer->config->base, timer->config->subtimer, (match >> 16) & 0xFF);
}

const struct pwm_timer_config timer_1_config = { .base = TIMER0_BASE,
                                           .channel = 0,
                                           .gpioPinFunction = GPIO_PB6_T0CCP0,
                                           .gpioPinNumber = GPIO_PIN_6,
                                           .subtimer = TIMER_A,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_TIMER0,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOB,
                                           .gpioBase = GPIO_PORTB_BASE, };

const struct pwm_timer_config timer_2_config = { .base = TIMER0_BASE,
                                           .channel = 1,
                                           .gpioPinFunction = GPIO_PB7_T0CCP1,
                                           .gpioPinNumber = GPIO_PIN_7,
                                           .subtimer = TIMER_B,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_TIMER0,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOB,
                                           .gpioBase = GPIO_PORTB_BASE, };

const struct pwm_timer_config timer_3_config = { .base = TIMER1_BASE,
                                           .channel = 2,
                                           .gpioPinFunction = GPIO_PB4_T1CCP0,
                                           .gpioPinNumber = GPIO_PIN_4,
                                           .subtimer = TIMER_A,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_TIMER1,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOB,
                                           .gpioBase = GPIO_PORTB_BASE, };

const struct pwm_timer_config timer_4_config = { .base = TIMER1_BASE,
                                           .channel = 0,
                                           .gpioPinFunction = GPIO_PB5_T1CCP1,
                                           .gpioPinNumber = GPIO_PIN_5,
                                           .subtimer = TIMER_B,
                                           .sysctlPeripheralId = SYSCTL_PERIPH_TIMER1,
                                           .gpioSysCtlPeripheralId = SYSCTL_PERIPH_GPIOB,
                                           .gpioBase = GPIO_PORTB_BASE, };

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
