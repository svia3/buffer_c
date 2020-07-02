

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX154_MAX_BUFF_SIZE 255


void print_arr(uint8_t* arr, size_t size) {
    printf("\nArray: [");
    for(int i = 0; i < size; ++i) {
        printf("%d,", arr[i]);
    }
    printf("]");
    printf("\n");
}

int main()
{
    // 15.4 API calls
    // uint8_t arr[128];
    // Inside 15.4 -----
    struct buffer_t test_buf;
    buffer_init(&test_buf, RX154_MAX_BUFF_SIZE);
    // printf("%lu", buf.capacity);
    // char arr[] = "012345";
    // buffer_write_multiple(&test_buf, (uint8_t*)&arr, sizeof(arr)/sizeof(uint8_t));
    // buffer_print(&test_buf);

    /**
        Create a pseudo transimission buffer for testing -> this will be used
        in 15.4 as the data req structure.
    */

    /**
     * Testng writing to buffer --- WORKS **!
     */
    // uint8_t write_arr[] = {1, 2, 3, 4, 5};
    // uint8_t size = sizeof(write_arr)/sizeof(uint8_t);
    // print_arr(write_arr, size);
    // buffer_write_multiple(&test_buf, write_arr, size);
    // buffer_write_multiple(&test_buf, write_arr, size);
    // buffer_write(&test_buf, 123);
    // print_buffer_stats(&test_buf);
    uint8_t overflow_arr[127];
    uint8_t size = sizeof(overflow_arr)/sizeof(uint8_t);
    for(int i = 0; i < 127; i++) {
        overflow_arr[i] = i;
    }
    // print_arr(overflow_arr, 128);
    buffer_write_multiple(&test_buf, overflow_arr, size);
    // print_buffer_stats(&test_buf);
    buffer_write_multiple(&test_buf, overflow_arr, size);
    // print_buffer_stats(&test_buf);

    /**
     * Testing reading single byte -- WORKS**!
     */
    printf("read byte: %d\n", buffer_read(&test_buf));
    printf("read byte: %d\n", buffer_read(&test_buf));
    printf("read byte: %d\n", buffer_read(&test_buf));
    print_buffer_stats(&test_buf);

    /**
     * Testing reading multiple bytes --
     */
    uint8_t read_buf[10];
    buffer_read_multiple(read_buf, &test_buf, 10);
    print_arr(read_buf, 10);
    print_buffer_stats(&test_buf);
    buffer_read_multiple(read_buf, &test_buf, 10);
    print_arr(read_buf, 10);
    print_buffer_stats(&test_buf);



    return 0;
}
