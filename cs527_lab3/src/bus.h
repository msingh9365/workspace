#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "device.h"

// Simple synchronous bus that connects one application to one device.
typedef struct Bus {
	Device *device;
} Bus;

static inline void bus_init(Bus *bus, Device *device)
{
	if (!bus) return;
	bus->device = device;
}

static inline bool bus_send_start(Bus *bus)
{
	return device_on_start(bus->device);
}

static inline bool bus_send_address(Bus *bus, uint8_t address_payload)
{
	return device_receive_address(bus->device, address_payload);
}

static inline void bus_send_rw(Bus *bus, bool is_read)
{
	device_set_rw(bus->device, is_read);
}

static inline bool bus_write_byte(Bus *bus, size_t offset0to3, uint8_t data_byte, uint8_t parity_bit)
{
	return device_receive_write_byte(bus->device, offset0to3, data_byte, parity_bit);
}

static inline void bus_read_byte(Bus *bus, size_t offset0to3, uint8_t *out_data_byte, uint8_t *out_parity_bit)
{
	device_send_read_byte(bus->device, offset0to3, out_data_byte, out_parity_bit);
}

#endif // BUS_H