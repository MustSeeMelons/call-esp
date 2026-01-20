#include "include/ring_buffer.h"
#include <string.h>

void ring_buffer_init(ring_buffer_t *rb, char *buffer, size_t size) {
    rb->buffer = buffer;
    rb->size = size;
    rb->write_pos = 0;
    rb->read_pos = 0;
}

size_t ring_buffer_write(ring_buffer_t *rb, const char *data, size_t len) {
    size_t written = 0;

    for (size_t i = 0; i < len; i++) {
        rb->buffer[rb->write_pos] = data[i];
        rb->write_pos = (rb->write_pos + 1) % rb->size;

        // If we overwrote unread data, move read position
        if (rb->write_pos == rb->read_pos) {
            rb->read_pos = (rb->read_pos + 1) % rb->size;
        }

        written++;
    }

    return written;
}

void ring_buffer_consume(ring_buffer_t *rb, size_t len) {
    size_t available_bytes = ring_buffer_available(rb);

    if (len > available_bytes) {
        len = available_bytes;
    }

    rb->read_pos = (rb->read_pos + len) % rb->size;
}

size_t ring_buffer_available(const ring_buffer_t *rb) {
    // When write is ahead then bytes avaialble is simply the subtraction
    if (rb->write_pos >= rb->read_pos) {
        return rb->write_pos - rb->read_pos;
    }

    // Handle wraparound
    return rb->size - rb->read_pos + rb->write_pos;
}

size_t ring_buffer_peek(ring_buffer_t *rb, char *dest, size_t len) {
    size_t available_bytes = ring_buffer_available(rb);
    size_t to_copy = (available_bytes < len) ? available_bytes : len;

    size_t pos = rb->read_pos;
    for (size_t i = 0; i < to_copy; i++) {
        dest[i] = rb->buffer[pos];
        // We may loop over
        pos = (pos + 1) % rb->size;
    }

    return to_copy;
}

size_t ring_buffer_read(ring_buffer_t *rb, char *dest, size_t len) {
    size_t read = ring_buffer_peek(rb, dest, len);
    ring_buffer_consume(rb, read);
    return read;
}

size_t ring_buffer_free_space(const ring_buffer_t *rb) {
    return rb->size - ring_buffer_available(rb) - 1;
}

void ring_buffer_clear(ring_buffer_t *rb) {
    rb->read_pos = rb->write_pos;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb->read_pos == rb->write_pos;
}