//adapted from https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/intro-to-application-level-security-using-the-ecb-

#ifndef NODE_CRYPTO_H_
#define NODE_CRYPTO_H_

#include <cstdint>
#include <cstring>
extern "C" {
    #include "nrf_soc.h"
    #include "nrf_delay.h"
    #include "app_error.h"
}

#define ECB_KEY_LEN            (16UL)
#define COUNTER_BYTE_LEN       (3UL)
#define NONCE_RAND_BYTE_LEN    (13UL)

// The RNG wait values are typical and not guaranteed. See Product Specifications for more info.
#define RNG_BYTE_WAIT_US       (124UL)

class StreamCipher {
    public:
        //
        void init_from_key(const uint8_t key[16]);
        void init_from_nonce(const uint8_t key[16], uint8_t nonce[16]);

        uint8_t* get_encrypted_nonce();
        uint8_t* get_unencrypted_nonce();
        uint32_t get_nonce_counter();

        void print_encrypted_nonce();
        void print_unencrypted_nonce();

        void print_counter_nonce();
        /**
         * @brief Encrypts the given buffer in-situ
         * @details The encryption step is done separately (using the nonce, counter, and
         *          key) and then the result from the encryption is XOR'd with the given
         *          buffer in-situ. The counter will be incremented only if no error occurs.
         *
         * @param[in]    p_clear_text    An array of length 16 containing the clear text
         *
         * @retval    NRF_SUCCESS                         Success
         * @retval    NRF_ERROR_INVALID_STATE             Module has not been initialized
         * @retval    NRF_ERROR_SOFTDEVICE_NOT_ENABLED    SoftDevice is present, but not enabled
         */
        uint32_t encrypt(uint8_t * p_clear_text, uint8_t len);
        /**
         * @brief Decrypts the given buffer in-situ
         * @details The encryption step is done separately (using the nonce, counter, and
         *          key) and then the result from the encryption is XOR'd with the given
         *          buffer in-situ. The counter will be incremented only if no error occurs.
         *
         * @param[in]    p_cipher_text    An array of length 16 containing the cipher text
         *
         * @retval    NRF_SUCCESS                         Succeess
         * @retval    NRF_ERROR_INVALID_STATE             Module has not been initialized
         * @retval    NRF_ERROR_SOFTDEVICE_NOT_ENABLED    SoftDevice is present, but not enabled
         */
        uint32_t decrypt(uint8_t * p_cipher_text, uint8_t len);

    private:
        // NOTE: The ECB data must be located in RAM or a HardFault will be triggered.
        nrf_ecb_hal_data_t m_ecb_data; 
        // used to increment the nonce
        uint32_t increment_counter();
        uint32_t counter;
        // used to encode cleartext and decode ciphertext:
        void crypt(uint8_t * buf, uint8_t len);
            /**
         * @brief Uses the RNG to write a 13-byte nonce to a buffer
         * @details The 13 bytes will be written to the buffer starting at index 3 to leave
         *          space for the 3-byte counter value.
         *
         * @param[in]    p_buf    An array of length 16
         */
        static void generate_random_nonce(uint8_t* nonce);
        /**
         * @brief Initializes the module with the given nonce and key
         * @details The nonce will be copied to an internal buffer so it does not need to
         *          be retained after the function returns. Additionally, a 32-bit counter
         *          will be initialized to zero and placed into the least-significant 4 bytes
         *          of the internal buffer. The nonce value should be generated in a
         *          reasonable manner (e.g. using this module's nonce_generate function).
         *
         * @param[in]    p_nonce    An array of length 16 containing 13 random bytes
         *                          starting at index 3
         * @param[in]    p_ecb_key  An array of length 16 containing the ECB key
         */
        uint32_t init(const uint8_t* nonce, const uint8_t * p_ecb_key);
};

#endif /* NODE_CRYPTO_H_ */