/*
 * Compile with gcc -Wall -Werror -o jetclocks_user jetclocks_user.c 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

int main() {
    char *clock = "pwm2";
    
    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev == -1) {
	printf("Opening was not possible!\n");
	return -1;
    }

    int x =0;
    x =ioctl(dev, CLK_ENABLE, clock);
    printf("enable was: %d\n", x);

    x =ioctl(dev, CLK_DISABLE, clock);
    printf("disable was: %d\n", x);

    close(dev);
    return 0;
}
