#include <stdint.h>

struct Field {
    uint8_t offset; //bits
    uint8_t length; //bits (max 32 bit variables)

    float decode_scale;
    float decode_add;
};

float decode(const struct Field* self, uint8_t line[]);
void encode(const struct Field* self, float numb, uint8_t line[]);
const uint8_t byte_length(const struct Field field_list[]);