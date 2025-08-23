#ifndef BUS_H
#define BUS_H

#include "common.h"

typedef struct slave_device slave_device_t;

typedef struct {
	int num_slaves;
	slave_device_t *slaves[MAX_SLAVES];
	ts_queue_t to_slave_queues[MAX_SLAVES];
	ts_queue_t to_master_queue; // responses
} bus_t;

int bus_init(bus_t *bus, int num_slaves);
void bus_destroy(bus_t *bus);

// Master-side helpers
int bus_send_request(bus_t *bus, request_t *req);
response_t *bus_take_response(bus_t *bus);

// Slave device API
struct slave_device {
	int id;
	pthread_t thread;
	uint8_t *memory;
	bus_t *bus;
	int running;
};

int slave_start(bus_t *bus, int id);
void slave_join_and_free(slave_device_t *dev);

#endif // BUS_H