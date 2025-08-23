#include "bus.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void mailbox_init(mailbox_t *mb) {
	pthread_mutex_init(&mb->mutex, NULL);
	pthread_cond_init(&mb->req_ready, NULL);
	pthread_cond_init(&mb->resp_ready, NULL);
	mb->has_request = 0;
	mb->has_response = 0;
	mb->req = NULL;
	mb->resp = NULL;
}

static void mailbox_destroy(mailbox_t *mb) {
	pthread_mutex_destroy(&mb->mutex);
	pthread_cond_destroy(&mb->req_ready);
	pthread_cond_destroy(&mb->resp_ready);
}

static void *slave_thread_main(void *arg) {
	slave_device_t *dev = (slave_device_t *)arg;
	bus_t *bus = dev->bus;
	mailbox_t *mb = &bus->mbox[dev->id];
	dev->running = 1;
	for (;;) {
		pthread_mutex_lock(&mb->mutex);
		while (!mb->has_request) {
			pthread_cond_wait(&mb->req_ready, &mb->mutex);
		}
		request_t *req = mb->req;
		mb->req = NULL;
		mb->has_request = 0;
		pthread_mutex_unlock(&mb->mutex);

		response_t *resp = (response_t *)calloc(1, sizeof(response_t));
		resp->from_slave_id = dev->id;
		if (req->op == REQ_SHUTDOWN) {
			resp->status = 0;
			free(req->payload);
			free(req);
			pthread_mutex_lock(&mb->mutex);
			mb->resp = resp;
			mb->has_response = 1;
			pthread_cond_signal(&mb->resp_ready);
			pthread_mutex_unlock(&mb->mutex);
			break;
		}

		uint32_t end = req->address + req->length;
		if (end > SLAVE_MEM_SIZE) {
			resp->status = -1;
		} else if (req->op == REQ_WRITE) {
			memcpy(dev->memory + req->address, req->payload, req->length);
			resp->status = 0;
		} else if (req->op == REQ_READ) {
			resp->status = 0;
			resp->length = req->length;
			resp->data = (uint8_t *)malloc(req->length);
			memcpy(resp->data, dev->memory + req->address, req->length);
		} else {
			resp->status = -2;
		}

		pthread_mutex_lock(&mb->mutex);
		mb->resp = resp;
		mb->has_response = 1;
		pthread_cond_signal(&mb->resp_ready);
		pthread_mutex_unlock(&mb->mutex);

		free(req->payload);
		free(req);
	}
	dev->running = 0;
	return NULL;
}

int bus_init(bus_t *bus, int num_slaves) {
	if (num_slaves < 1 || num_slaves > MAX_SLAVES) return -1;
	bus->num_slaves = num_slaves;
	for (int i = 0; i < num_slaves; ++i) {
		bus->slaves[i] = NULL;
		mailbox_init(&bus->mbox[i]);
	}
	for (int i = 0; i < num_slaves; ++i) {
		if (slave_start(bus, i) != 0) return -1;
	}
	return 0;
}

void bus_destroy(bus_t *bus) {
	for (int i = 0; i < bus->num_slaves; ++i) {
		if (bus->slaves[i]) {
			slave_join_and_free(bus->slaves[i]);
			bus->slaves[i] = NULL;
		}
		mailbox_destroy(&bus->mbox[i]);
	}
}

int bus_send_and_wait(bus_t *bus, request_t *req, response_t **out_resp) {
	if (!req || req->target_slave_id < 0 || req->target_slave_id >= bus->num_slaves) return -1;
	mailbox_t *mb = &bus->mbox[req->target_slave_id];

	pthread_mutex_lock(&mb->mutex);
	// Wait if previous response hasn't been collected (single in-flight request per slave)
	while (mb->has_request) {
		pthread_cond_wait(&mb->resp_ready, &mb->mutex);
	}
	mb->req = req;
	mb->has_request = 1;
	pthread_cond_signal(&mb->req_ready);

	while (!mb->has_response) {
		pthread_cond_wait(&mb->resp_ready, &mb->mutex);
	}
	response_t *resp = mb->resp;
	mb->resp = NULL;
	mb->has_response = 0;
	pthread_mutex_unlock(&mb->mutex);

	*out_resp = resp;
	return 0;
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