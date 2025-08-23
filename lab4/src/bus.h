#ifndef BUS_H
#define BUS_H

#include "common.h"

// Simple Slave device
typedef struct {
	int id;
	uint8_t *memory; // 4096 bytes
} slave_device_t;

// Single-threaded bus containing an array of slaves
typedef struct {
	int num_slaves;
	slave_device_t slaves[MAX_SLAVES];
} bus_t;

int bus_init(bus_t *bus, int num_slaves);
void bus_destroy(bus_t *bus);

// Synchronous operations initiated by master
// Returns 0 on success, negative on error
int bus_write(bus_t *bus, int slave_id, uint32_t address, const uint8_t *data, uint32_t length);
int bus_read(bus_t *bus, int slave_id, uint32_t address, uint8_t *out, uint32_t length);
int bus_shutdown(bus_t *bus); // no-op in single-threaded version

#endif // BUS_H