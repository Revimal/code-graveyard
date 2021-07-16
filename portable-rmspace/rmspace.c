#include <stddef.h>
#include <inttypes.h>

static uint_fast8_t rmspace_jmptbl[256] = {
/*       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
/* 00 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1,
/* 10 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 20 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 30 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 40 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 50 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 60 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 70 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 90 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* a0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* b0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* c0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* d0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* e0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* f0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static inline uint_fast8_t rmspace_block(uint64_t input, char *output) {
    uint_fast8_t pos = 0;
    uint_fast8_t u8;

    output[pos] = (u8 = ((input >> 0x00) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x08) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x10) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x18) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x20) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x28) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x30) & 0xff)), pos += rmspace_jmptbl[u8];
    output[pos] = (u8 = ((input >> 0x38) & 0xff)), pos += rmspace_jmptbl[u8];

    return pos;
}

#define RMSPACE_ZEROCHK(x) \
    (~(x) & UINT64_C(0x8080808080808080) & ((x) - UINT64_C(0x0101010101010101)))

size_t rmspace_all(char *input, size_t bufsz) {
    static const uint64_t tab = (~UINT64_C(0) / 255 * (uint64_t)('\t'));
    static const uint64_t space = (~UINT64_C(0) / 255 * (uint64_t)(' '));
    static const uint64_t newline = (~UINT64_C(0) / 255 * (uint64_t)('\n'));
    static const uint64_t carriage = (~UINT64_C(0) / 255 * (uint64_t)('\r'));
    size_t pos = 0;
    size_t idx;

    for (idx = 0; (idx + 0x20) < bufsz; idx += 0x20) {
        uint64_t block0 = *(uint64_t *)&input[(idx + 0x00)];
        uint64_t block1 = *(uint64_t *)&input[(idx + 0x08)];
        uint64_t block2 = *(uint64_t *)&input[(idx + 0x10)];
        uint64_t block3 = *(uint64_t *)&input[(idx + 0x18)];
        uint64_t check0 = (RMSPACE_ZEROCHK(block0 ^ tab) || RMSPACE_ZEROCHK(block0 ^ space) ||
            RMSPACE_ZEROCHK(block0 ^ newline) || RMSPACE_ZEROCHK(block0 ^ carriage));
        uint64_t check1 = (RMSPACE_ZEROCHK(block1 ^ tab) || RMSPACE_ZEROCHK(block1 ^ space) ||
            RMSPACE_ZEROCHK(block1 ^ newline) || RMSPACE_ZEROCHK(block1 ^ carriage));
        uint64_t check2 = (RMSPACE_ZEROCHK(block2 ^ tab) || RMSPACE_ZEROCHK(block2 ^ space) ||
            RMSPACE_ZEROCHK(block2 ^ newline) || RMSPACE_ZEROCHK(block2 ^ carriage));
        uint64_t check3 = (RMSPACE_ZEROCHK(block3 ^ tab) || RMSPACE_ZEROCHK(block3 ^ space) ||
            RMSPACE_ZEROCHK(block3 ^ newline) || RMSPACE_ZEROCHK(block3 ^ carriage));

        pos += (check0 ? rmspace_block(block0, &input[pos]) : (*(uint64_t *)&input[pos] = block0, 0x8));
        pos += (check1 ? rmspace_block(block1, &input[pos]) : (*(uint64_t *)&input[pos] = block1, 0x8));
        pos += (check2 ? rmspace_block(block2, &input[pos]) : (*(uint64_t *)&input[pos] = block2, 0x8));
        pos += (check3 ? rmspace_block(block3, &input[pos]) : (*(uint64_t *)&input[pos] = block3, 0x8));
    }

    for (; idx < bufsz; ++idx)
        input[pos] = input[idx], pos += rmspace_jmptbl[input[pos]];

    return pos;
}