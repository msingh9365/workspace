#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "bus.h"

// Returns 0 on success, negative on error
int app_write_4bytes(Bus *bus, uint8_t address7, const uint8_t data[4]);
int app_read_4bytes(Bus *bus, uint8_t address7, uint8_t out_data[4]);

#endif // APP_H