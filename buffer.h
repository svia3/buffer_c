#ifndef buffer_h
#define buffer_h

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct buffer_t buffer_t;
    //typedef buffer_t* buf_handle_t;
    // typedef uint8_t (*available)(buf_handle_t);

    /* Types */
    //typedef unsigned int size_t;

    struct buffer_t {
        uint8_t* buffer;
        size_t read_index;  // head of array -> start reading from here.
        size_t fill_index;  // next available byte to write to
        // uint8_t size;
        size_t capacity;
        // bool full;
    };

    /**
     * To be called from the FifteenDotFour::available member function; determine
     * the number of bytes available in the buffer to be read. This is done by
     * calculating the space between the write pointer ahead of the read pointer.
     *
     * @param buf   Pass by pointer; address f buffer struct
     * @return      Number of bytes available in the buffer
     */
     size_t buffer_get_size(buffer_t* buf);

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
     *
     * @param buf Pass by pointer; address of buffer struct
     */
    void buffer_free(buffer_t* buf);

    /**
     * Return the first value at the read index w/out popping and incrementing
     * read index
     * @param  buf Pass by pointer; address of buffer struct
     * @return     Front of buffer
     */
    uint8_t buffer_peek(buffer_t* buf);

    /**
     * Pop one byte off the top of the buffer queue if there are bytes available
     * Move over the read pointer by one byte (one index)
     * Return -1 if no bytes available and none read.
     *
     * @param buf      Pass by pointer; address of buffer struct
     * @return         Byte value copied into rx_buffer
     */
    int16_t buffer_read(buffer_t* src_buf);

    /**
     * Pop a specificed amount of bytes off the top of the buffer queue of available.
     * Move the read pointer over by the amount of bytes poped.
     * Return -1 if no bytes were available and none  read.
     *
     * @param  buf_handle_buf Pass by pointer; address of buffer struct
     * @return                Number of read bytes; copied into dest_buf
     */
    uint8_t buffer_read_multiple(uint8_t* dest_buf, buffer_t* src_buf, size_t r_size); /* Read one byte if available */

    /**
     * Write a single byte to the destination buffer; checking if there is enough
     * space inside of the buffer first and then copying over the single byte.
     *
     * @param  dest_buf   Destination buffer struct
     * @param  write_byte Byte to be written
     * @return            Number of bytes written to tx_buffer (1/0)
     */
    size_t buffer_write(buffer_t* dest_buf, const uint8_t write_byte);

    /**
     * Take bytes from source buffer and copy to destination buffer. Handles transfer
     * of data from MAC layer to APP.
     *
     * @param  src_buf  Pass by pointer
     * @param  dest_buf Pass by pointer
     * @param  w_size   Length of data
     * @return          Number of bytes written to tx_buffer
     */
    size_t buffer_write_multiple(buffer_t* dest_buf, const uint8_t* src_arr, size_t w_size);

    /**
     * For app layer testing and debugging. Print the buffer array. Implemented
     * to minmic the output of printing an array for visualization in python.
     *
     * @param buf Buffer to print from
     */
    void buffer_print(buffer_t* buf);

    /**
     * For app layer testing and debugging. Call buffer_print and display other
     * buffer_t structure data.
     *
     * @param buf Buffer to display data for
     */
    void print_buffer_stats(buffer_t* buf);

#ifdef __cplusplus
}
#endif

#endif
