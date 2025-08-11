#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>

// === BIBLIOTECAS DA PASTA LIB ===
#include "rfm95.h"
#include "bmp280.h"
#include "aht20.h"

// === DEFINIÇÕES DE PINOS E CONSTANTES DOS SENSORES ===
#define I2C_PORT_SENSORS i2c0
#define I2C_SDA_SENSORS 0
#define I2C_SCL_SENSORS 1
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15

// === DADOS DOS SENSORES ===
float temperatura;
int32_t pressao;
float umidade;

// === PROTÓTIPO DA FUNÇÃO DE INICIALIZAÇÃO ===
void setup();

// ========================================================================
// FUNÇÃO PRINCIPAL
// ========================================================================
int main() {

    // Inicialização do módulo LoRa e do sistema
    setup();

    if(!rfm95_initialize()) {
        printf("RFM95 initialization failed!\n");
        return 1;
    }

    printf("RFM95 initialized successfully!\n");
    rfm95_set_tx_power(17); // Define potência de transmissão (2-17 dBm)

    // Inicialização das estruturas de dados para os sensores
    struct bmp280_calib_param params;
    bmp280_get_calib_params(I2C_PORT_SENSORS, &params);

    AHT20_Data aht20_data;
    int32_t raw_temp_bmp;
    int32_t raw_pressure;

    char buffer[64];

    while (true) {

        // === LEITURA DO SENSOR BMP280 ===
        bmp280_read_raw(I2C_PORT_SENSORS, &raw_temp_bmp, &raw_pressure);
        int32_t temperature_bmp = bmp280_convert_temp(raw_temp_bmp, &params); 
        pressao = (bmp280_convert_pressure(raw_pressure, raw_temp_bmp, &params) / 1000); // kPa

        // === LEITURA DO SENSOR AHT20 ===
        if (aht20_read(I2C_PORT_SENSORS, &aht20_data)) {
            temperatura = ((aht20_data.temperature + (temperature_bmp / 100.0)) / 2.0); // Média das temperaturas
            umidade = (aht20_data.humidity) > 100 ? 100 : (aht20_data.humidity); // Limita a umidade a 100%
        }
        else {
            printf("Erro ao ler AHT20\n");
            temperatura = 0.0;
            umidade = 0.0;
        }

        // Formatação da string JSON
        snprintf(buffer, sizeof(buffer),
                            "{\"temperatura\":%.2f,\"pressao\":%d,\"umidade\":%.2f}\r\n",
                            temperatura, pressao, umidade);

        // Transmissão dos dados via LoRa
        rfm95_transmit((uint8_t*)buffer, strlen(buffer));

        sleep_ms(2000);
    }
}


// ========================================================================
// FUNÇÕES DE INICIALIZAÇÃO
// ========================================================================

/**
 * @brief Inicializa todos os periféricos e sensores do sistema
 */
void setup() {
    stdio_init_all();

    // === CONFIGURAÇÃO DOS SENSORES ===
    setup_I2C_aht20(I2C_PORT_SENSORS, I2C_SDA_SENSORS, I2C_SCL_SENSORS, 400 * 1000);
    aht20_reset(I2C_PORT_SENSORS);
    aht20_init(I2C_PORT_SENSORS);
    bmp280_init(I2C_PORT_SENSORS);
}
