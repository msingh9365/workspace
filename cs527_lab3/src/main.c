#include <stdio.h>
#include <string.h>
#include "app.h"

static void print_bytes(const char *label, const uint8_t b[4])
{
	printf("%s: %02X %02X %02X %02X\n", label, b[0], b[1], b[2], b[3]);
}

int main(void)
{
	Device dev;
	device_init(&dev, 0x00);
	Bus bus;
	bus_init(&bus, &dev);

	uint8_t addr = 10; // example 7-bit address
	uint8_t to_write[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
	uint8_t read_back[4] = { 0 };

	int rc = app_write_4bytes(&bus, addr, to_write);
	if (rc != 0) {
		printf("Write failed with code %d\n", rc);
		return 1;
	}

	rc = app_read_4bytes(&bus, addr, read_back);
	if (rc != 0) {
		printf("Read failed with code %d\n", rc);
		return 1;
	}

	print_bytes("Wrote", to_write);
	print_bytes("Read ", read_back);

	int ok = memcmp(to_write, read_back, sizeof(to_write)) == 0;
	printf("Verification: %s\n", ok ? "SUCCESS" : "MISMATCH");
	return ok ? 0 : 2;
}