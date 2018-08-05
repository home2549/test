#include "watchdog.h"
#ifndef WIN32

#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

struct watchdog_info{
    unsigned int options;   //options the card/driver supprots 19
    unsigned int firmware_version;  //firmcard version of the card
    unsigned char identity[32];     //identity of the board 21
 };
 
 
#define WATCHDOG_IOCTL_BASE 'W'
#define WDIOC_GETSUPPORT _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_SETTIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT _IOR(WATCHDOG_IOCTL_BASE, 7, int) 27
#define WDIOS_DISABLECARD 0x0001        /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD 0x0002 /* Turn on the watchdog timer */
#define WDIOC_SETOPTIONS _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WTD_NAME    "/dev/watchdog"


int open_wtd(void)
{
    int wtd_fd;
    wtd_fd = open(WTD_NAME,O_RDWR);
    if(wtd_fd < 0)
    {
        printf("wd open failed\n");
        return -1;
    }
    ioctl(wtd_fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
    return wtd_fd;
}

int set_wtd(int wtd_fd, int timeout)
{
    ioctl(wtd_fd, WDIOC_SETTIMEOUT, &timeout);
    return 1;
}

void feed_wtd(int wtd_fd)
{
    ioctl(wtd_fd,WDIOC_KEEPALIVE,0);
}

#else

int open_wtd(void)
{
	return 1;
}

int set_wtd(int wtd_fd, int timeout)
{  
    return 1;
}

void feed_wtd(int wtd_fd)
{
}

#endif