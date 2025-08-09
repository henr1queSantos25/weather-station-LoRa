#ifndef REGISTRARS_H
#define REGISTRARS_H

// ============================================================================
// REGISTRADORES BÁSICOS DO RFM95
// ============================================================================

// Registradores de controle principal
#define REG_FIFO                    0x00    // Buffer FIFO para TX/RX
#define REG_OPMODE                  0x01    // Modo de operação
#define REG_VERSION                 0x42    // Versão do chip (deve ser 0x12)

// Registradores de frequência
#define REG_FRF_MSB                 0x06    // Frequência RF - byte mais significativo
#define REG_FRF_MID                 0x07    // Frequência RF - byte do meio
#define REG_FRF_LSB                 0x08    // Frequência RF - byte menos significativo

// Registradores de FIFO
#define REG_FIFO_ADDR_PTR           0x0D    // Ponteiro de endereço do FIFO
#define REG_FIFO_TX_BASE_AD         0x0E    // Endereço base do FIFO TX
#define REG_FIFO_RX_BASE_AD         0x0F    // Endereço base do FIFO RX
#define REG_FIFO_RX_CURRENT_ADDR    0x10    // Endereço atual do FIFO RX

// Registradores de interrupção
#define REG_IRQ_FLAGS_MASK          0x11    // Máscara de flags de interrupção
#define REG_IRQ_FLAGS               0x12    // Flags de interrupção
#define REG_IRQ_FLAGS2              0x3F    // Flags de interrupção 2 (FSK)

// Registradores de dados recebidos
#define REG_RX_NB_BYTES             0x13    // Número de bytes recebidos
#define REG_PKT_SNR_VALUE           0x19    // SNR do último pacote
#define REG_PKT_RSSI_VALUE          0x1A    // RSSI do último pacote

// ============================================================================
// CONFIGURAÇÕES DE MODEM E PACOTE
// ============================================================================

// Configuração do modem LoRa
#define REG_MODEM_CONFIG_1          0x1D    // BW, Coding Rate, Header Mode
#define REG_MODEM_CONFIG_2          0x1E    // Spreading Factor, CRC
#define REG_MODEM_CONFIG_3          0x26    // Low Data Rate Optimize

// Configuração de preâmbulo
#define REG_PREAMBLE_MSB            0x20    // Preâmbulo - byte alto
#define REG_PREAMBLE_LSB            0x21    // Preâmbulo - byte baixo

// Configuração de payload
#define REG_PAYLOAD_LENGTH          0x22    // Tamanho do payload
#define PAYLOAD_LENGTH              255     // Tamanho máximo do payload

// Outros registradores
#define REG_HOP_PERIOD              0x24    // Período de frequency hopping
#define REG_FREQ_ERROR              0x28    // Erro de frequência
#define REG_DETECT_OPT              0x31    // Otimização de detecção
#define REG_DETECTION_THRESHOLD     0x37    // Threshold de detecção

// Mapeamento de pinos DIO
#define REG_DIO_MAPPING_1           0x40    // Mapeamento DIO0-DIO3
#define REG_DIO_MAPPING_2           0x41    // Mapeamento DIO4-DIO5

// ============================================================================
// REGISTRADORES FSK (MODO FSK/OOK)
// ============================================================================

#define REG_BITRATE_MSB             0x02    // Taxa de bits - byte alto
#define REG_BITRATE_LSB             0x03    // Taxa de bits - byte baixo
#define REG_FDEV_MSB                0x04    // Desvio de frequência - byte alto
#define REG_FDEV_LSB                0x05    // Desvio de frequência - byte baixo
#define REG_PREAMBLE_MSB_FSK        0x25    // Preâmbulo FSK - byte alto
#define REG_PREAMBLE_LSB_FSK        0x26    // Preâmbulo FSK - byte baixo
#define REG_PACKET_CONFIG1          0x30    // Configuração de pacote FSK
#define REG_PAYLOAD_LENGTH_FSK      0x32    // Tamanho do payload FSK
#define REG_FIFO_THRESH             0x35    // Threshold do FIFO

// ============================================================================
// MODOS DE OPERAÇÃO
// ============================================================================

#define MODE_LORA                   0x80    // Modo LoRa (vs FSK)
#define MODE_SLEEP                  0x00    // Modo Sleep
#define MODE_STDBY                  0x01    // Modo Standby
#define MODE_TX                     0x03    // Modo Transmissão
#define MODE_RX_CONTINUOUS          0x05    // Modo Recepção Contínua
#define MODE_RX_SINGLE              0x06    // Modo Recepção Única

// ============================================================================
// CONFIGURAÇÕES DE MODEM LORA
// ============================================================================

// Modo de header
#define EXPLICIT_MODE               0x00    // Header explícito
#define IMPLICIT_MODE               0x01    // Header implícito

// Error Coding Rate (Taxa de correção de erro)
#define ERROR_CODING_4_5            0x02    // 4/5
#define ERROR_CODING_4_6            0x04    // 4/6
#define ERROR_CODING_4_7            0x06    // 4/7
#define ERROR_CODING_4_8            0x08    // 4/8

// Bandwidth (Largura de banda)
#define BANDWIDTH_7K8               0x00    // 7.8 kHz
#define BANDWIDTH_10K4              0x10    // 10.4 kHz
#define BANDWIDTH_15K6              0x20    // 15.6 kHz
#define BANDWIDTH_20K8              0x30    // 20.8 kHz
#define BANDWIDTH_31K25             0x40    // 31.25 kHz
#define BANDWIDTH_41K7              0x50    // 41.7 kHz
#define BANDWIDTH_62K5              0x60    // 62.5 kHz
#define BANDWIDTH_125K              0x70    // 125 kHz (padrão)
#define BANDWIDTH_250K              0x80    // 250 kHz
#define BANDWIDTH_500K              0x90    // 500 kHz

// Spreading Factor (Fator de espalhamento)
#define SPREADING_6                 0x60    // SF6 (64 chips/symbol)
#define SPREADING_7                 0x70    // SF7 (128 chips/symbol) - padrão
#define SPREADING_8                 0x80    // SF8 (256 chips/symbol)
#define SPREADING_9                 0x90    // SF9 (512 chips/symbol)
#define SPREADING_10                0xA0    // SF10 (1024 chips/symbol)
#define SPREADING_11                0xB0    // SF11 (2048 chips/symbol)
#define SPREADING_12                0xC0    // SF12 (4096 chips/symbol)

// CRC (Verificação de redundância cíclica)
#define CRC_OFF                     0x00    // CRC desabilitado
#define CRC_ON                      0x04    // CRC habilitado

// ============================================================================
// CONFIGURAÇÕES DE POTÊNCIA (POWER AMPLIFIER)
// ============================================================================

#define REG_PA_CONFIG               0x09    // Configuração do PA
#define REG_PA_DAC                  0x4D    // DAC do PA para alta potência

// Configurações predefinidas de potência
#define PA_MAX_BOOST                0x8F    // 100mW (max 869.4 - 869.65)
#define PA_LOW_BOOST                0x81    // Baixa potência
#define PA_MED_BOOST                0x8A    // Média potência
#define PA_MAX_UK                   0x88    // 10mW (max 434 MHz)
#define PA_OFF_BOOST                0x00    // PA desligado
#define RFO_MIN                     0x00    // RFO mínimo

// Alta potência (20dBm)
#define PA_DAC_20                   0x87    // Para modo 20dBm

// ============================================================================
// AMPLIFICADOR DE BAIXO RUÍDO (LNA)
// ============================================================================

#define REG_LNA                     0x0C    // Configuração do LNA
#define LNA_MAX_GAIN                0x23    // Ganho máximo (0010 0011)
#define LNA_OFF_GAIN                0x00    // LNA desligado

// ============================================================================
// MÁSCARAS DE INTERRUPÇÃO
// ============================================================================

#define IRQ_RX_DONE_MASK            0x40    // Recepção concluída
#define IRQ_TX_DONE_MASK            0x08    // Transmissão concluída
#define IRQ_PAYLOAD_CRC_ERROR_MASK  0x20    // Erro de CRC no payload

#endif // REGISTRARS_H