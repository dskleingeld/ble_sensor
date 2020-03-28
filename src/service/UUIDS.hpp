#ifndef UUID_H
#define UUID_H

#include <cstdint>

uint8_t const uuid128_service[16] =
{131, 20, 153, 220, 231, 245, 91, 152, 153, 21, 183, 27, 175, 191, 112, 147};

uint16_t const uuid16_auth_challange = 132;
uint16_t const uuid16_auth_response = 133;

uint16_t const uuid16_nonce = 134;

uint8_t const uuid128_hmac[16] = 
{135, 20, 153, 220, 231, 245, 91, 152, 153, 21, 183, 27, 175, 191, 112, 147};
uint8_t const uuid128_awk[16] = 
{136, 20, 153, 220, 231, 245, 91, 152, 153, 21, 183, 27, 175, 191, 112, 147};
uint8_t const uuid128_data[16] = 
{137, 20, 153, 220, 231, 245, 91, 152, 153, 21, 183, 27, 175, 191, 112, 147};

constexpr int NUMB_OF_BUTTONS = 6;
constexpr int TYPE_USER_INPUT = 1;
constexpr int TYPE_SLOW_DATA = 2;
constexpr int TYPE_FAST_DATA = 3;
#endif