

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX154_MAX_BUFF_SIZE 255


void print_arr(uint8_t* arr) {
    for(int i = 0; i < 128; ++i) {
        printf("%d", arr[i]);
    }
    printf("\n");
}

int main()
{
    // 15.4 API calls
    // uint8_t arr[128];
    // Inside 15.4 -----
    struct buffer_t rx_buf;
    buffer_init(&rx_buf, RX154_MAX_BUFF_SIZE);
    // printf("%lu", buf.capacity);
    // char arr[] = "012345";
    // buffer_write_multiple(&rx_buf, (uint8_t*)&arr, sizeof(arr)/sizeof(uint8_t));
    printf("rx read index: %zu\n", rx_buf.read_index);
    printf("rx fill index: %zu\n", rx_buf.fill_index);
    buffer_print(&rx_buf);

    /**
        Create a pseudo transimission buffer for testing -> this will be used
        in 15.4 as the data req structure.
    */
    // struct buffer_t tx_buf;
    // buffer_init(&tx_buf, RX154_MAX_BUFF_SIZE);
    uint8_t write_to_arr[] = {1, 2, 3, 4, 5};
    uint8_t size = sizeof(write_to_arr)/sizeof(uint8_t);
    // printf("size: %d\n", size);
    bool write_status = buffer_write_multiple(&rx_buf, write_to_arr, size);
    printf("success: %d\n", write_status); 
    buffer_print(&rx_buf);
    // printf("%d\n", buffer_read_multiple(write_to_arr, tx_buf->buffer, 10));
    // printf("rx read index: %zu\n", rx_buf.read_index);
    // printf("rx fill index: %zu\n", rx_buf.fill_index);
    write_status = buffer_write_multiple(&rx_buf, write_to_arr, size);
    // printf("rx read index: %zu\n", rx_buf.read_index);
    // printf("rx fill index: %zu\n", rx_buf.fill_index);


    // now read
    uint8_t read_buf[128];
    memset(read_buf, 1, 128);
    print_arr(read_buf);
    printf("read bytes: %d\n", buffer_read_multiple(read_buf, &rx_buf, 5));
    print_arr(read_buf);
    // printf("%d", buffer_get_size(tx_buf));
    return 0;
}
