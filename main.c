

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX154_MAX_BUFF_SIZE 255

int main()
{
    struct buffer_t rx_buf;
    buffer_init(&rx_buf, RX154_MAX_BUFF_SIZE);
    // printf("%lu", buf.capacity);
    // char arr[] = "012345";
    // buffer_write_multiple(&rx_buf, (uint8_t*)&arr, sizeof(arr)/sizeof(uint8_t));
    printf("rx read index: %zu\n", rx_buf.read_index);
    printf("rx fill index: %zu\n", rx_buf.fill_index);
    buffer_print(&rx_buf);

    /*
        Create a pseudo transimission buffer for testing -> this will be used
        in 15.4 as the data req structure.
    */
    struct buffer_t tx_buf;
    buffer_init(&tx_buf, RX154_MAX_BUFF_SIZE);
    char arr[] = "012345";
    buffer_write_multiple(&tx_buf, (uint8_t*)&arr, sizeof(arr)/sizeof(uint8_t));
    // buffer_print(&tx_buf);
    printf("%d\n", buffer_read_multiple(&rx_buf, &tx_buf, 10));
    printf("rx read index: %zu\n", rx_buf.read_index);
    printf("rx fill index: %zu\n", rx_buf.fill_index);
    buffer_print(&rx_buf);
    // buffer_flush(&rx_buf);
    // buffer_print(&rx_buf);
    return 0;
}
