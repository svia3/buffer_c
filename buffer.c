
#include "buffer.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RX154_MAX_BUFF_SIZE 255

/* How many bytes are available in the buffer? -> move over to buffer_c*/
static uint8_t buffer_get_size(buffer_t* buf);

bool buffer_init(buffer_t* buf, size_t capacity)
{
    // assert(size);
    /* Creating space for the buffer */
    if (!capacity && !buf) {
        return false;
    }
//    struct buf_handle_t buf_p;
//    struct buffet_t = buf;
//    buf_p = &buf;
    buf->buffer = (uint8_t*)malloc(capacity);
    if (!buf->buffer) {
        return false;
    }
    /* Default */
    buf->read_index = 0;
    buf->fill_index = 0;
    buf->capacity = capacity;
    return true;
}

void buffer_flush(buffer_t* buf)
{
    /* Empty buffer space */
    memset(buf->buffer, 0, buf->capacity);
    buf->read_index = 0;
    buf->fill_index = 0;
}

void buffer_free(buffer_t* buf)
{
    free(buf->buffer);
}

/**
 * Check how many bytes are available to read (occupied) in the buffer array.
 * @param buf Pass by pointer; address of buffer struct
 */
static uint8_t buffer_get_size(buffer_t* buf)
{
    /* Init fill space */
    int total_bytes = 0;
    if (buf->read_index - buf->fill_index > 0) {
        total_bytes = buf->fill_index - buf->read_index;
    } else {
        /* Concat bytes at the end of the buffer and space filled at beginning */
        total_bytes = buf->capacity - buf->read_index + buf->fill_index + 1;
    }
    return total_bytes;
}

/**
 * Pop one byte off the top of the buffer queue if there are bytes available
 * Move over the read pointer by one byte (one index)
 * Return -1 if no bytes available and none read.
 *
 * @param buf      Pass by pointer; address of buffer struct
 * @return         Number of read bytes; copied into struct buffer
 */
uint8_t buffer_read(buffer_t* src_buf)
{
    if(!buffer_get_size(src_buf)) {
        return -1;
    }                            /* Error */
    uint8_t read_byte = src_buf->buffer[src_buf->read_index];         /* Index of read byte */
    src_buf->read_index = src_buf->read_index + 1 % src_buf->capacity;    /* Increment read index */
    return read_byte;
}

/**
 * Pop a specificed amount of bytes off the top of the buffer queue of available.
 * Move the read pointer over by the amount of bytes poped.
 * Return -1 if no bytes were available and none  read.
 *
 * @param  buf_handle_buf Pass by pointer; address of buffer struct
 * @return                Number of read bytes; copied into struct buffer
 */
uint8_t buffer_read_multiple(buffer_t* dest_buf, buffer_t* src_buf, size_t r_size)
{
    /* If no bytes available. return immediately */
    if(!buffer_get_size(src_buf)) {
        return -1;
    }
    /* Length of current buffer */
    int filled_bytes = 0;
    if (src_buf->fill_index - src_buf->read_index > 0) {
        /* Length of occupied buffer  */
        filled_bytes = src_buf->fill_index - src_buf->read_index;
        /* Does the user want to read more bytes than the length of occupied memory? */
        if (filled_bytes > r_size) {
            filled_bytes = r_size;
        }
        // copy to local buffer by reference
        memcpy(dest_buf, src_buf, filled_bytes);
        // move read pointer mod length
        src_buf->read_index = (src_buf->read_index + filled_bytes) % src_buf->capacity;
        // move write pointer mod length
        dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;
        return filled_bytes;
    }

    /* Copy last and first chunks independently, move pointers accordingly */
    int bytes_end = (src_buf->capacity - src_buf->read_index);
    memcpy(dest_buf, &src_buf->buffer[src_buf->read_index], bytes_end);

    /* Copy the remaining bytes at the front of the circular buffer) */
    int rem_bytes = r_size - bytes_end;
    memcpy(dest_buf, src_buf, rem_bytes);

    /* Move read pointer accordingly to amount of bytes read */
    src_buf->read_index = rem_bytes;       // if 3 bytes were read, read_ptr will now be at index 3
    dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;   // move write ptr of dest buffer

    /* How to handle edge case where read pointer crosses write pointer? */
    return bytes_end + rem_bytes;
}

uint8_t buffer_write(buffer_t* dest_buf, uint8_t write_byte)
{
    /* How full is the write buffer? */
    uint8_t available = buffer_get_size(dest_buf);
    int vacant = dest_buf->capacity - available;
    if(vacant <= 0) {
        buffer_flush(dest_buf);
    }
    uint8_t* dest_offset = dest_buf->buffer + dest_buf->fill_index;    /* Circular */
    memcpy(dest_offset, &write_byte, 1);
    /* Update write pointer by write size */
    dest_buf->fill_index = (dest_buf->fill_index + 1) % dest_buf->capacity;
    return true;
}

uint8_t buffer_write_multiple(buffer_t* dest_buf, buffer_t* src_buf, size_t w_size) {

    /* How full is the write buffer? */
    uint8_t available = buffer_get_size(dest_buf);
    int vacant = dest_buf->capacity - available;
    /* Are there less bytes available than number you want to write */
    if(vacant < w_size) {
        buffer_flush(dest_buf);
    }
    /* Copy data over usign memset */
    if(w_size <= dest_buf->capacity) {
        uint8_t* dest_offset = dest_buf->buffer + dest_buf->fill_index;    /* Circular */
        memcpy(dest_offset, &src_buf[src_buf->read_index], w_size);
        /* Update write pointer by write size */
        dest_buf->fill_index = (dest_buf->fill_index + w_size) % dest_buf->capacity;
        src_buf->read_index = (src_buf->read_index + w_size) % src_buf->capacity;
        return true;
    }
    return false;
}

void buffer_print(buffer_t* buf) {
    for(int i = 0; i < buf->capacity; ++i) {
        printf("%d", buf->buffer[i]);
    }
}
