/* testgpio.c
 *
 * Raspberry Pi GPIO test program using sysfs interface.
 * Adapted from Guillermo A. Amaral B. <g@maral.me> blink.c
 * Dr. Pedro E. Colla (LU7DID)
 * This file blinks GPIO 27.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1
#define VALUE_MAX 30

//#define PIN  24 /* P1-18 */
#define POUT 27  /* P1-07 */

// GPIO Export
// Makes "pin" GPIO available

static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

// GPIOUnExport
// Remove availability of "pin" 

static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

// GPIODirection
// Establish whether the pin would be set as input (0) or output(1)

static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	return(0);
}
/*
// GPIO Read
// Used to read the value of pin (either 0 or 1)

static int GPIORead(int pin)
{
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}

	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}
*/
// GPIOWrite
// Used to establish the output value of a pin

static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}

	close(fd);
	return(0);
}

//----------------------------------------------------------------------
// main
//
// Execute the program by cycling 10 times ON/OFF the GPIO 27 pin
//----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int repeat = 10;

	/*
	 * Enable GPIO 27 pin
	 */
	if (-1 == GPIOExport(POUT))
		return(1);

	/*
	 * Set GPIO directions as OUTPUT
	 */
	if (-1 == GPIODirection(POUT, OUT))
		return(2);

        /*
         * Cycle on/off 10 times
         */

	do {
		/*
		 * Write GPIO value
		 */
		if (-1 == GPIOWrite(POUT, repeat % 2))
			return(3);
                fprintf(stderr, "PTT GPIO27 pin set to (%d)\n",repeat % 2);

		/*
		 * Read GPIO value
		 */
		//printf("I'm reading %d in GPIO %d\n", GPIORead(PIN), PIN);

		usleep(500 * 1000);
	}
	while (repeat--);

	/*
	 * Disable GPIO 27 pin
	 */

	if (-1 == GPIOUnexport(POUT))
		return(4);

	return(0);
}
