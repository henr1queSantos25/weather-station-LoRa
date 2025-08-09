#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "rfm95.h"
#include "ssd1306.h"
#include "bmp280.h"
#include "aht20.h"

// === DEFINIÇÕES DE PINOS E CONSTANTES ===
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

int main() {
    stdio_init_all();

    // === CONFIGURAÇÃO DOS SENSORES ===
    setup_I2C_aht20(I2C_PORT_SENSORS, I2C_SDA_SENSORS, I2C_SCL_SENSORS, 400 * 1000);
    aht20_reset(I2C_PORT_SENSORS);
    aht20_init(I2C_PORT_SENSORS);
    bmp280_init(I2C_PORT_SENSORS);


    if(!rfm95_initialize()) {
        printf("RFM95 initialization failed!\n");
        return 1;
    }

    printf("RFM95 initialized successfully!\n");
    rfm95_set_tx_power(17); // Define potência de transmissão (2-17 dBm)


    char buffer[64];
    
    AHT20_Data data;
    int32_t raw_temp_bmp;
    int32_t raw_pressure;

    // Inicialização das estruturas de dados para os sensores
    struct bmp280_calib_param params;
    bmp280_get_calib_params(I2C_PORT_SENSORS, &params);

    while (true) {


        // === LEITURA DO SENSOR BMP280 ===
        bmp280_read_raw(I2C_PORT_SENSORS, &raw_temp_bmp, &raw_pressure);
        int32_t temperature_bmp = bmp280_convert_temp(raw_temp_bmp, &params); 
        pressao = (bmp280_convert_pressure(raw_pressure, raw_temp_bmp, &params) / 1000); // kPa

        // === LEITURA DO SENSOR AHT20 ===
        if (aht20_read(I2C_PORT_SENSORS, &data)) {
            temperatura = ((data.temperature + (temperature_bmp / 100.0)) / 2.0); // Média das temperaturas
            umidade = (data.humidity) > 100 ? 100 : (data.humidity); // Limita a umidade a 100%
        }
        else {
            printf("Erro ao ler AHT20\n");
            temperatura = 0.0;
            umidade = 0.0;
        }

        snprintf(buffer, sizeof(buffer),
                            "{\"temperatura\":%.2f,\"pressao\":%d,\"umidade\":%.2f}\r\n",
                            temperatura, pressao, umidade);
        rfm95_transmit((uint8_t*)buffer, strlen(buffer));

        sleep_ms(2000);
    }
}
