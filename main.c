#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "rfm95.h"
#include "ssd1306.h"
#include "bmp280.h"
#include "aht20.h"



int main() {
    stdio_init_all();

    if(!rfm95_initialize()) {
        printf("RFM95 initialization failed!\n");
        return 1;
    }

    printf("RFM95 initialized successfully!\n");
    rfm95_set_tx_power(17); // Define potência de transmissão (2-17 dBm)


    char buffer[64];

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
