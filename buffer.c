
#include "buffer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//#define RX154_MAX_BUFF_SIZE 255

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

uint8_t buffer_peek(buffer_t* buf)
{
    return buf->buffer[buf->read_index];
}

size_t buffer_get_size(buffer_t* buf)
{
    /* Init fill space */
    int total_bytes = buf->fill_index - buf->read_index;
    /* Default write in front of read ptr, if circular, calcualate bytes
            at the beginning and end of the buffer */
    if (total_bytes < 0) {
        total_bytes += buf->capacity;
    }

    return total_bytes;
}

int16_t buffer_read(buffer_t* src_buf)
{
    if(!buffer_get_size(src_buf)) {
        return -1;
    }                            /* Error */
    int16_t read_byte = src_buf->buffer[src_buf->read_index];         /* Index of read byte */
    src_buf->read_index = src_buf->read_index + 1 % src_buf->capacity;    /* Increment read index */
    return read_byte;
}

uint8_t buffer_read_multiple(uint8_t* dest_buf, buffer_t* src_buf, size_t r_size)
{
    /* If no bytes available. return immediately
        Save local variable to preserve stack space
    */
    uint8_t available = buffer_get_size(src_buf);
    if(!available || !r_size) {
        return 0;
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
    memcpy(&dest_buf[bytes_end], src_buf, rem_bytes);
    /* Move read pointer accordingly to amount of bytes read */
    src_buf->read_index = rem_bytes;
    // dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;
    /* How to handle edge case where read pointer crosses write pointer? */
    return r_size;
}

size_t buffer_write(buffer_t* dest_buf, const uint8_t write_byte)
{
    /* How full is the write buffer? */
    size_t available = buffer_get_size(dest_buf);
    int vacant = dest_buf->capacity - available - 1;
    /* Are there less bytes available than number you want to write */
    if(vacant <= 0) {
//        buffer_flush(dest_buf);
        return 0;
    }
    /* Copy to local buffer by reference; Offset using read pointer */
    memcpy(&dest_buf->buffer[dest_buf->fill_index], &write_byte, 1);
    /* Update write pointer by write size */
    dest_buf->fill_index = (dest_buf->fill_index + 1) % dest_buf->capacity;
    return 1;
}


size_t buffer_write_multiple(buffer_t* dest_buf, const uint8_t* src_arr, size_t w_size) {
    int vacant_end;
    /* How full is the write buffer?  */
    size_t available = buffer_get_size(dest_buf);
    /* One less than total space, do not let pointers overlap to retain sizing info*/
    int vacant = dest_buf->capacity - available - 1;
    /* Terminate quickly if size is null */
    if (!w_size || !vacant) {
        return 0;
    }
    /* Floor the write size -> 4 spaces, want to write 5, write 3 bytes */
    if(w_size > vacant) {
        w_size = vacant;
    }
    /* Up to the user to tell that the number of bytes written < the size they intended */
    /* Writing to the end of the buffer overflow -> wrap around -> fill_index is empty space*/
    /* If the read_ptr is in front of the w_ptr, vacancy is space between*/
    if (dest_buf->read_index > dest_buf->fill_index) {
        vacant_end = dest_buf->read_index - dest_buf->fill_index - 1;
    } else {
        vacant_end = dest_buf->capacity - dest_buf->fill_index;
    }
    // printf("VACANE END: %d", vacant_end);
    if (w_size <= vacant_end) {
        /* Make sure that the write_ptr does not overlap with the read_ptr */
        // if (w_size == vacant_end)
            // w_size--;
        /* Copy to local buffer by reference; Offset using read pointer */
        memcpy(&dest_buf->buffer[dest_buf->fill_index], src_arr, w_size);
        /* Update write pointer by write size */
        dest_buf->fill_index = (dest_buf->fill_index + w_size) % dest_buf->capacity;
        /* Make sure fill index does not overlap with r_pointer -> this is start condition */
        return w_size;
    }
    /* Copy last and first chunks independently, move pointers accordingly */
    memcpy(&dest_buf->buffer[dest_buf->fill_index], src_arr, vacant_end);
    /* Copy the remaining bytes at the front of the circular buffer) */
    int bytes_rem = w_size - vacant_end;
    /* Copy to local buffer by reference; Start at beginning of buf array*/
    memcpy(dest_buf->buffer, &src_arr[vacant_end], bytes_rem);
    /* Move write pointer accordingly to amount of bytes read */
    dest_buf->fill_index = bytes_rem;
    // dest_buf->fill_index = (dest_buf->fill_index + filled_bytes) % dest_buf->capacity;
    /* How to handle edge case where read pointer crosses write pointer? */
    return w_size;
}

void buffer_print(buffer_t* buf)
{
    printf("[");
    for(int i = 0; i < buf->capacity; ++i) {
        if (i % 20 == 0 && i) printf("\n");
        printf("%d,", buf->buffer[i]);
    }
    printf("]");
    printf("\n");
}

void print_buffer_stats(buffer_t* buf)
{
    printf("--------------------------------------------------------\n");
    buffer_print(buf);
    printf("Read Index: %zu\n", buf->read_index);
    printf("Fill Index: %zu\n", buf->fill_index);
    printf("Size:       %zu\n", buffer_get_size(buf));
    printf("Vacant:     %lu\n", buf->capacity - buffer_get_size(buf) - 1);
    printf("--------------------------------------------------------\n");
}
