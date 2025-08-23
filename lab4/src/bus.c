#include "bus.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void *slave_thread_main(void *arg) {
	slave_device_t *dev = (slave_device_t *)arg;
	bus_t *bus = dev->bus;
	ts_queue_t *in = &bus->to_slave_queues[dev->id];
	dev->running = 1;
	while (1) {
		request_t *req = (request_t *)ts_queue_take(in);
		if (!req) continue;
		response_t *resp = (response_t *)calloc(1, sizeof(response_t));
		resp->from_slave_id = dev->id;
		if (req->op == REQ_SHUTDOWN) {
			resp->status = 0;
			ts_queue_put(&bus->to_master_queue, resp);
			free(req->payload);
			free(req);
			break;
		}

		// Bounds checking
		uint32_t end = req->address + req->length;
		if (end > SLAVE_MEM_SIZE) {
			resp->status = -1;
		} else if (req->op == REQ_WRITE) {
			memcpy(dev->memory + req->address, req->payload, req->length);
			resp->status = 0;
			resp->length = 0;
		} else if (req->op == REQ_READ) {
			resp->status = 0;
			resp->length = req->length;
			resp->data = (uint8_t *)malloc(req->length);
			memcpy(resp->data, dev->memory + req->address, req->length);
		} else {
			resp->status = -2;
		}
		ts_queue_put(&bus->to_master_queue, resp);
		free(req->payload);
		free(req);
	}
	dev->running = 0;
	return NULL;
}

int bus_init(bus_t *bus, int num_slaves) {
	if (num_slaves < 1 || num_slaves > MAX_SLAVES) return -1;
	bus->num_slaves = num_slaves;
	if (ts_queue_init(&bus->to_master_queue, 1024) != 0) return -1;
	for (int i = 0; i < num_slaves; ++i) {
		bus->slaves[i] = NULL;
		ts_queue_init(&bus->to_slave_queues[i], 1024);
	}
	for (int i = 0; i < num_slaves; ++i) {
		slave_start(bus, i);
	}
	return 0;
}

void bus_destroy(bus_t *bus) {
	for (int i = 0; i < bus->num_slaves; ++i) {
		if (bus->slaves[i]) {
			slave_join_and_free(bus->slaves[i]);
			bus->slaves[i] = NULL;
		}
		ts_queue_destroy(&bus->to_slave_queues[i]);
	}
	ts_queue_destroy(&bus->to_master_queue);
}

int bus_send_request(bus_t *bus, request_t *req) {
	if (req->target_slave_id < 0 || req->target_slave_id >= bus->num_slaves) return -1;
	return ts_queue_put(&bus->to_slave_queues[req->target_slave_id], req);
}

response_t *bus_take_response(bus_t *bus) {
	return (response_t *)ts_queue_take(&bus->to_master_queue);
}

int slave_start(bus_t *bus, int id) {
	slave_device_t *dev = (slave_device_t *)calloc(1, sizeof(slave_device_t));
	if (!dev) return -1;
	dev->id = id;
	dev->bus = bus;
	dev->memory = (uint8_t *)calloc(SLAVE_MEM_SIZE, 1);
	if (!dev->memory) { free(dev); return -1; }
	bus->slaves[id] = dev;
	if (pthread_create(&dev->thread, NULL, slave_thread_main, dev) != 0) {
		free(dev->memory);
		free(dev);
		bus->slaves[id] = NULL;
		return -1;
	}
	return 0;
}

void slave_join_and_free(slave_device_t *dev) {
	if (!dev) return;
	pthread_join(dev->thread, NULL);
	free(dev->memory);
	free(dev);
}