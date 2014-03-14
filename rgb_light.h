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
    uint8_t color[3];
} RGB_LIGHT_T;

void calculate_rgb(RGB_LIGHT_T *state);
void pabort(const char *s);
void transfer(RGB_LIGHT_T *state);
int initialize_rgb(RGB_LIGHT_T *state);
void shutdown_rgb(RGB_LIGHT_T *state);

#endif
