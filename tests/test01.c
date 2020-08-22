#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "../obsidian_serialize.h"

int main() {
    ByteBuffer buf;

    struct {
        uint64_t check_1;
        uint64_t check_2;
        uint64_t check_4; 
        uint64_t check_8; 
    } tests, original;

    tests.check_1 = 124;
    tests.check_2 = 25565;
    tests.check_4 = 4294900000;
    tests.check_8 = 1844674407370000000;

    original = tests;

    buf.allocate = true;
    buf.size = 0;
    buf.allocated_size = 0;
    
    if(mc_pack(&buf, "1234", 
            tests.check_1,
            tests.check_2,
            tests.check_4,
            tests.check_8) == -1) {
        
        puts("Packing failed");
        return -1;
    }


    bzero(&tests, sizeof(tests));

    /*
    if(mc_unpack(buf, "1234",
            &tests.check_1,
            &tests.check_2,
            &tests.check_4,
            &tests.check_8) == -1) {

        puts("Unpacking failed");
        return -1;
    }
    */

    if(mc_unpack(buf, "1",
            &tests.check_1)== -1) {

        puts("Unpacking failed");
        return -1;
    }

    if(memcmp(&tests, &original, sizeof(tests)) != 0) {
        puts("Type serialization invalid");
        return -1;
    }

    bytebuffer_free(&buf);

    return 0;
}
