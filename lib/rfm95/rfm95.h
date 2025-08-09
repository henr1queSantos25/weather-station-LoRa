#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdbool.h>
#include <string.h>

// Frequência padrão de operação do Brasil (Hz)
#define FREQUENCY_HZ 915000000

// Frequência do cristal do módulo (32 MHz)
#define RF_CRYSTAL_FREQ_HZ 32000000

// Definições de Pinos e SPI
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20

// --- ASSINATURA DAS FUNÇÕES ---
bool rfm95_initialize();
void rfm95_set_idle_mode();
void rfm95_set_sleep_mode();
void rfm95_set_frequency(long frequency);
void rfm95_set_tx_power(uint8_t power);
void rfm95_transmit(const uint8_t* buffer, uint8_t size);
int rfm95_receive(uint8_t* buffer, int max_size);
int rfm95_get_rssi();
float rfm95_get_snr();