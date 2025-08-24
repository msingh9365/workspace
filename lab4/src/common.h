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
	uint16_t crc16;      // CRC-16-CCITT over fields above and payload
} frame_header_t;

#endif // COMMON_H