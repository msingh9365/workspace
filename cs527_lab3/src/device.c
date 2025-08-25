#include "device.h"
#include <string.h>

void device_init(Device *dev, uint8_t fill_value)
{
	if (!dev) return;
	for (size_t i = 0; i < DEVICE_MEMORY_SIZE; ++i) {
		dev->memory[i] = fill_value;
	}
	dev->base_address7 = 0U;
	dev->is_read_transaction = false;
}

bool device_on_start(Device *dev)
{
	(void)dev;
	// For this simulation, always ack a start bit.
	return true;
}

bool device_receive_address(Device *dev, uint8_t address_payload)
{
	if (!dev) return false;
	if (!protocol_check_address_parity(address_payload)) {
		return false; // nack on bad parity
	}
	dev->base_address7 = protocol_unpack_address(address_payload) & 0x7FU;
	return true; // ack
}

void device_set_rw(Device *dev, bool is_read)
{
	if (!dev) return;
	dev->is_read_transaction = is_read;
}

bool device_receive_write_byte(Device *dev, size_t offset0to3, uint8_t data_byte, uint8_t parity_bit)
{
	if (!dev) return false;
	uint8_t expected = protocol_even_parity_bit_u8(data_byte);
	if ((parity_bit & 0x1U) != expected) {
		return false; // nack
	}
	uint8_t address = (uint8_t)((dev->base_address7 + (uint8_t)offset0to3) & 0x7FU);
	dev->memory[address] = data_byte;
	return true; // ack
}

void device_send_read_byte(Device *dev, size_t offset0to3, uint8_t *out_data_byte, uint8_t *out_parity_bit)
{
	if (!dev) return;
	uint8_t address = (uint8_t)((dev->base_address7 + (uint8_t)offset0to3) & 0x7FU);
	uint8_t data = dev->memory[address];
	if (out_data_byte) *out_data_byte = data;
	if (out_parity_bit) *out_parity_bit = protocol_even_parity_bit_u8(data);
}