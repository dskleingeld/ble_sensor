#include <stdint.h>
#include "encoding.h"

float decode(const struct Field* self, uint8_t line[])
{
    const uint32_t int_repr = decode_value(line, self->offset, self->length);
    float decoded = (float)int_repr;

    decoded *= (float)self->decode_scale;
    decoded += (float)self->decode_add;

    return decoded;
}

void encode(const struct Field* self, float numb, uint8_t line[])
{
    numb -= (float)self->decode_add;
    numb /= (float)self->decode_scale;
    //println!("scale: {}, add: {}, numb: {}", self.decode_scale, self.decode_add, numb);

    const uint32_t to_encode = (uint32_t)numb;

    encode_value(to_encode, line, self->offset, self->length);
}

const uint8_t byte_length(const struct Field field_list[]){
    uint8_t last = sizeof(field_list)/sizeof(struct Field) -1;
    uint16_t bits = field_list[last].offset+field_list[last].length;
    
    uint8_t bytes = (bits - 1) / 8 + 1;
    return bytes;
}