#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>

#include "../mcpack.h"


struct test_set {
    uint8_t check_1;
    uint16_t check_2;
    uint32_t check_4; 
    uint64_t check_8; 
};

int test_set_equal(struct test_set a, struct test_set b) {
    int ret = 0;

    ret += (a.check_1 == b.check_1);
    ret += (a.check_2 == b.check_2);
    ret += (a.check_4 == b.check_4);
    ret += (a.check_8 == b.check_8);

    return (ret > 0) ? 0 : 1;
}

int main() {
    MCBuffer buf;

    struct test_set tests, original;

    tests.check_1 = 250;
    tests.check_2 = 65530;
    tests.check_4 = 4294967290;
    tests.check_8 = 1844674407370955161;

    buf.allocate = true;
    buf.size = 0;
    buf.read_ptr = 0;
    buf.allocated_size = 0;
    
    if(mc_pack(&buf, "1248", 
            tests.check_1,
            tests.check_2,
            tests.check_4,
            tests.check_8) == -1) {
        
        puts("Packing failed");
        return -1;
    }

    bzero(&tests, sizeof(tests));
    memcpy(&original, &tests, sizeof(tests));

    if(mc_unpack(buf, "1248",
            &tests.check_1,
            &tests.check_2,
            &tests.check_4,
            &tests.check_8) == -1) {

        puts("Unpacking failed");
        return -1;
    }

    if(!test_set_equal(tests, original)) {
        puts("Type serialization invalid");
        return -1;
    }

    mcbuffer_free(&buf);

    return 0;
}
