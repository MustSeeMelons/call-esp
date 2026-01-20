#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char *buffer;
    size_t size;
    size_t write_pos;
    size_t read_pos;
} ring_buffer_t;

// Bring your own buffer
void ring_buffer_init(ring_buffer_t *rb, char *buffer, size_t size);

size_t ring_buffer_write(ring_buffer_t *rb, const char *data, size_t len);

size_t ring_buffer_read(ring_buffer_t *rb, char *dest, size_t len);

// Retrieve a slice from the current buffer
size_t ring_buffer_peek(ring_buffer_t *rb, char *dest, size_t len);

// Moves read pointer forward
void ring_buffer_consume(ring_buffer_t *rb, size_t len);

// How many bytes are avaialble to be read
size_t ring_buffer_available(const ring_buffer_t *rb);

// How many bytes left till we wrap around
size_t ring_buffer_free_space(const ring_buffer_t *rb);

// Reset all pointers, does not clear the buffer!
void ring_buffer_clear(ring_buffer_t *rb);

bool ring_buffer_is_empty(const ring_buffer_t *rb);

#endif