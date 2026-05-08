
#include <stdbool.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

#define GET_PULSE_WIDTH_TICKS(load,duty)                   \
    ((uint32_t)((float)load*((float)duty/100.0f)))

struct pwm_frequency
{
    uint32_t ch0_1;
    uint32_t ch2_3;
};

struct pwm_duty
{
    float ch0;
    float ch1;
    float ch2;
    float ch3;
};

struct pwm_frequency frequency;
struct pwm_duty duty;

void getOptimalPwmSettings(uint32_t frequency, uint32_t* optimalLoad, uint32_t* optimalPrescaler, uint32_t* optimalPrescalerEnum)
{
    uint32_t sysFrequency;
    uint32_t pwmBaseFrequency;
    uint32_t load;

    double minError;
    double error;

    int i;

    const uint32_t prescaler_list[] = {1, 2, 4, 8, 16, 32, 64};
    const uint32_t prescalerEnum_list[] =
                {   SYSCTL_PWMDIV_1, SYSCTL_PWMDIV_2, SYSCTL_PWMDIV_4, SYSCTL_PWMDIV_8,
                    SYSCTL_PWMDIV_16, SYSCTL_PWMDIV_32, SYSCTL_PWMDIV_64 };

    sysFrequency = SysCtlClockGet();

    minError = 1000000000.0;

    for( i = 0 ; i < sizeof(prescaler_list)/sizeof(prescaler_list[0]) ; i++ )
    {
        pwmBaseFrequency = sysFrequency / prescaler_list[i];
        load = (pwmBaseFrequency / frequency);
        error = (double)frequency - (double)( (double)pwmBaseFrequency / (double)(load) );

        if(error < 0.0)
        {
            error = -error;
        }

        if( error < minError )
        {
            minError = error;
            *optimalLoad = load;
            *optimalPrescaler = prescaler_list[i];
            *optimalPrescalerEnum = prescalerEnum_list[i];
        }
    }
}

void setFrequencyAndDuty(uint32_t pwmChannel, uint32_t pwmFrequency, float pwmDuty)
{
    uint32_t load;
    uint32_t prescaler;
    uint32_t prescalerEnum;

    getOptimalPwmSettings(pwmFrequency, &load, &prescaler, &prescalerEnum);

    SysCtlPWMClockSet(prescalerEnum);

    if((pwmChannel == 0) || (pwmChannel == 1))
    {
        frequency.ch0_1 = pwmFrequency;
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, load);
    }
    else
    {
        frequency.ch2_3 = pwmFrequency;
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, load);
    }

    if(pwmChannel == 0)
    {
        duty.ch0 = pwmDuty;
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, GET_PULSE_WIDTH_TICKS(load, (duty.ch0)));
    }
    else if(pwmChannel == 1)
    {
        duty.ch1 = pwmDuty;
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, GET_PULSE_WIDTH_TICKS(load, (duty.ch1)));
    }
    else if( pwmChannel == 2 )
    {
        duty.ch2 = pwmDuty;
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, GET_PULSE_WIDTH_TICKS(load, (duty.ch2)));
    }
    else if( pwmChannel == 3 )
    {
        duty.ch3 = pwmDuty;
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, GET_PULSE_WIDTH_TICKS(load, (duty.ch3)));
    }
}

int main(void)
{
    SysCtlClockFreqSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ, 80000000U);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinConfigure(GPIO_PB7_M0PWM1);
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);

    frequency.ch0_1 = 10000U;
    frequency.ch2_3 = 10000U;

    duty.ch0 = 10.0f;
    duty.ch1 = 20.0f;
    duty.ch2 = 30.0f;
    duty.ch3 = 40.0f;

    PWMGenConfigure(PWM0_BASE,      \
                    PWM_GEN_0,      \
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenConfigure(PWM0_BASE,      \
                    PWM_GEN_1,      \
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    setFrequencyAndDuty(0, frequency.ch0_1, duty.ch0);
    setFrequencyAndDuty(1, frequency.ch0_1, duty.ch1);
    setFrequencyAndDuty(2, frequency.ch2_3, duty.ch2);
    setFrequencyAndDuty(3, frequency.ch2_3, duty.ch3);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
    PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

    while(1)
    {
        setFrequencyAndDuty(0, frequency.ch0_1, duty.ch0);
        setFrequencyAndDuty(1, frequency.ch0_1, duty.ch1);
        setFrequencyAndDuty(2, frequency.ch2_3, duty.ch2);
        setFrequencyAndDuty(3, frequency.ch2_3, duty.ch3);
    }

	return 0;
}
