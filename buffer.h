#include <stdint.h>

typedef struct buffer_t buffer_t;
typedef buffer_t* buf_handle_t;
// typedef uint8_t (*available)(buf_handle_t);

/* Types */
typedef unsigned long size_t;

struct buffer_t {
    uint8_t* buffer;
    uint8_t read_index;  // head of array -> start reading from here.
    uint8_t fill_index;  // next available byte to write to
    size_t size;
    size_t capacity;
    //available buffer_available
    // bool full;
};

buf_handle_t buffer_init();                /* Init buffer with size? */
void buffer_flush(buf_handle_t buf);       /* Reset buffer to empty; head == tail */
void buffer_free(buf_handle_t buf);         /* Free the buffer struct, not data -> user job */
void buffer_update_capacity(buf_handle_t buf);  /* How many bytes are available in the buffer? */
uint8_t buffer_read(buf_handle_t buf);     /* Read one byte if available */
uint8_t buffer_read_multiple(buf_handle_t buf, size_t r_size);  /* Read one byte if available */
uint8_t buffer_write(buf_handle_t dest_buf, uint8_t* src_buf, size_t w_size);   /* Write from source to dest buffer for sending data */
void buffer_print(buf_handle_t buf);
