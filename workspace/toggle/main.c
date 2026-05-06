
#include <stdbool.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void)
{
    /* Set system clock to 80 MHz. */

    SysCtlClockFreqSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ, 80000000U);

    /* Enable GPIO PORT E. */

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    /* Configure PE.1 as an output. */

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);

    /* Enable GPIO PORT F. */

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    /* For Port F, we require an unlock operation to be done before configuring it. */

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    /* Configure PF0 and PF4 as inputs */

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    /* Enable pull-up resistors on PF0 & PF4. */

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0, GPIO_PIN_TYPE_STD_WPU);

    while(1)
    {
        /* If PF0 is pressed, turn PE1 on. */

        if( GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0 )
        {
            GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,GPIO_PIN_1);
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0);
        }

        /* If PF4 is pressed, turn PE1 off. */

        if( GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0 )
        {
            GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,0);
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0);
        }
    }

	return 0;
}
