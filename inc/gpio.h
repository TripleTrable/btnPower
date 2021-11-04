#ifndef GPIO_H
#define GPIO_H


#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// bcm2711 Address
#define BCM2711_BASE 0x0FE000000
#define GPIO_BASE (BCM2711_BASE + 0x000200000)
 
#define BLOCK_SIZE (0x01800000)


// Register offset based on uint32_t pointer
#define GPFSEL0 0 //GPIO Function Select 0
#define GPFSEL1 1 //GPIO Function Select 1
#define GPFSEL2 2 //GPIO Function Select 2
#define GPFSEL3 3 //GPIO Function Select 3
#define GPFSEL4 4 //GPIO Function Select 4
#define GPFSEL5 5 //GPIO Function Select 5
#define GPSET0 7 //GPIO Pin Output Set 0
#define GPSET1 8 //GPIO Pin Output Set 1
#define GPCLR0 10 //GPIO Pin Output Clear 0
#define GPCLR1 11 //GPIO Pin Output Clear 1
#define GPLEV0 13 //GPIO Pin Level 0
#define GPLEV1 14 //GPIO Pin Level 1
#define GPEDS0 16 //GPIO Pin Event Detect Status 0
#define GPEDS1 17 //GPIO Pin Event Detect Status 1
#define GPREN0 19 //GPIO Pin Rising Edge Detect Enable 0
#define GPREN1 20 //GPIO Pin Rising Edge Detect Enable 1
#define GPFEN0 22 //GPIO Pin Falling Edge Detect Enable 0
#define GPFEN1 23 //GPIO Pin Falling Edge Detect Enable 1
#define GPHEN0 25 //GPIO Pin High Detect Enable 0
#define GPHEN1 26 //GPIO Pin High Detect Enable 1
#define GPLEN0 28 //GPIO Pin Low Detect Enable 0
#define GPLEN1 29 //GPIO Pin Low Detect Enable 1
#define GPAREN0 31 //GPIO Pin Async. Rising Edge Detect 0
#define GPAREN1 32 //GPIO Pin Async. Rising Edge Detect 1
#define GPAFEN0 34 //GPIO Pin Async. Falling Edge Detect 0
#define GPAFEN1 35 //GPIO Pin Async. Falling Edge Detect 1
#define GPIO_PUP_PDN_CNTRL_REG0 57 //GPIO Pull-up / Pull-down Register 0
#define GPIO_PUP_PDN_CNTRL_REG1 58 //GPIO Pull-up / Pull-down Register 1
#define GPIO_PUP_PDN_CNTRL_REG2 59 //GPIO Pull-up / Pull-down Register 2
#define GPIO_PUP_PDN_CNTRL_REG3 60 //GPIO Pull-up / Pull-down Register 3


// Peripheral struct
struct bcm2711_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile uint32_t *addr;
};
// Map  physical address defined in structure (p->addr_p)
int map_peripheral(struct bcm2711_peripheral  *p)
{
    printf("sizeof(unsigned long): %ld",sizeof(unsigned long));
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) 
   {
      printf("Failed to open /dev/mem.\n");
      return -1;
   }
 
   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,      // File descriptor to '/dev/mem'
      p->addr_p       // Physical address to expose inside /dev/mem
   );
 
   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }
 
   p->addr = (volatile uint32_t*)p->map;
 
   return 0;
}

void unmap_peripheral(struct bcm2711_peripheral *p) {
 
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}


#endif
