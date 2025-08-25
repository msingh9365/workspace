#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Forward-declare protocol helpers
#include "protocol.h"

#define DEVICE_MEMORY_SIZE 128U

typedef struct Device {
	uint8_t memory[DEVICE_MEMORY_SIZE];
	uint8_t base_address7; // 7-bit base address for current transaction
	bool is_read_transaction;
} Device;

void device_init(Device *dev, uint8_t fill_value);

// Step 1-2: Application sends start, device responds with ack/nack (true=ack)
bool device_on_start(Device *dev);

// Step 3-5: App sends address payload; device checks parity and acks/nacks
bool device_receive_address(Device *dev, uint8_t address_payload);

// Step 6: App sends read/write request bit
void device_set_rw(Device *dev, bool is_read);

// Step 7-10: Data phase (4 bytes)
// Write direction: app -> device
bool device_receive_write_byte(Device *dev, size_t offset0to3, uint8_t data_byte, uint8_t parity_bit);

// Read direction: device -> app
void device_send_read_byte(Device *dev, size_t offset0to3, uint8_t *out_data_byte, uint8_t *out_parity_bit);

#endif // DEVICE_H