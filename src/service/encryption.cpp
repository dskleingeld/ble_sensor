//adapted from https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/intro-to-application-level-security-using-the-ecb-
//need to include https://security.stackexchange.com/questions/20129/how-and-when-do-i-use-hmac
//hmac with sha1 is fine https://security.stackexchange.com/questions/33123/hotp-with-as-hmac-hashing-algoritme-a-hash-from-the-sha-2-family
//reference implementation https://tools.ietf.org/html/rfc6234#section-8.1


//TODO when int rolls over we need to renagotiate a new nonce

#include "encryption.hpp"

/**
 * @brief Uses the RNG to write a 12-byte nonce to a buffer
 * @details The 12 bytes will be written to the buffer starting at index 4 to leave
 *          space for the 4-byte counter value.
 *
 * @param[in]    p_buf    An array of length 16
 */
void StreamCipher::generate_random_nonce(uint8_t* nonce)
{
    uint8_t i         = COUNTER_BYTE_LEN;
    uint8_t remaining = NONCE_RAND_BYTE_LEN;

    // The random number pool may not contain enough bytes at the moment so
    // a busy wait may be necessary.
    while(0 != remaining){
        uint32_t err_code;
        uint8_t  available = 0;

        err_code = sd_rand_application_bytes_available_get(&available);
        APP_ERROR_CHECK(err_code);

        available = ((available > remaining) ? remaining : available);
        if (0 != available)
        {
	        err_code = sd_rand_application_vector_get((nonce + i), available);
	        APP_ERROR_CHECK(err_code);

	        i         += available;
	        remaining -= available;
	    }

	    if (0 != remaining)
	    {
	        nrf_delay_ms(RNG_BYTE_WAIT_US * remaining);
	    }
    }    
}

/**
 * @brief Initializes the module with the given nonce and key
 * @details The nonce will be copied to an internal buffer so it does not need to
 *          be retained after the function returns. Additionally, a 32-bit counter
 *          will be initialized to zero and placed into the least-significant 4 bytes
 *          of the internal buffer. The nonce value should be generated in a
 *          reasonable manner (e.g. using this module's nonce_generate function).
 *
 * @param[in]    p_nonce    An array of length 16 containing 12 random bytes
 *                          starting at index 4
 * @param[in]    p_ecb_key  An array of length 16 containing the ECB key
 */
uint32_t StreamCipher::init(const uint8_t* p_nonce, const uint8_t * p_ecb_key)
{
    // Save the key.
    memcpy(&m_ecb_data.key[0], p_ecb_key, ECB_KEY_LEN);

    // Copy the nonce.
    memcpy(&m_ecb_data.cleartext[COUNTER_BYTE_LEN],
	        &p_nonce[COUNTER_BYTE_LEN],
	        NONCE_RAND_BYTE_LEN);

    // Zero the counter value.
    memset(&m_ecb_data.cleartext[0], 0x00, COUNTER_BYTE_LEN);

    //encrypt the nonce
    uint32_t err_code = sd_ecb_block_encrypt(&m_ecb_data);
    if (NRF_SUCCESS != err_code)
    {
	    return err_code;
    }
    return NRF_SUCCESS;
}

// used to encode cleartext and decode ciphertext:
// length of len must be equal or smaller then ECB_KEY_LEN (=16)
void StreamCipher::crypt(uint8_t * buf, uint8_t len) {
    for (uint8_t i=0; i < len; i++) {
	    buf[i] ^= m_ecb_data.ciphertext[i];
    }
}

void write_24_bit_int(uint8_t* destination, uint32_t number){
    *(destination+2) = (uint8_t)(number >> 16);
    *(destination+1) = (uint8_t)(number >> 8);
    *(destination+0) = (uint8_t)(number);    
}

void read_24_bit_int(uint8_t* destination, uint32_t number){
    *(destination+2) = (uint8_t)(number >> 16);
    *(destination+1) = (uint8_t)(number >> 8);
    *(destination+0) = (uint8_t)(number);    
}

constexpr uint32_t _24BIT_MAX = 1 << 23;
uint32_t StreamCipher::increment_counter(){
    uint32_t err_code;
    if (counter>_24BIT_MAX){
        counter = 0; 
    } else { 
        counter++;
    }
    
    write_24_bit_int(m_ecb_data.cleartext, counter);

    err_code = sd_ecb_block_encrypt(&m_ecb_data);
    if (NRF_SUCCESS != err_code)
    {
	    return err_code;
    }
    return NRF_SUCCESS;
}

void StreamCipher::init_from_key(const uint8_t ecb_key[16]){
    uint8_t nonce[16];// = {0}; //TODO remove setting to zero (not needed)
    generate_random_nonce(nonce);
    init(nonce, ecb_key);
}

void StreamCipher::init_from_nonce(const uint8_t ecb_key[16], uint8_t* nonce){
    init(nonce, ecb_key);
}

uint32_t StreamCipher::get_nonce_counter(){
    return counter;
}

uint8_t* StreamCipher::get_unencrypted_nonce(){
    return &m_ecb_data.cleartext[0];
}

uint8_t* StreamCipher::get_encrypted_nonce(){
    return &m_ecb_data.ciphertext[0];
}

void StreamCipher::print_unencrypted_nonce(){
    uint8_t* nonce = get_unencrypted_nonce();
    
    Serial.print("unencrypted nonce: ");
    for(int i=0; i<COUNTER_BYTE_LEN+NONCE_RAND_BYTE_LEN; i++){
        Serial.print(*(nonce+i));
        Serial.print(",");
    }
    Serial.println("");
}

void StreamCipher::print_encrypted_nonce(){
    uint8_t* nonce = get_encrypted_nonce();
    
    Serial.print("encrypted nonce: ");
    for(int i=0; i<COUNTER_BYTE_LEN+NONCE_RAND_BYTE_LEN; i++){
        Serial.print(*(nonce+i));
        Serial.print(",");
    }
    Serial.println("");
}

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
uint32_t StreamCipher::encrypt(uint8_t * p_clear_text, uint8_t len) {
    crypt(p_clear_text, len);
    uint32_t retcode = increment_counter();
    return retcode;
}

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
uint32_t StreamCipher::decrypt(uint8_t * p_cipher_text, uint8_t len) {
    //encryption and decryption are identical (symmetric code)
    return encrypt(p_cipher_text, len);
}