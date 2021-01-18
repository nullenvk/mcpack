#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>

#include "../mcpack.h"

int main() {
    mc_buffer_t buf = mc_buffer_new();

    /*
    buf.allocate = true;
    buf.size = 0;
    buf.read_ptr = 0;
    buf.allocated_size = 0;
    */

    char *test_buf;

    const char *TEST_STRING = "ABCDEFG";
    
    if(mc_pack(&buf, "s", TEST_STRING) == -1) {
        puts("Packing failed");
        return -1;
    }

    if(mc_unpack(buf, "s", &test_buf) == -1) {
        puts("Unpacking failed");
        return -1;
    }

    if(strcmp(TEST_STRING, test_buf)) {
        printf("%s\n", test_buf);
        puts("Type serialization invalid");
        return -1;
    }

    mc_buffer_free(&buf);

    return 0;
}
