#include "bus.h"
#include <stdlib.h>
#include <string.h>

static int validate_access(uint32_t address, uint32_t length) {
	uint64_t end = (uint64_t)address + (uint64_t)length;
	return end <= SLAVE_MEM_SIZE ? 0 : -1;
}

int bus_init(bus_t *bus, int num_slaves) {
	if (!bus || num_slaves < 1 || num_slaves > MAX_SLAVES) return -1;
	bus->num_slaves = num_slaves;
	for (int i = 0; i < num_slaves; ++i) {
		bus->slaves[i].id = i;
		bus->slaves[i].memory = (uint8_t *)calloc(SLAVE_MEM_SIZE, 1);
		if (!bus->slaves[i].memory) return -1;
	}
	return 0;
}

void bus_destroy(bus_t *bus) {
	if (!bus) return;
	for (int i = 0; i < bus->num_slaves; ++i) {
		free(bus->slaves[i].memory);
		bus->slaves[i].memory = NULL;
	}
}

static int check_slave(const bus_t *bus, int slave_id) {
	return slave_id >= 0 && slave_id < bus->num_slaves ? 0 : -1;
}

int bus_write(bus_t *bus, int slave_id, uint32_t address, const uint8_t *data, uint32_t length) {
	if (!bus || check_slave(bus, slave_id) != 0 || !data) return -1;
	if (validate_access(address, length) != 0) return -1;
	memcpy(bus->slaves[slave_id].memory + address, data, length);
	return 0;
}

int bus_read(bus_t *bus, int slave_id, uint32_t address, uint8_t *out, uint32_t length) {
	if (!bus || check_slave(bus, slave_id) != 0 || !out) return -1;
	if (validate_access(address, length) != 0) return -1;
	memcpy(out, bus->slaves[slave_id].memory + address, length);
	return 0;
}

int bus_shutdown(bus_t *bus) {
	(void)bus;
	return 0; // no-op in single-threaded version
}