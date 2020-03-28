#include "authentication.hpp"

//BLECharacteristic auth_challange = BLECharacteristic(auth_challange_uuid);
//BLECharacteristic auth_response = BLECharacteristic(auth_response_uuid);

namespace authentication{

    uint8_t challange[16] = {0};
    bool authenticated = false;
    uint8_t unauthenticated_conn_hdl;
    //SoftwareTimer auto_disconnect_timer; 
    //1 second after connection an unauthenticated connection is dropped, 
    //this times is started in connect callback and responsible for calling disconnect function

    void setup(){
        /* // Calling .begin() on a BLECharacteristic will cause it to be added to the last BLEService that
        // was 'begin()'ed!
        auth_challange.setProperties(CHR_PROPS_READ);
        auth_challange.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); //everybody may read, nobody write
        auth_challange.setFixedLen(16); //128 bit nonce that is readable
        //init challange 
        authentication::randomise_challange();
        auth_challange.begin();
        auth_challange.write(&authentication::challange[0], 16); // Use .write for init data

        auth_response.setProperties(CHR_PROPS_WRITE); //write then get response
        auth_response.setPermission(SECMODE_NO_ACCESS, SECMODE_OPEN); //everybody may read, nobody write
        auth_response.setFixedLen(16); // 128 bit nonce encrypted with pre shared key
        auth_response.setWriteCallback(authentication::check_response); //TODO change challange
        auth_response.begin();

        authentication::auto_disconnect_timer.begin(2000, authentication::disconnect_unauthenticated); */
    }

    void reset_auth(){
        //auth_response.setWriteCallback(authentication::check_response);
    }

    /*void disconnect_unauthenticated(TimerHandle_t xTimerID){
        if(!authenticated){
          BLEConnection* connection = Bluefruit.Connection(unauthenticated_conn_hdl);
          connection->disconnect();
            Serial.println("disconnected: unauthenticated, timed out");
        }
        auto_disconnect_timer.stop();
    }*/
   
    /**
     * @brief Uses the RNG to write a 16-byte nonce to a buffer
     *
     * @param[in]    p_buf    An array of length 16
     */
    static void generate_16_byte_nonce(uint8_t * p_buf) {
        uint8_t i         = 0;
        uint8_t remaining = SOC_ECB_KEY_LENGTH;

        // The random number pool may not contain enough bytes at the moment so
        // a busy wait may be necessary.
        while(0 != remaining)
        {
            uint32_t err_code;
            uint8_t  available = 0;

            err_code = sd_rand_application_bytes_available_get(&available);
            APP_ERROR_CHECK(err_code);

            available = ((available > remaining) ? remaining : available);
            if (0 != available) {
                err_code = sd_rand_application_vector_get((p_buf + i), available);
                APP_ERROR_CHECK(err_code);

                i         += available;
                remaining -= available;
            }

            if (0 != remaining)
            {
                nrf_delay_us(RNG_BYTE_WAIT_US * remaining);
            }
        }
    }

    void randomise_challange() {
        generate_16_byte_nonce(&challange[0]);
    }

    void correct_challange_response(uint8_t* resp){
        nrf_ecb_hal_data_t m_ecb_data; 
    
        // Save the key.
        memcpy(&m_ecb_data.key[0], HMAC_KEY, ECB_KEY_LEN);
        // Copy the challange.
        memcpy(&m_ecb_data.cleartext[0], &challange[0], 16);
        sd_ecb_block_encrypt(&m_ecb_data);

        //retrieve response
        memcpy(resp, &m_ecb_data.ciphertext[0], 16);
    }

    uint8_t test[16] = {};
    /*void check_response(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
    {
        if (len != 16){ Serial.println(len); return; }

        
        uint8_t correct_resp[16];
        correct_challange_response(correct_resp);
        
        if (memcmp(data, correct_resp, 16) != 0){
        //    // if unauthorized => disconnect (TODO)
        //    // Get the reference to current connection
            BLEConnection* connection = Bluefruit.Connection(conn_hdl);
            connection->disconnect();
            Serial.println("INVALID_NONCE");
        } else {
            randomise_challange();
            auth_challange.write(&authentication::challange[0], 16);
            auto_disconnect_timer.stop();

            Serial.println("AUTHENTICATED");
            authenticated = true;
        }

    }*/
};