

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
    struct buffer_t write_buffer;
    buffer_init(&write_buffer, 50);



    buffer_print(&rx_buf);
    return 0;
}
