#include "bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static request_t *make_write(int slave, uint32_t addr, const uint8_t *data, uint32_t len) {
	request_t *r = (request_t *)calloc(1, sizeof(request_t));
	r->op = REQ_WRITE;
	r->address = addr;
	r->length = len;
	r->payload = (uint8_t *)malloc(len);
	memcpy(r->payload, data, len);
	r->target_slave_id = slave;
	return r;
}

static request_t *make_read(int slave, uint32_t addr, uint32_t len) {
	request_t *r = (request_t *)calloc(1, sizeof(request_t));
	r->op = REQ_READ;
	r->address = addr;
	r->length = len;
	r->payload = NULL;
	r->target_slave_id = slave;
	return r;
}

static request_t *make_shutdown(int slave) {
	request_t *r = (request_t *)calloc(1, sizeof(request_t));
	r->op = REQ_SHUTDOWN;
	r->target_slave_id = slave;
	return r;
}

static void free_response(response_t *resp) {
	if (!resp) return;
	free(resp->data);
	free(resp);
}

int main(int argc, char **argv) {
	int num_slaves = 3;
	if (argc > 1) num_slaves = atoi(argv[1]);
	if (num_slaves <= 0 || num_slaves > MAX_SLAVES) num_slaves = 3;

	bus_t bus;
	if (bus_init(&bus, num_slaves) != 0) {
		fprintf(stderr, "Failed to init bus\n");
		return 1;
	}

	// Write different patterns to each slave
	for (int s = 0; s < num_slaves; ++s) {
		char msg[64];
		snprintf(msg, sizeof(msg), "Hello from master to slave %d", s);
		request_t *w = make_write(s, 100, (const uint8_t *)msg, (uint32_t)strlen(msg)+1);
		bus_send_request(&bus, w);
		response_t *wr = bus_take_response(&bus);
		printf("WRITE to slave %d -> status=%d\n", s, wr->status);
		free_response(wr);
	}

	// Read back
	for (int s = 0; s < num_slaves; ++s) {
		request_t *r = make_read(s, 100, 32);
		bus_send_request(&bus, r);
		response_t *rr = bus_take_response(&bus);
		printf("READ from slave %d -> status=%d, data='%.*s'\n", s, rr->status, (int)rr->length, rr->data ? (char *)rr->data : "");
		free_response(rr);
	}

	// Bounds test
	request_t *bad = make_read(0, SLAVE_MEM_SIZE-10, 20);
	bus_send_request(&bus, bad);
	response_t *br = bus_take_response(&bus);
	printf("BOUNDS READ status=%d (expect -1)\n", br->status);
	free_response(br);

	// Shutdown slaves
	for (int s = 0; s < num_slaves; ++s) {
		bus_send_request(&bus, make_shutdown(s));
	}
	for (int s = 0; s < num_slaves; ++s) {
		response_t *sr = bus_take_response(&bus);
		printf("SHUTDOWN ack from slave %d status=%d\n", sr->from_slave_id, sr->status);
		free_response(sr);
	}

	bus_destroy(&bus);
	return 0;
}