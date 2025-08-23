#include "bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	int num_slaves = 3;
	if (argc > 1) num_slaves = atoi(argv[1]);
	if (num_slaves <= 0 || num_slaves > MAX_SLAVES) num_slaves = 3;

	bus_t bus;
	if (bus_init(&bus, num_slaves) != 0) {
		fprintf(stderr, "Failed to init bus\n");
		return 1;
	}

	for (int s = 0; s < num_slaves; ++s) {
		char msg[64];
		snprintf(msg, sizeof(msg), "Hello from master to slave %d", s);
		int st = bus_write(&bus, s, 100, (const uint8_t *)msg, (uint32_t)strlen(msg)+1);
		printf("WRITE to slave %d -> status=%d\n", s, st);
	}

	for (int s = 0; s < num_slaves; ++s) {
		uint8_t buf[32] = {0};
		int st = bus_read(&bus, s, 100, buf, sizeof(buf));
		printf("READ from slave %d -> status=%d, data='%s'\n", s, st, (char *)buf);
	}

	// Bounds test
	uint8_t tmp[20];
	int st = bus_read(&bus, 0, SLAVE_MEM_SIZE-10, tmp, 20);
	printf("BOUNDS READ status=%d (expect -1)\n", st);

	bus_shutdown(&bus);
	bus_destroy(&bus);
	return 0;
}