#ifndef SLAVE_H
#define SLAVE_H

#include "common.h"

typedef struct {
	int id;
	uint8_t *memory;
} slave_t;

int slave_init(slave_t *s, int id);
void slave_destroy(slave_t *s);

// Handle a frame and produce an optional response payload for reads.
// Returns 0 on success, negative on error. For OP_READ, fills out_buf with data.
int slave_handle_frame(slave_t *s, const frame_header_t *hdr, const uint8_t *payload,
		uint8_t *out_buf /* size >= hdr->length */);

#endif // SLAVE_H