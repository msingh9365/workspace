#include "slave.h"
#include <stdlib.h>
#include <string.h>

int slave_init(slave_t *s, int id) {
	if (!s) return -1;
	s->id = id;
	s->memory = (uint8_t *)calloc(SLAVE_MEM_SIZE, 1);
	return s->memory ? 0 : -1;
}

void slave_destroy(slave_t *s) {
	if (!s) return;
	free(s->memory);
	s->memory = NULL;
}

static int validate_bounds(uint32_t address, uint32_t length) {
	uint64_t end = (uint64_t)address + (uint64_t)length;
	return end <= SLAVE_MEM_SIZE ? 0 : -1;
}

int slave_handle_frame(slave_t *s, const frame_header_t *hdr, const uint8_t *payload,
		uint8_t *out_buf) {
	if (!s || !hdr) return -1;
	if ((int)hdr->slave_id != s->id) return -1;
	if (validate_bounds(hdr->address, hdr->length) != 0) return -1;
	if (hdr->op == OP_WRITE) {
		if (!payload) return -1;
		memcpy(s->memory + hdr->address, payload, hdr->length);
		return 0;
	} else if (hdr->op == OP_READ) {
		if (!out_buf) return -1;
		memcpy(out_buf, s->memory + hdr->address, hdr->length);
		return 0;
	}
	return -1;
}