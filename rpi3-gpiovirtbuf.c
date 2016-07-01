/*
 * rpi3-gpiovirtbuf.c -- Control Raspberry Pi 3's activity LED by using the Mailbox interface
 *
 * Copyright (c) 2016 Sugizaki Yukimasa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "raspberrypi-firmware.h"

#define RPI_FIRMWARE_DEV "/dev/vcio"
#define IOCTL_RPI_FIRMWARE_PROPERTY _IOWR(100, 0, char*)

#define DEV_MEM "/dev/mem"

#define BUS_TO_PHYS(addr) ((((addr)) & ~0xc0000000))

static void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s NUM\n", progname);
	fprintf(stderr, "Set NUM to 0 to turn off the activity LED and non-0 to turn it on.\n");
	fprintf(stderr, "This program requires root privilege to map memory.\n");
}

static int rpi_firmware_open()
{
	int fd;

	fd = open(RPI_FIRMWARE_DEV, O_NONBLOCK);
	if (fd == -1) {
		fprintf(stderr, "error: open: %s: %s\n", RPI_FIRMWARE_DEV, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return fd;
}

static void rpi_firmware_close(const int fd)
{
	int reti;

	reti = close(fd);
	if (reti == -1) {
		fprintf(stderr, "error: close: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void rpi_firmware_property(const int fd, const uint32_t tag, void *tag_data, const size_t buf_size)
{
	int i;
	uint32_t *p = NULL;
	int reti;

	p = malloc((5 + buf_size / 4 + 1) * sizeof(*p));
	if (p == NULL) {
		fprintf(stderr, "error: Failed to allocate memory for RPi firmware\n");
		exit(EXIT_FAILURE);
	}

	i = 0;
	p[i++] = (5 + buf_size / 4 + 1) * sizeof(*p);
	p[i++] = RPI_FIRMWARE_STATUS_REQUEST;
	p[i++] = tag; // tag
	p[i++] = buf_size; // buf_size
	p[i++] = 0; // req_resp_size
	memcpy(p + i, tag_data, buf_size);
	p[i + buf_size / 4] = RPI_FIRMWARE_PROPERTY_END;;

	reti = ioctl(fd, IOCTL_RPI_FIRMWARE_PROPERTY, p);
	if (reti == -1) {
		fprintf(stderr, "error: ioctl: IOCTL_RPI_FIRMWARE_PROPERTY: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (p[1] != RPI_FIRMWARE_STATUS_SUCCESS) {
		fprintf(stderr, "error: RPi firmware returned 0x%08x\n", p[1]);
		exit(EXIT_FAILURE);
	}

	memcpy(tag_data, p + i, buf_size);
}

void gpio_set(int mb, int gpio, int state)
{
	uint32_t gpio_set[2];
	gpio_set[0] = gpio;
	gpio_set[1] = state ? 1 : 0;
	rpi_firmware_property(mb, RPI_FIRMWARE_SET_GPIO_STATE, gpio_set, sizeof(gpio_set));
	return;
}

int main(int argc, char *argv[])
{
	int mb = -1;
	uint32_t gvp = NULL;
	unsigned *addr = NULL;
	unsigned off;
	int val;
	int i;

	if (argc < 2) {
		fprintf(stderr, "error: Invalid the number of the arguments\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	off = 0; /* 0 is for the activity LED. */
	val = atoi(argv[1]);

	mb = rpi_firmware_open();

	if(argc == 3) {
		fprintf(stderr, "Set state of %d to %d\n", val, atoi(argv[2]));
		gpio_set(mb, val, atoi(argv[2]));
	} else {
		for (i=0; i<10; i++) {
			fprintf(stderr, "On\n");
			gpio_set(mb, val, 1);
			sleep(1);
			fprintf(stderr, "Off\n");
			gpio_set(mb, val, 0);
			sleep(1);
		}
	}

	rpi_firmware_close(mb);
	mb = -1;

	return 0;
}
