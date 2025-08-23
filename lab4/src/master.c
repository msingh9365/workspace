#include "bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t checksum_calc_fields(uint8_t start, uint8_t slave_id, uint8_t op, uint16_t length, uint32_t address, const uint8_t *payload) {
	uint8_t sum = 0;
	sum ^= start;
	sum ^= slave_id;
	sum ^= op;
	sum ^= (uint8_t)(length & 0xFF);
	sum ^= (uint8_t)((length >> 8) & 0xFF);
	sum ^= (uint8_t)(address & 0xFF);
	sum ^= (uint8_t)((address >> 8) & 0xFF);
	sum ^= (uint8_t)((address >> 16) & 0xFF);
	sum ^= (uint8_t)((address >> 24) & 0xFF);
	if (op == OP_WRITE && payload) {
		for (uint16_t i = 0; i < length; ++i) sum ^= payload[i];
	}
	return sum;
}

static int master_write(bus_t *bus, int slave, uint32_t addr, const uint8_t *data, uint16_t len) {
	frame_header_t hdr = { .start = 0xAA, .slave_id = (uint8_t)slave, .op = OP_WRITE, .length = len, .address = addr, .checksum = 0 };
	hdr.checksum = checksum_calc_fields(hdr.start, hdr.slave_id, hdr.op, hdr.length, hdr.address, data);
	return bus_transfer(bus, &hdr, data, NULL);
}

static int master_read(bus_t *bus, int slave, uint32_t addr, uint8_t *out, uint16_t len) {
	frame_header_t hdr = { .start = 0xAA, .slave_id = (uint8_t)slave, .op = OP_READ, .length = len, .address = addr, .checksum = 0 };
	hdr.checksum = checksum_calc_fields(hdr.start, hdr.slave_id, hdr.op, hdr.length, hdr.address, NULL);
	return bus_transfer(bus, &hdr, NULL, out);
}

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
		int st = master_write(&bus, s, 100, (const uint8_t *)msg, (uint16_t)(strlen(msg)+1));
		printf("WRITE to slave %d -> status=%d\n", s, st);
	}

	for (int s = 0; s < num_slaves; ++s) {
		uint8_t buf[32] = {0};
		int st = master_read(&bus, s, 100, buf, sizeof(buf));
		printf("READ from slave %d -> status=%d, data='%s'\n", s, st, (char *)buf);
	}

	uint8_t tmp[20];
	int st = master_read(&bus, 0, SLAVE_MEM_SIZE-10, tmp, 20);
	printf("BOUNDS READ status=%d (expect -1)\n", st);

	bus_destroy(&bus);
	return 0;
}