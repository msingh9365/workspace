#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Protocol helpers for parity and byte packing

// Even parity bit for a byte: returns 1 if the number of set bits in data is odd,
// else returns 0. When appended, total set bits become even.
static inline uint8_t protocol_even_parity_bit_u8(uint8_t data)
{
	uint8_t x = data;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (uint8_t)(x & 1U);
}

// Address payload: 7-bit address in bits [6:0], parity bit in bit 7 (MSB)
static inline uint8_t protocol_pack_address(uint8_t address7)
{
	address7 &= 0x7FU;
	uint8_t parity = protocol_even_parity_bit_u8(address7);
	return (uint8_t)((parity << 7) | address7);
}

static inline uint8_t protocol_unpack_address(uint8_t address_payload)
{
	return (uint8_t)(address_payload & 0x7FU);
}

static inline bool protocol_check_address_parity(uint8_t address_payload)
{
	uint8_t address7 = (uint8_t)(address_payload & 0x7FU);
	uint8_t parity = (uint8_t)((address_payload >> 7) & 0x1U);
	return protocol_even_parity_bit_u8(address7) == parity;
}

#endif // PROTOCOL_H