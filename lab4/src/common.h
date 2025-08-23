#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

#define SLAVE_MEM_SIZE 4096
#define MAX_SLAVES 8

// Frame-level protocol between master and slave via bus
typedef enum {
	OP_WRITE = 1,
	OP_READ = 2
} op_t;

typedef struct {
	uint8_t start;       // sync, e.g., 0xAA
	uint8_t slave_id;    // destination slave
	uint8_t op;          // op_t
	uint16_t length;     // number of data bytes
	uint32_t address;    // byte offset
	// Followed by 'length' bytes of payload for write
	// For read, payload length is 0
	uint8_t checksum;    // simple XOR over header and payload
} frame_header_t;

#endif // COMMON_H