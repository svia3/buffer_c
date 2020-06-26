
#include "buffer.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RX154_MAX_BUFF_SIZE 255

/**
 * Initialize and fill a default buffer structure along with statically
 * allocate the space for the buffer array (RX154_MAX_BUFF_SIZE), fill
 * memory with zeroes. Set read and write pointers to index 0.
 *
 * @return  pointer to the buffer structure
 */
buf_handle_t buffer_init()
{
    // assert(size);
    /* Creating space for the buffer */
    buf_handle_t buf = malloc(sizeof(buffer_t));
    uint8_t buffer[RX154_MAX_BUFF_SIZE];
    memset(buffer, 0, RX154_MAX_BUFF_SIZE);
    /* Statically allocate? */
    // If successful, initialize struct
    if (buf) {
        buf->buffer = buffer;
        buf->read_index = 0;
        buf->fill_index = 10;
        buf->size = 0;
        buf->capacity = RX154_MAX_BUFF_SIZE;
        // buf->full = false;
    }
    return buf;
}

/**
 * Clear the data that is in the buffer array by filling the space with all 0's
 * Reset the read and write pointers.
 *
 * @param buf Pass by pointer; address of buffer struct
 */
void buffer_flush(buf_handle_t buf)
{
    /* Empty buffer space */
    memset(buf->buffer, 0, RX154_MAX_BUFF_SIZE);
    buf->read_index = 0;
    buf->fill_index = 0;
}

/**
 * Free the buffer container, not the data buffer array (USER JOB)
 * @param buf Pass by pointer; address of buffer struct
 */
void buffer_free(buf_handle_t buf)
{
    free(buf);
}

/**
 * Check how many bytes are available to read (occupied) in the buffer array.
 * @param buf Pass by pointer; address of buffer struct
 */
void buffer_update_capacity(buf_handle_t buf)
{
    /* Init fill space */
    int total_bytes = 0;
    if (buf->read_index - buf->fill_index) {
        total_bytes = buf->fill_index - buf->read_index;
    } else {
        /* Concat bytes at the end of the buffer and space filled at beginning */
        total_bytes = RX154_MAX_BUFF_SIZE - buf->read_index + buf->fill_index + 1;
    }
    buf->size = total_bytes;
}

/**
 * Pop one byte off the top of the buffer queue if there are bytes available
 * Move over the read pointer by one byte (one index)
 * Return -1 if no bytes available and none read.
 *
 * @param buf Pass by pointer; address of buffer struct
 * @return         Number of read bytes; copied into struct buffer
 */
uint8_t buffer_read(buf_handle_t buf)
{
    if(!buf->size) {
        return -1;
    }                            /* Error */
    uint8_t read_byte = buf->buffer[buf->read_index];     /* Index of read byte */
    buf->read_index = buf->read_index + 1 % buf->size;    /* Increment read index */
    buffer_update_capacity(buf);                          /* Update size */
    return read_byte;
}

/**
 * Pop a specificed amount of bytes off the top of the buffer queue of available.
 * Move the read pointer over by the amount of bytes poped.
 * Return -1 if no bytes were available and none  read.
 *
 * @param  buf_handle_buf Pass by pointer; address of buffer struct
 * @return                Number of readf bytes; copied into struct buffer
 */
uint8_t buffer_read_multiple(buf_handle_t buf, size_t r_size)
{
    /* If no bytes available. return immediately */
    if(!buf->size) {
        return -1;
    }
    /* Length of current buffer */
    int filled_bytes = 0;
    if (buf->fill_index - buf->read_index > 0) {
        /* Length of occupied buffer  */
        filled_bytes = buf->fill_index - buf->read_index;
        /* Does the user want to read more bytes than the length of occupied memory? */
        if (filled_bytes > r_size) {
            filled_bytes = r_size;
        }
        // memcpy(buf, &rx_buffer[rx_readCurrentIndex], fillBytes);    // copy to local buffer by reference
        buf->read_index = (buf->read_index + filled_bytes) % RX154_MAX_BUFF_SIZE; // move read pointer mod length
        buffer_update_capacity(buf);
        return filled_bytes;
    }
    /* Copy last and first chunks independently, move pointers accordingly */
    int bytes_end = (RX154_MAX_BUFF_SIZE - buf->read_index);
    // memcpy(buf, &rx_buffer[rx_readCurrentIndex], bytesCopyAtEnd);
    /* Copy the remaining bytes at the front of the circular buffer) */
    int rem_bytes = r_size - bytes_end;
    // memcpy(buf, rx_buffer, remainingBytes);
    /* Move read pointer accordingly to amount of bytes read */
    buf->read_index = rem_bytes;       // if 3 bytes were read, read_ptr will now be at index 3
    /* How to handle edge case where read pointer crosses write pointer? */
    buffer_update_capacity(buf);
    return bytes_end + rem_bytes;
}

/**
 * Take bytes from source buffer and copy to destination buffer. Handles transfer
 * of data from MAC layer to APP.
 *
 * @param  src_buf  Pass by pointer
 * @param  dest_buf Pass by pointer
 * @param  w_size   Length of data
 * @return 1 if successful, 0 if not
 */
uint8_t buffer_write(buf_handle_t dest_buf, uint8_t* src_buf, size_t w_size) {

    /* Update write pointers */
    int vacant = dest_buf->capacity - dest_buf->size;
    if(vacant <= w_size) {
        buffer_flush(dest_buf);
    }
    /* Copy data over usign memset */
    if(w_size <= dest_buf->capacity) {
        uint8_t* array_offset = dest_buf->buffer + (dest_buf->fill_index * sizeof(uint8_t));
        memcpy(array_offset, src_buf, w_size);
        /* Update write pointer by write size */
        dest_buf->fill_index += w_size;
        return 1;
    }
    return 0;
}

void buffer_print(buf_handle_t buf) {
    for(int i = 0; i < buf->capacity; ++i) {
        printf("%d", buf->buffer[i]);
    }
}
