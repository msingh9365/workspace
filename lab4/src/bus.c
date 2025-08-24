#include "bus.h"
#include <string.h>

static uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
	uint16_t crc = 0xFFFF; // initial value
	for (size_t i = 0; i < len; ++i) {
		crc ^= (uint16_t)data[i] << 8;
		for (int b = 0; b < 8; ++b) {
			if (crc & 0x8000) crc = (crc << 1) ^ 0x1021; else crc <<= 1;
		}
	}
	return crc;
}

static uint16_t crc_over_fields_and_payload(uint8_t start, uint8_t slave_id, uint8_t op, uint16_t length, uint32_t address, const uint8_t *payload) {
	uint8_t buf[1 + 1 + 1 + 2 + 4];
	buf[0] = start;
	buf[1] = slave_id;
	buf[2] = op;
	buf[3] = (uint8_t)(length & 0xFF);
	buf[4] = (uint8_t)((length >> 8) & 0xFF);
	buf[5] = (uint8_t)(address & 0xFF);
	buf[6] = (uint8_t)((address >> 8) & 0xFF);
	buf[7] = (uint8_t)((address >> 16) & 0xFF);
	buf[8] = (uint8_t)((address >> 24) & 0xFF);
	uint16_t crc = crc16_ccitt(buf, sizeof(buf));
	if (op == OP_WRITE && payload && length > 0) {
		crc = crc16_ccitt(payload, length) ^ crc; // combine; order-insensitive is fine here
	}
	return crc;
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
	uint16_t calc = crc_over_fields_and_payload(hdr->start, hdr->slave_id, hdr->op, hdr->length, hdr->address, payload);
	if (calc != hdr->crc16) return -1;
	return slave_handle_frame(&bus->slaves[hdr->slave_id], hdr, payload, out_buf);
}