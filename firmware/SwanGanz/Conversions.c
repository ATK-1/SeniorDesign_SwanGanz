#include <ti/devices/msp/msp.h>
#include "Conversions.h"

void FourDigUIntToFixedStr(int32_t val, char* str) {
    uint8_t dig;

    dig = 0;
    while (val >= 1000) { 
        val -= 1000; 
        dig++; 
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
    str[2] = '0' + dig;


    str[3] = '0' + val;
}

void ThreeDigUIntToFixedStr(int32_t val, char* str) {
    uint8_t dig;

    dig = 0;
    while (val >= 100) { 
        val -= 100; 
        dig++; 
    }
    str[0] = '0' + dig;

    dig = 0;
    while (val >= 10) { 
        val -= 10; 
        dig++; 
    }
    str[1] = '0' + dig;


    str[2] = '0' + val;
}

void FiveDigDecUIntToFixedStr(uint32_t val, char* str) {
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

void FourDigDecUIntToFixedStr(uint32_t val, char* str) {
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