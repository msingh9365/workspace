#ifndef BUS_H
#define BUS_H

#include "common.h"

typedef struct slave_device slave_device_t;

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t req_ready;
	pthread_cond_t resp_ready;
	int has_request;
	int has_response;
	request_t *req;
	response_t *resp;
} mailbox_t;

typedef struct {
	int num_slaves;
	slave_device_t *slaves[MAX_SLAVES];
	mailbox_t mbox[MAX_SLAVES];
} bus_t;

int bus_init(bus_t *bus, int num_slaves);
void bus_destroy(bus_t *bus);

// Master-side synchronous helper
int bus_send_and_wait(bus_t *bus, request_t *req, response_t **out_resp);

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