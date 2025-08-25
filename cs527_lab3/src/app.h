#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "bus.h"

// Returns 0 on success, negative on error
int app_write_4bytes(Bus *bus, uint8_t address7, const uint8_t data[4]);
int app_read_4bytes(Bus *bus, uint8_t address7, uint8_t out_data[4]);

// Callback invoked per byte read successfully with verified parity.
// User data pointer is passed through for stateful receivers.
typedef void (*app_read_byte_cb)(size_t index0to3, uint8_t data_byte, void *user_ctx);

// Read 4 bytes and invoke callback for each byte; does not require out buffer.
int app_read_4bytes_cb(Bus *bus, uint8_t address7, app_read_byte_cb cb, void *user_ctx);

#endif // APP_H