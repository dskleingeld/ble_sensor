#include "nrf_log.h"
#include "test.h"

/**@brief Function for application main entry.
 */
int main(void) {
    // Start execution.
    NRF_LOG_INFO("test example started.");
    int b = plus_one(2);
    NRF_LOG_INFO(teststr);
}