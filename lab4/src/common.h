#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#define SLAVE_MEM_SIZE 4096
#define MAX_SLAVES 8

// Operation codes
typedef enum {
	REQ_WRITE = 1,
	REQ_READ = 2,
	REQ_SHUTDOWN = 3
} op_code_t;

// Request from Master to a specific Slave
typedef struct {
	op_code_t op;
	uint32_t address; // byte offset in slave memory
	uint32_t length;  // number of bytes to read/write
	uint8_t *payload; // for write requests (length bytes)
	int target_slave_id; // 0..N-1
} request_t;

// Response back to Master
typedef struct {
	int status;       // 0 success, negative errno-like
	uint32_t length;  // number of bytes valid in data
	uint8_t *data;    // for read responses
	int from_slave_id;
} response_t;

// Thread-safe queue for requests/responses (bounded, simple)
typedef struct {
	void **items;
	size_t capacity;
	size_t head;
	size_t tail;
	size_t count;
	pthread_mutex_t mutex;
	pthread_cond_t not_empty;
	pthread_cond_t not_full;
} ts_queue_t;

int ts_queue_init(ts_queue_t *q, size_t capacity);
void ts_queue_destroy(ts_queue_t *q);
int ts_queue_put(ts_queue_t *q, void *item);
void *ts_queue_take(ts_queue_t *q);

#endif // COMMON_H