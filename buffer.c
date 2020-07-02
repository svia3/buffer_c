
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

uint8_t buffer_read(buffer_t* src_buf)
{
    if(!buffer_get_size(src_buf)) {
        return -1;
    }                            /* Error */
    uint8_t read_byte = src_buf->buffer[src_buf->read_index];         /* Index of read byte */
    src_buf->read_index = src_buf->read_index + 1 % src_buf->capacity;    /* Increment read index */
    return read_byte;
}

uint8_t buffer_read_multiple(uint8_t* dest_buf, buffer_t* src_buf, size_t r_size)
{
    /* If no bytes available. return immediately
        Save local variable to preserve stack space
    */
    uint8_t available = buffer_get_size(src_buf);
    if(!available) {
        return -1;
    }
    /* Floor the read size if it is larger than the size of buffer */
    if(r_size > available) {
        r_size = available;
    }
    /* Length of current buffer */
    int filled_bytes = 0;
    if (src_buf->fill_index - src_buf->read_index > 0) {
        // printf("here");
        /* Length of occupied buffer  */
        filled_bytes = src_buf->fill_index - src_buf->read_index;
        // printf("filled bytes: %d", filled_bytes);
        /* Does the user want to read more bytes than the length of occupied memory? */
        if (r_size > filled_bytes) {
            r_size = filled_bytes;
        }
        /* Copy to local buffer by reference; Offset using read pointer */
        memcpy(dest_buf, &src_buf->buffer[src_buf->read_index], r_size);
        // move read pointer mod length
        src_buf->read_index = (src_buf->read_index + r_size) % src_buf->capacity;
        // move write pointer mod length
        // dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;
        return r_size;
    }

    /* Copy last and first chunks independently, move pointers accordingly */
    int bytes_end = (src_buf->capacity - src_buf->read_index);
    /* Copy to local buffer by reference; Offset using read pointer */
    memcpy(dest_buf, &src_buf->buffer[src_buf->read_index], bytes_end);
    /* Copy the remaining bytes at the front of the circular buffer) */
    int rem_bytes = r_size - bytes_end;
    /* Copy to local buffer by reference; Start at beginning of buf array*/
    memcpy(dest_buf, src_buf, rem_bytes);
    /* Move read pointer accordingly to amount of bytes read */
    src_buf->read_index = rem_bytes;
    // dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;
    /* How to handle edge case where read pointer crosses write pointer? */
    return bytes_end + rem_bytes;
}

bool buffer_write(buffer_t* dest_buf, uint8_t write_byte)
{
    /* How full is the write buffer? */
    uint8_t available = buffer_get_size(dest_buf);
    int vacant = dest_buf->capacity - available;
    if(vacant <= 0) {
        buffer_flush(dest_buf);
    }
    /* Copy to local buffer by reference; Offset using read pointer */
    memcpy(&dest_buf->buffer[dest_buf->fill_index], &write_byte, 1);
    /* Update write pointer by write size */
    dest_buf->fill_index = (dest_buf->fill_index + 1) % dest_buf->capacity;
    return true;
}

bool buffer_write_multiple(buffer_t* dest_buf, uint8_t* src_arr, size_t w_size) {

    /* How full is the write buffer?  */
    uint8_t available = buffer_get_size(dest_buf);
    int vacant = dest_buf->capacity - available;
    /* Are there less bytes available than number you want to write */
    /* Change this later to fill up the buffer?*/
    if(vacant <= w_size) {
        buffer_flush(dest_buf); // CHANGE THIS ****!
    }
    /* Copy data over usign memset */
    if(w_size <= dest_buf->capacity) {
        // uint8_t* dest_offset = dest_buf->buffer + dest_buf->fill_index;    /* Circular */
        memcpy(&dest_buf->buffer[dest_buf->fill_index], src_arr, w_size);
        /* Update write pointer by write size */
        dest_buf->fill_index = (dest_buf->fill_index + w_size) % dest_buf->capacity;
        return true;
    }
    return false;
}

void buffer_print(buffer_t* buf) {
    printf("[");
    for(int i = 0; i < buf->capacity; ++i) {
        printf("%d,", buf->buffer[i]);
    }
    printf("]");
    printf("\n");
}

void print_buffer_stats(buffer_t* buf) {
    buffer_print(buf);
    printf("Read Index: %zu\n", buf->read_index);
    printf("Fill Index: %zu\n", buf->fill_index);
}
