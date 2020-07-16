

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define RX154_MAX_BUFF_SIZE 255
#define MTU             128


void print_arr(uint8_t* arr, size_t size) {
    printf("Array: [");
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
    buffer_init(&test_buf, MTU);
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
    uint8_t overflow_arr[200];
    uint8_t size = sizeof(overflow_arr)/sizeof(uint8_t);
    for(int i = 0; i < 200; ++i) {
        overflow_arr[i] = i;
    }
    // print_arr(overflow_arr, 128);
    printf("%zu\n", buffer_write_multiple(&test_buf, &overflow_arr[50], 100));
    // print_buffer_stats(&test_buf);
    // printf("%zu\n", buffer_write_multiple(&test_buf, overflow_arr, 50));
    // print_buffer_stats(&test_buf);
    /**
     * Testing reading single byte -- WORKS**!
     */
    // printf("read byte: %d\n", buffer_read(&test_buf));
    // printf("read byte: %d\n", buffer_read(&test_buf));
    // printf("read byte: %d\n", buffer_read(&test_buf));
    print_buffer_stats(&test_buf);

    /**
     * Testing reading multiple bytes --
     */
     // print_arr(overflow_arr, size);
    uint8_t read_buf[200];
    for(int i = 0; i < 200; ++i) {
        read_buf[i] = 0;
    }
    printf("Read Multiple:        %zu\n", buffer_read_multiple(read_buf, &test_buf, 30));
    print_arr(read_buf, 200);
//
    int write_bytes = 50;
    // printf("Read Multiple:        %zu\n", buffer_read_multiple(read_buf, &test_buf, 20));
    // print_arr(read_buf, 200);
    // print_buffer_stats(&test_buf);
    // printf("Write one byte:       %zu\n", buffer_write(&test_buf, 8));
    // print_buffer_stats(&test_buf);
    // buffer_flush(&test_buf);
    // printf("Write one byte:       %zu\n", buffer_write(&test_buf, 22));
    // print_buffer_stats(&test_buf);
    printf("Write multiple bytes: %zu\n", buffer_write_multiple(&test_buf, overflow_arr, write_bytes));
    print_buffer_stats(&test_buf);
    // for(int i = 0; i < 200; ++i) {
    //     overflow_arr[i] = 2;
    // }
    // write_bytes = 5;
    // printf("Wanted to write: %d; Wrote multiple bytes: %zu\n", write_bytes, buffer_write_multiple(&test_buf, overflow_arr, write_bytes));
    // print_buffer_stats(&test_buf);
    //
    // write_bytes = 15;
    // printf("Wanted to write: %d; Wrote multiple bytes: %zu\n", write_bytes, buffer_write_multiple(&test_buf, overflow_arr, write_bytes));
    // print_buffer_stats(&test_buf);
    // buffer_flush(&test_buf);
    // printf("Write multiple bytes: %zu\n", buffer_write_multiple(&test_buf, overflow_arr, 10));
    // buffer_read_multiple(read_buf, &test_buf, 10);
    // print_arr(read_buf, 10);
    // print_buffer_stats(&test_buf);



    return 0;
}
