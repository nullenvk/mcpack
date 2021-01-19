#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include "../mcpack.h"

noreturn void panic(const char *msg) {
    printf("%s\n", msg);
    exit(-1);
}

int main() {
    mc_buffer_t buf = mc_buffer_new();

    unsigned char *test_buf;
    long long test_buf_len;

    const unsigned char TEST_STRING[3] = {0x1, 0x2, 0x3};
    
    if (mc_pack(&buf, "B", 3, TEST_STRING) == -1) panic("Packing failed");
    if (mc_unpack(buf, "B", &test_buf_len, &test_buf) == -1) panic("Unpacking failed");
    if (test_buf_len != 3) panic("Invalid unpacked length");
    if (memcmp(TEST_STRING, test_buf, 3)) panic("Type serialization invalid");

    mc_buffer_free(&buf);
    free(test_buf);

    return 0;
}
