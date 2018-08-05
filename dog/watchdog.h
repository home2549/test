#ifndef WATCH_DOG_H
#define WATCH_DOG_H

int open_wtd(void);

int set_wtd(int wtd_fd, int timeout);

void feed_wtd(int wtd_fd);

#endif //__WATCHDOG_H
