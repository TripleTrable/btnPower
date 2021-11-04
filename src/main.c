#include <stdio.h>
 
#include "gpio.h"
 
 
 
struct bcm2711_peripheral gpio = {GPIO_BASE};


void printBin(uint32_t value)
{
    for (int i = 0; i < 32; i++)
    {
        printf("%c",value & 0x80000000 ? '1' : '0');
        value = value << 1;
    }
}

int main()
{

  // map physical/peripherals address GPIO_BASE into virtual memory
  if(map_peripheral(&gpio) == -1) 
  {
    printf("Failed to map BCM2711 address");
    return -1;
  }

  //HW pin 27 = BCM pin 0 = GPIO pin 0
  //set GPIO00 to output and GPIO01 to input in function register
  *(gpio.addr + GPFSEL0) &= ~(1 << 3 | 1 << 4 | 1 << 5); // set bit 5:3 to 0 // clear bits for FSEL1 (GPIO01) // it is now Input
  *(gpio.addr + GPFSEL0) &= ~(1 << 0 | 1 << 1 | 1 << 2); // set bit 2:0 to 0 // clear bits for FSEL0 (GPIO00) 
  *(gpio.addr + GPFSEL0) |= 1 << 0 ; // set FSEL0 to 001 (output config)

  // print content of GPFSEL0 register
  printf("function reg  (%p):\t%#010x\n",gpio.addr + GPFSEL0, *(gpio.addr + GPFSEL0));

  // set pulldown resistor
  *(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) &= ~( 1 << 0 | 1 << 1); // set not Resistor for GPIO00 (output)
  *(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) &= ~( 1 << 2 | 1 << 3); // Clear bits for GPIO01
  *(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) |= 0x2 << 2; // set bit 3:2 to 10 // Pull-donw resistor for GPIO01 (input)

  // print content of GPIO_PUP_PDN_CNTRL_REG0 register
  printf("pull down reg (%p):\t%#010x\n",(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) ,*(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0));


  // Create event/interrupts for GPIO01

  *(gpio.addr + GPREN0) |= 1 << 1; // enable Rising edge trigger for GPIO01

  while(1)
  {
    // set GPIO00 output to 1 // No need for |= because GPSET# does only set (in contrast to AVR microcontroller) and 0's are ignored
    *(gpio.addr + GPSET0) = 1;
    // print current logic level of all pins
    printf("On  after all clear: 0b");
    printBin(*(gpio.addr + GPLEV0));
    printf("\n");

    // test for Event
    if(*(gpio.addr + GPEDS0))
    {
        printf("Found event: 0b");
        printBin(*(gpio.addr + GPEDS0));
        printf("\n");
        *(gpio.addr + GPEDS0) = 1 << 1; // clear event
    }


    sleep(1);
 

    // set GPIO00 output to 0 (write 0 into GPCLR#) // 0's are ignored see comment aboce
    *(gpio.addr + GPCLR0) = 1 ;
    // print current logic level of all pins
    printf("Off after all clear: 0b");
    printBin(*(gpio.addr + GPLEV0));
    printf("\n");
    sleep(1);
  }
 
  return 0; 
}
