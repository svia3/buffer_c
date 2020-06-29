

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX154_MAX_BUFF_SIZE 255

int main()
{

    struct buffer_t my_buf;
    bool success = buffer_init(&my_buf, RX154_MAX_BUFF_SIZE);
    // Practice writing to a dummy buffer pass by reference
    // uint8_t tx_buffer[11];
    // memset(tx_buffer, 0, 11);
    // uint8_t* tx_ptr = tx_buffer;
    // for(uint8_t i = 0; i < 11; ++i) {
    //     tx_buffer[i] = i;
    //     printf("%d", *(tx_ptr+i));
    //     // printf("%d", tx_buffer[i]);
    // }
    // write to rx_buffer from mac layer example --
    // buffer_write(rx_buffer, tx_buffer, 11);
    // buffer_print(rx_buffer);
    // for(uint8_t i = 0; i < 2; ++i) {
    //     uint8_t value = rx_buffer->buffer[i];
    //     printf("%d", value);
    // }
    // printf("available: %d", buffer_available(rx_buffer));
    // free(rx_buffer->buffer);
    // buffer_free(rx_buffer);

    return 0;
}
