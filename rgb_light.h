#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <signal.h>

static const char *device = "/dev/spidev0.0";
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 0;

int fd;

static void pabort(const char *s);
static void transfer(int fd, uint8_t r_color, uint8_t g_color, uint8_t b_color);
void initialize_rgb();
void shutdown_rgb();

