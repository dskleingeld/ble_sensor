#include <cstdint>

#include "keys.hpp"
#include "encryption.hpp"
#include "authentication.hpp"
#include "UUIDS.hpp"

//setting up encryption etc
extern StreamCipher encryptor;
extern bool client_subscribed; //TODO move to main

constexpr uint32_t RESEND_PERIOD = 50; //milliseconds
constexpr uint32_t BURST_PERIOD = 50; //milliseconds
constexpr int BURST_SIZE = 3;
constexpr int QUEUE_SIZE = 20;
constexpr int HMAC_SIZE = 20;

//TODO figure out how to send smaller packages
class Package {
    public:
        Package();
        Package(uint8_t _data_length, uint8_t type, bool _urgent=false);
        uint8_t* get_bufferpointer();
        uint32_t numb();
        void finalise();

        uint8_t buffer[20]; //a multiple of 16 bytes, with 4 bytes added for the cleartxt nonce
        uint8_t hmac[20];
        uint8_t length;
        bool acknowledged = false;
        bool urgent = false;
        uint32_t last_notify;
        uint8_t send_attempts = 0;
    private:
        uint8_t data_length;
        uint8_t length_encrypted_part;
        //should works on the (stream)encrypted data this makes sure the hmac is
        //unpredictable even if the same sensor values are send
        void update_hmac();
        void encrypt();
};

class PackageQueue {
  public:
    PackageQueue();
    void setup();
    void enqueue(Package&& package);
    void poll();
    void reset();

    void acknowledge(uint32_t package_id);
  //private:
    Package queue[QUEUE_SIZE];

    //BLECharacteristic data_chr;
    //BLECharacteristic hmac_chr;
    //BLECharacteristic awk_chr;

};

void write_24_bit_int(uint8_t* destination, uint32_t number);
uint32_t read_24_bit_int(uint8_t* source);
//void awk_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);
//void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value);
