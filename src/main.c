#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
 
#include "gpio.h"
 
#define LOCK_FILE "/screenlock"
 
 
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
    printf("GPIO_Handler: Failed to map BCM2711 address");
    return -1;
  }

  //set  GPIO01 to input in function register
  *(gpio.addr + GPFSEL0) &= ~(1 << 3 | 1 << 4 | 1 << 5); // set bit 5:3 to 0 // clear bits for FSEL1 (GPIO01) // it is now Input

  // print content of GPFSEL0 register
  printf("GPIO_Handler: function reg  (%p):\t%#010x\n",gpio.addr + GPFSEL0, *(gpio.addr + GPFSEL0));

  // set pulldown resistor
  *(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) &= ~( 1 << 2 | 1 << 3); // Clear bits for GPIO01
  *(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) |= 0x2 << 2; // set bit 3:2 to 10 // Pull-donw resistor for GPIO01 (input)

  // print content of GPIO_PUP_PDN_CNTRL_REG0 register
  printf("GPIO_Handler: pull down reg (%p):\t%#010x\n",(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0) ,*(gpio.addr + GPIO_PUP_PDN_CNTRL_REG0));


  // Create event/interrupts for GPIO01

  *(gpio.addr + GPREN0) |= 1 << 1; // enable Rising edge trigger for GPIO01

  while(1)
  {
    // test for Event
    if(*(gpio.addr + GPEDS0))
    {
        printf("GPIO_Handler: Found event: 0b");
        printBin(*(gpio.addr + GPEDS0));
        printf("\n");
        if(*(gpio.addr + GPEDS0) & 0x2)
        if(access(LOCK_FILE, F_OK) == 0)
        {
            // file exists
            remove(LOCK_FILE);
            printf("GPIO_Handler: screenlock file deleted\n");
        }
        else
        {
            int fd = open(LOCK_FILE, O_WRONLY | O_CREAT | O_TRUNC ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            close(fd);
            printf("GPIO_Handler: screenlock file created\n");
        }
        sleep(1); // sleep to prevent multiple button presses
        *(gpio.addr + GPEDS0) = 1 << 1; // clear event
    }


    sleep(0.1);
  }
 
  return 0; 
}
