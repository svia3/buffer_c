#include <stdint.h>
#include <stdbool.h>

typedef struct buffer_t buffer_t;
//typedef buffer_t* buf_handle_t;
// typedef uint8_t (*available)(buf_handle_t);

/* Types */
typedef unsigned long size_t;

struct buffer_t {
    uint8_t* buffer;
    size_t read_index;  // head of array -> start reading from here.
    size_t fill_index;  // next available byte to write to
    uint8_t size;
    size_t capacity;
    // bool full;
};

/**
 * Initialize and fill a default buffer structure along with statically
 * allocate the space for the buffer array (RX154_MAX_BUFF_SIZE), fill
 * memory with zeroes. Set read and write pointers to index 0.
 *
 * @return  pointer to the buffer structure
 */
bool buffer_init(buffer_t* buf, size_t capacity);

/**
 * Clear the data that is in the buffer array by filling the space with all 0's
 * Reset the read and write pointers.
 *
 * @param buf Pass by pointer; address of buffer struct
 */
void buffer_flush(buffer_t* buf);                                            /* Reset buffer to empty; head == tail */
void buffer_free(buffer_t* buf);                                             /* Free the buffer struct, not data -> user job */
uint8_t buffer_read(buffer_t* buf);                                          /* Read one byte if available */
uint8_t buffer_read_multiple(buffer_t* buf, buffer_t* user_buf, size_t r_size)/* Read one byte if available */
uint8_t buffer_write(buffer_t* buf, uint8_t write_byte);
uint8_t buffer_write_multiple(buffer_t* dest_buf, buffer_t* src_buf, size_t w_size);   /* Write from source to dest buffer for sending data */
void buffer_print(buffer_t* buf);
