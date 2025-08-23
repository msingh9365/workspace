#include "bus.h"
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

int bus_init(bus_t *bus, int num_slaves) {
	if (!bus || num_slaves < 1 || num_slaves > MAX_SLAVES) return -1;
	bus->num_slaves = num_slaves;
	for (int i = 0; i < num_slaves; ++i) {
		slave_init(&bus->slaves[i], i);
	}
	return 0;
}

void bus_destroy(bus_t *bus) {
	if (!bus) return;
	for (int i = 0; i < bus->num_slaves; ++i) slave_destroy(&bus->slaves[i]);
}

int bus_transfer(bus_t *bus, const frame_header_t *hdr, const uint8_t *payload, uint8_t *out_buf) {
	if (!bus || !hdr) return -1;
	if (hdr->start != 0xAA) return -1;
	if (hdr->slave_id >= bus->num_slaves) return -1;
	uint8_t calc = checksum_calc_fields(hdr->start, hdr->slave_id, hdr->op, hdr->length, hdr->address, payload);
	if (calc != hdr->checksum) return -1;
	return slave_handle_frame(&bus->slaves[hdr->slave_id], hdr, payload, out_buf);
}