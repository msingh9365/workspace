#include "bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
	uint16_t crc = 0xFFFF;
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
		crc = crc16_ccitt(payload, length) ^ crc;
	}
	return crc;
}

static int master_write(bus_t *bus, int slave, uint32_t addr, const uint8_t *data, uint16_t len) {
	frame_header_t hdr = { .start = 0xAA, .slave_id = (uint8_t)slave, .op = OP_WRITE, .length = len, .address = addr, .crc16 = 0 };
	hdr.crc16 = crc_over_fields_and_payload(hdr.start, hdr.slave_id, hdr.op, hdr.length, hdr.address, data);
	return bus_transfer(bus, &hdr, data, NULL);
}

static int master_read(bus_t *bus, int slave, uint32_t addr, uint8_t *out, uint16_t len) {
	frame_header_t hdr = { .start = 0xAA, .slave_id = (uint8_t)slave, .op = OP_READ, .length = len, .address = addr, .crc16 = 0 };
	hdr.crc16 = crc_over_fields_and_payload(hdr.start, hdr.slave_id, hdr.op, hdr.length, hdr.address, NULL);
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