#include "nrf_delay.h"

class Test {
    int a;
    int b;
    
    public:
    Test(int _a, int _b){
        a=_a+10;
        b=_b+10;
    };
};

/**@brief Function for application main entry.
 */
int main(void) {

    Test test_instance(1,2);
    // Enter main loop.
    for (;;)
    {
        nrf_delay_ms(1000);
    }
}