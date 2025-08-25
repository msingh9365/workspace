#include "app.h"
#include "protocol.h"
#include <stdio.h>

static void app_store_byte_cb(size_t index0to3, uint8_t data_byte, void *user_ctx)
{
	uint8_t *buf = (uint8_t*)user_ctx;
	buf[index0to3] = data_byte;
}

static int app_start_and_address(Bus *bus, uint8_t address7)
{
	// Step 1-2: start then ack
	int attempts = 0;
	while (attempts < 3) {
		if (bus_send_start(bus)) break;
		attempts++;
	}
	if (attempts == 3) return -1;

	// Step 3-5: send address+parity until ack
	uint8_t payload = protocol_pack_address(address7);
	attempts = 0;
	while (attempts < 5) {
		if (bus_send_address(bus, payload)) return 0; // ack
		attempts++;
	}
	return -2; // address phase failed
}

int app_write_4bytes(Bus *bus, uint8_t address7, const uint8_t data[4])
{
	if (!bus || !data) return -10;
	int rc = app_start_and_address(bus, address7);
	if (rc != 0) return rc;

	// Step 6: send write request (0 for write)
	bus_send_rw(bus, false);

	// Steps 7-10: send 4 bytes with parity; retry byte on nack
	for (size_t i = 0; i < 4; ++i) {
		uint8_t byte = data[i];
		uint8_t parity = protocol_even_parity_bit_u8(byte);
		int attempts = 0;
		while (attempts < 5) {
			bool ack = bus_write_byte(bus, i, byte, parity);
			if (ack) break;
			attempts++;
		}
		if (attempts == 5) return -3; // data byte failed after retries
	}
	return 0;
}

int app_read_4bytes_cb(Bus *bus, uint8_t address7, app_read_byte_cb cb, void *user_ctx)
{
	if (!bus || !cb) return -10;
	int rc = app_start_and_address(bus, address7);
	if (rc != 0) return rc;

	// Step 6: send read request (1 for read)
	bus_send_rw(bus, true);

	// Steps 7-11: receive 4 bytes; verify parity; ack/nack via retry, invoke cb
	for (size_t i = 0; i < 4; ++i) {
		int attempts = 0;
		for (;;) {
			uint8_t data_byte = 0, parity = 0;
			bus_read_byte(bus, i, &data_byte, &parity);
			uint8_t expected = protocol_even_parity_bit_u8(data_byte);
			bool ok = (expected == (parity & 0x1U));
			if (ok) {
				cb(i, data_byte, user_ctx);
				break;
			}
			attempts++;
			if (attempts >= 5) return -4;
		}
	}
	return 0;
}

int app_read_4bytes(Bus *bus, uint8_t address7, uint8_t out_data[4])
{
	if (!bus || !out_data) return -10;
	return app_read_4bytes_cb(bus, address7, app_store_byte_cb, out_data);
}