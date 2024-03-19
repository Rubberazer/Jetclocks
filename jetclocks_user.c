/*
 * Compile with gcc -Wall -Werror -o jetclocks_user jetclocks_user.c 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

struct jetclk clock = {0};

int main() {

    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev == -1) {
	printf("Opening /dev/jetclocks not possible\n");
	return -1;
    }

    /* Enabling clock "spi1" */
    
    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    ioctl(dev, CLK_DISABLE, &clock);
    printf("Enabling clock %s: \n",clock.clk);

    /* Now checking whether the clok is enabled*/

    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    ioctl(dev, CLK_IS_ENABLED, &clock);
    printf("clock %s status(0 disabled, 1 enabled): %d\n",clock.clk, clock.clk_enabled);
 
    close(dev);
    return 0;
}
