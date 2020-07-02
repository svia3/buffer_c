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
    // uint8_t size;
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
void buffer_flush(buffer_t* buf);

/**
 * Free the buffer array. Never call on embedded systems for contiguous memory.
 * @param buf Pass by pointer; address of buffer struct
 */
void buffer_free(buffer_t* buf);

/**
 * Pop one byte off the top of the buffer queue if there are bytes available
 * Move over the read pointer by one byte (one index)
 * Return -1 if no bytes available and none read.
 *
 * @param buf      Pass by pointer; address of buffer struct
 * @return         Number of read bytes; copied into struct buffer
 */
uint8_t buffer_read(buffer_t* src_buf);

/**
 * Pop a specificed amount of bytes off the top of the buffer queue of available.
 * Move the read pointer over by the amount of bytes poped.
 * Return -1 if no bytes were available and none  read.
 *
 * @param  buf_handle_buf Pass by pointer; address of buffer struct
 * @return                Number of read bytes; copied into struct buffer
 */
uint8_t buffer_read_multiple(uint8_t* dest_buf, buffer_t* src_buf, size_t r_size); /* Read one byte if available */

/**
 * Write a single byte to the destination buffer; checking if there is enough
 * space inside of the buffer first and then copying over the single byte.
 * @param  dest_buf   Destination buffer struct
 * @param  write_byte Byte to be written
 * @return            1 for success
 */
bool buffer_write(buffer_t* dest_buf, uint8_t write_byte);

/**
 * Take bytes from source buffer and copy to destination buffer. Handles transfer
 * of data from MAC layer to APP.
 *
 * @param  src_buf  Pass by pointer
 * @param  dest_buf Pass by pointer
 * @param  w_size   Length of data
 * @return 1 if successful, 0 if not
 */
bool buffer_write_multiple(buffer_t* dest_buf, uint8_t* src_arr, size_t w_size);   /* Write from source to dest buffer for sending data */

/**
 * For app layer testing and debugging. Print the buffer array.
 * @param buf Buffer to print from
 */
void buffer_print(buffer_t* buf);
