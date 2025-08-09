#include "rfm95.h"
#include "rfm95_definitions.h"

// ============================================================================
// COMUNICAÇÃO SPI E CONTROLE DE HARDWARE
// ============================================================================

/**
 * @brief Reseta o módulo RFM95 via pino RST
 * 
 * Executa um reset por hardware do módulo, mantendo o pino RST em LOW por 10ms
 * e aguardando 10ms após voltar para HIGH para estabilização.
 */
static void rfm95_reset() {
    gpio_put(PIN_RST, 0);    // Ativa reset (LOW)
    sleep_ms(10);            // Mantém reset por 10ms
    gpio_put(PIN_RST, 1);    // Desativa reset (HIGH)
    sleep_ms(10);            // Aguarda estabilização
}

/**
 * @brief Lê um registrador do RFM95 via SPI
 * 
 * @param reg Endereço do registrador a ser lido (0x00-0x7F)
 * @return Valor lido do registrador
 * 
 * Para leitura, o bit MSB deve ser 0. Envia o endereço seguido de um byte dummy
 * e recebe a resposta no segundo byte.
 */
static uint8_t rfm95_read_register(uint8_t reg) {
    uint8_t tx[] = { reg & 0x7F, 0x00 };  // Garante bit MSB = 0 para leitura
    uint8_t rx[2];
    
    gpio_put(PIN_CS, 0);                   // Seleciona o dispositivo
    spi_write_read_blocking(SPI_PORT, tx, rx, 2);
    gpio_put(PIN_CS, 1);                   // Desseleciona o dispositivo
    
    return rx[1];                          // Retorna o valor lido
}

/**
 * @brief Escreve um valor em um registrador do RFM95 via SPI
 * 
 * @param reg Endereço do registrador (0x00-0x7F)
 * @param value Valor a ser escrito
 * 
 * Para escrita, o bit MSB deve ser 1. Envia o endereço com MSB=1 seguido do valor.
 */
static void rfm95_write_register(uint8_t reg, uint8_t value) {
    uint8_t tx[] = { reg | 0x80, value }; // Define bit MSB = 1 para escrita
    
    gpio_put(PIN_CS, 0);                   // Seleciona o dispositivo
    spi_write_blocking(SPI_PORT, tx, 2);
    gpio_put(PIN_CS, 1);                   // Desseleciona o dispositivo
    sleep_ms(1); // Aguarda estabilização após escrita
}

/**
 * @brief Lê dados do FIFO do RFM95
 * 
 * @param buffer Buffer para armazenar os dados lidos
 * @param length Quantidade de bytes a serem lidos
 * 
 * Acessa o FIFO através do registrador REG_FIFO para receber dados de pacotes.
 */
static void rfm95_read_payload_data(uint8_t* data, uint8_t length) {
    uint8_t addr = REG_FIFO & 0x7F;       // Endereço FIFO para leitura
    
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &addr, 1);        // Envia endereço do FIFO
    spi_read_blocking(SPI_PORT, 0, data, length); // Lê os dados
    gpio_put(PIN_CS, 1);
}

/**
 * @brief Escreve dados no FIFO do RFM95
 * 
 * @param buffer Buffer contendo os dados a serem escritos
 * @param length Quantidade de bytes a serem escritos
 * 
 * Acessa o FIFO através do registrador REG_FIFO para enviar dados de pacotes.
 */
static void rfm95_write_payload_data(const uint8_t* data, uint8_t length) {
    uint8_t addr = REG_FIFO | 0x80;       // Endereço FIFO para escrita
    
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &addr, 1);         // Envia endereço do FIFO
    spi_write_blocking(SPI_PORT, data, length);   // Escreve os dados
    gpio_put(PIN_CS, 1);
}

// ============================================================================
// CONFIGURAÇÃO E CONTROLE DE MODO
// ============================================================================

/**
 * @brief Inicializa o módulo RFM95 e configura parâmetros básicos
 * 
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 * 
 * Configura:
 * - Interface SPI (1 MHz, modo 0)
 * - Pinos GPIO (CS, RST)
 * - Reset do módulo
 * - Verificação da versão do chip
 * - Frequência de operação (915 MHz - Brasil)
 * - Configurações LoRa (BW=125kHz, CR=4/5, SF=7, CRC habilitado)
 * - Preâmbulo de 8 símbolos
 */
bool rfm95_initialize() {
    // Configuração da interface SPI
    spi_init(SPI_PORT, 1 * 1000 * 1000);          
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Configuração dos pinos de controle
    gpio_init(PIN_CS);   
    gpio_set_dir(PIN_CS, GPIO_OUT);   
    gpio_put(PIN_CS, 1);                           // CS inativo (HIGH)

    gpio_init(PIN_RST);  
    gpio_set_dir(PIN_RST, GPIO_OUT);

    // Reset do módulo e verificação de comunicação
    rfm95_reset();
    if (rfm95_read_register(REG_VERSION) != 0x12) {     // Versão esperada do RFM95
        return false;                              // Falha na comunicação
    }

    // Configuração inicial
    rfm95_set_sleep_mode();                        // Inicia em modo sleep
    rfm95_set_frequency(FREQUENCY_HZ);        // Define frequência (915 MHz)

    // Configuração do FIFO
    rfm95_write_register(REG_FIFO_TX_BASE_AD, 0x00);    // Base do FIFO TX
    rfm95_write_register(REG_FIFO_RX_BASE_AD, 0x00);    // Base do FIFO RX

    // Configuração do LNA (Low Noise Amplifier)
    rfm95_write_register(REG_LNA, rfm95_read_register(REG_LNA) | 0x03);

    // Configuração LoRa: BW=125kHz, CR=4/5, modo explícito
    rfm95_write_register(REG_MODEM_CONFIG_1, BANDWIDTH_125K + ERROR_CODING_4_5 + EXPLICIT_MODE);

    // Configuração LoRa: SF=7, CRC habilitado
    rfm95_write_register(REG_MODEM_CONFIG_2, SPREADING_7 + CRC_ON);

    // Configuração do preâmbulo (8 símbolos)
    rfm95_write_register(REG_PREAMBLE_MSB, 0x00);
    rfm95_write_register(REG_PREAMBLE_LSB, 0x08);

    rfm95_set_idle_mode();                         // Coloca em modo standby
    return true;
}

/**
 * @brief Define a frequência de operação do RFM95
 * 
 * @param frequency Frequência em Hz (ex: 915000000 para 915 MHz)
 * 
 * Calcula o valor FRF (Frequency Register Value) baseado na fórmula:
 * FRF = (frequency * 2^19) / crystal_frequency
 * onde crystal_frequency = 32 MHz
 */
void rfm95_set_frequency(long frequency) {
    uint64_t frf = ((uint64_t)frequency << 19) / RF_CRYSTAL_FREQ_HZ;
    
    // Escreve os 24 bits da frequência nos 3 registradores
    rfm95_write_register(REG_FRF_MSB, (uint8_t)(frf >> 16));  // Bits 23-16
    rfm95_write_register(REG_FRF_MID, (uint8_t)(frf >> 8));   // Bits 15-8
    rfm95_write_register(REG_FRF_LSB, (uint8_t) frf);         // Bits 7-0
}

/**
 * @brief Configura a potência de transmissão
 * 
 * @param power Potência em dBm (2-17 dBm)
 * 
 * Usa o amplificador PA_BOOST para potências de 2-17 dBm.
 * A fórmula é: Output_Power = power - 2
 */
void rfm95_set_tx_power(uint8_t power) {
    // Limita a potência entre 2 e 17 dBm
    if (power > 17) power = 17;
    if (power < 2)  power = 2;
    
    // PA_BOOST habilitado (bit 7) + Output_Power (bits 3-0)
    rfm95_write_register(REG_PA_CONFIG, 0x80 | (power - 2));
}

/**
 * @brief Coloca o módulo em modo Sleep
 * 
 * No modo Sleep, o consumo é mínimo (~0.2µA), mas todas as funções ficam
 * desabilitadas exceto a interface SPI.
 */
void rfm95_set_sleep_mode() {
    rfm95_write_register(REG_OPMODE, MODE_LORA | MODE_SLEEP);
}

/**
 * @brief Coloca o módulo em modo Standby (Idle)
 * 
 * No modo Standby, o módulo está pronto para transmitir ou receber,
 * com consumo reduzido (~1.5mA) mas maior que Sleep.
 */
void rfm95_set_idle_mode() {
    rfm95_write_register(REG_OPMODE, MODE_LORA | MODE_STDBY);
}

// ============================================================================
// TRANSMISSÃO E RECEPÇÃO
// ============================================================================

/**
 * @brief Transmite um pacote de dados
 * 
 * @param buffer Ponteiro para os dados a serem transmitidos
 * @param size Tamanho dos dados em bytes (máximo 255)
 * 
 * Processo:
 * 1. Coloca o módulo em modo Standby
 * 2. Reseta o ponteiro do FIFO
 * 3. Escreve os dados no FIFO
 * 4. Define o tamanho do payload
 * 5. Inicia transmissão
 * 6. Aguarda conclusão da transmissão
 * 7. Limpa a flag de transmissão concluída
 * 8. Retorna ao modo Standby
 */
void rfm95_transmit(const uint8_t* data, uint8_t size) {
    rfm95_set_idle_mode();                         // Modo Standby
    
    // Prepara o FIFO para transmissão
    rfm95_write_register(REG_FIFO_ADDR_PTR, 0);         // Reset ponteiro FIFO
    rfm95_write_payload_data(data, size);                // Escreve dados no FIFO
    rfm95_write_register(REG_PAYLOAD_LENGTH, size);     // Define tamanho do payload

    // Inicia transmissão
    rfm95_write_register(REG_OPMODE, MODE_LORA | MODE_TX);

    // Aguarda conclusão da transmissão
    while (!(rfm95_read_register(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK)) {
        sleep_ms(1);                               // Polling com delay
    }
    
    // Limpa flag de transmissão concluída
    rfm95_write_register(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
    rfm95_set_idle_mode();                         // Retorna ao Standby
}

/**
 * @brief Verifica se há dados recebidos e os lê
 * 
 * @param buffer Buffer para armazenar os dados recebidos
 * @param max_size Tamanho máximo do buffer
 * @return Número de bytes recebidos (0 se nenhum dado disponível)
 * 
 * Processo:
 * 1. Coloca o módulo em modo de recepção contínua
 * 2. Verifica se há dados recebidos (flag RX_DONE)
 * 3. Verifica se há erro de CRC
 * 4. Lê o tamanho dos dados recebidos
 * 5. Lê os dados do FIFO
 * 6. Retorna o número de bytes lidos
 */
int rfm95_receive(uint8_t* data, int max_size) {
    // Coloca em modo de recepção contínua
    rfm95_write_register(REG_OPMODE, MODE_LORA | MODE_RX_CONTINUOUS);

    // Verifica flags de interrupção
    uint8_t irq = rfm95_read_register(REG_IRQ_FLAGS);

    if (irq & IRQ_RX_DONE_MASK) {                  // Dados recebidos?
        // Limpa flag de recepção concluída
        rfm95_write_register(REG_IRQ_FLAGS, IRQ_RX_DONE_MASK);

        // Verifica erro de CRC
        if (irq & IRQ_PAYLOAD_CRC_ERROR_MASK) {
            return 0;                              // Dados corrompidos
        }

        // Lê tamanho dos dados recebidos
        uint8_t len = rfm95_read_register(REG_RX_NB_BYTES);
        if (len > max_size) len = max_size;        // Limita ao tamanho do buffer

        // Configura ponteiro do FIFO para posição dos dados recebidos
        uint8_t fifo_addr = rfm95_read_register(REG_FIFO_RX_CURRENT_ADDR);
        rfm95_write_register(REG_FIFO_ADDR_PTR, fifo_addr);
        
        // Lê os dados do FIFO
        rfm95_read_payload_data(data, len);
        return len;                                // Retorna quantidade de bytes
    }
    
    return 0;                                      // Nenhum dado disponível
}

// ============================================================================
// INFORMAÇÕES DE QUALIDADE DO SINAL
// ============================================================================

/**
 * @brief Obtém o RSSI (Received Signal Strength Indicator) do último pacote
 * 
 * @return RSSI em dBm (valores negativos, ex: -80 dBm)
 * 
 * O valor bruto do registrador é ajustado subtraindo 157 para obter o valor
 * real em dBm para a banda de 915 MHz.
 */
int rfm95_get_rssi() {
    return (rfm95_read_register(REG_PKT_RSSI_VALUE) - 157);
}

/**
 * @brief Obtém o SNR (Signal-to-Noise Ratio) do último pacote
 * 
 * @return SNR em dB (pode ser positivo ou negativo)
 * 
 * O valor é lido como complemento de 2 (int8_t) e multiplicado por 0.25
 * para obter o valor real em dB.
 */
float rfm95_get_snr() {
    return ((int8_t)rfm95_read_register(REG_PKT_SNR_VALUE)) * 0.25f;
}