#ifndef RGB_LIGHT_H
#define RGB_LIGHT_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <signal.h>

struct {
    char *device;
    uint8_t mode;
    uint8_t bits;
    uint32_t speed;
    uint16_t delay;
    int fd;
} RGB_LIGHT_T;

static void pabort(const char *s);
static void transfer(int fd, uint8_t r_color, uint8_t g_color, uint8_t b_color);
int initialize_rgb();
void shutdown_rgb(int fd);

#endif
