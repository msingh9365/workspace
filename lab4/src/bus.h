#ifndef BUS_H
#define BUS_H

#include "common.h"
#include "slave.h"

typedef struct {
	int num_slaves;
	slave_t slaves[MAX_SLAVES];
} bus_t;

int bus_init(bus_t *bus, int num_slaves);
void bus_destroy(bus_t *bus);

// Transfer a frame from master to the addressed slave and optionally return data
// For OP_READ, out_buf must be at least hdr->length bytes; ignored for writes
int bus_transfer(bus_t *bus, const frame_header_t *hdr, const uint8_t *payload,
		uint8_t *out_buf);

#endif // BUS_H