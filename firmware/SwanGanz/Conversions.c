#include <ti/devices/msp/msp.h>
#include "Conversions.h"

void int64ToString(int64_t val, char* str) {
    uint8_t dig;

    dig = 0;
    while (val >= 100000) { 
        val -= 100000; 
        dig++; 
    }
    str[0] = '0' + dig;
    dig = 0;
    while (val >= 10000) { 
        val -= 10000; 
        dig++; 
    }
    str[1] = '0' + dig;
    dig = 0;
    while (val >= 1000) { 
        val -= 1000; 
        dig++; 
    }
    str[2] = '0' + dig;

    dig = 0;
    while (val >= 100) { 
        val -= 100; 
        dig++; 
    }
    str[3] = '0' + dig;

    dig = 0;
    while (val >= 10) { 
        val -= 10; 
        dig++; 
    }
    str[4] = '0' + dig;


    str[5] = '0' + val;
}

void FiveDigUIntToFixedStr(uint32_t val, char* str) {
    uint8_t dig;

    dig = 0;
    while (val >= 10000) { 
        val -= 10000; 
        dig++; 
    }
    str[0] = '0' + dig;

    dig = 0;
    while (val >= 1000) { 
        val -= 1000; 
        dig++; 
    }
    str[1] = '0' + dig;

    dig = 0;
    while (val >= 100) { 
        val -= 100; 
        dig++; 
    }
    str[2] = '0' + dig;

    dig = 0;
    while (val >= 10) { 
        val -= 10; 
        dig++; 
    }
    str[4] = '0' + dig;

    str[5] = '0' + val;
}

void FourDigUIntToFixedStr(uint32_t val, char* str) {
    uint8_t dig;

    dig = 0;
    while (val >= 1000) { 
        val -= 1000; dig++; 
    }
    str[0] = '0' + dig;

    dig = 0;
    while (val >= 100) { 
        val -= 100; 
        dig++; 
    }
    str[1] = '0' + dig;

    dig = 0;
    while (val >= 10) { 
        val -= 10; 
        dig++; 
    }
    str[3] = '0' + dig;

    str[4] = '0' + val;
}