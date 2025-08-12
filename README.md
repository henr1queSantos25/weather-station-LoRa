# LoRa Weather Station: Estação Meteorológica com Comunicação LoRa

O **LoRa Weather Station** é uma estação meteorológica embarcada desenvolvida para o microcontrolador **Raspberry Pi Pico (RP2040)** com comunicação de longo alcance via tecnologia **LoRa**. O sistema realiza medição contínua e precisa de **temperatura**, **umidade relativa do ar** e **pressão atmosférica**, transmitindo os dados em formato JSON através do módulo **RFM95** para sistemas receptores remotos. O projeto oferece uma solução robusta para monitoramento ambiental distribuído, ideal para aplicações IoT que exigem comunicação de baixo consumo e longo alcance.

---

## Funcionalidades Principais

- **Monitoramento Ambiental Contínuo**: Medição precisa de temperatura, umidade e pressão atmosférica utilizando sensores digitais de alta qualidade.
- **Comunicação LoRa de Longo Alcance**:
  - Transmissão via módulo RFM95 na frequência de 915 MHz (banda ISM brasileira)
  - Alcance de até 10km em campo aberto com baixo consumo energético
  - Configuração LoRa otimizada: BW=125kHz, CR=4/5, SF=7, CRC habilitado
- **Transmissão de Dados Estruturados**:
  - Formato JSON para fácil integração com sistemas receptores
  - Dados transmitidos: temperatura, pressão e umidade em tempo real
  - Intervalo de transmissão configurável (padrão: 2 segundos)
- **Sensores Duplos para Maior Precisão**:
  - Leitura redundante de temperatura através de dois sensores independentes
  - Cálculo de média entre AHT20 e BMP280 para maior confiabilidade
  - Limitação automática de umidade a 100% para evitar valores irreais
- **Comunicação SPI Otimizada**: Interface dedicada para controle do módulo RFM95
- **Sistema Robusto**: Verificação de integridade na inicialização e controle de erro CRC
- **Arquitetura Modular**: Bibliotecas separadas para cada componente facilitando manutenção

---

## Tecnologias Utilizadas

- **Linguagem de Programação**: C
- **Microcontrolador**: Raspberry Pi Pico (RP2040)
- **Comunicação**: LoRa (Long Range) - Tecnologia LPWAN
- **Protocolo de RF**: LoRaWAN Physical Layer com modulação CSS (Chirp Spread Spectrum)
- **Sensores Utilizados**:
  - **AHT20**: Sensor digital I2C para temperatura (-40°C a +85°C) e umidade relativa (0-100% RH)
  - **BMP280**: Sensor digital I2C para pressão atmosférica (300-1100 hPa) e temperatura auxiliar
- **Módulo de RF**:
  - **RFM95W**: Transceptor LoRa de 915 MHz com potência ajustável (2-17 dBm)
  - Interface SPI para comunicação de alta velocidade com o microcontrolador
- **Bibliotecas**:
  - Pico SDK para acesso ao hardware (GPIO, I2C, SPI)
  - Bibliotecas customizadas para controle de sensores e módulo LoRa
  - Implementação completa do protocolo LoRa físico

---

## Como Funciona

### Sistema de Aquisição de Dados
- **Leitura Paralela de Sensores**:
  - **AHT20**: Fornece temperatura e umidade relativa através de interface I2C
  - **BMP280**: Oferece pressão atmosférica e segunda leitura de temperatura para redundância
- **Processamento Inteligente**:
  - Média aritmética entre as duas leituras de temperatura para maior precisão
  - Conversão de unidades: pressão em kPa, temperatura em °C, umidade em %
  - Validação de dados: umidade limitada a 100% máximo
- **Formatação JSON**: Estruturação dos dados no formato `{"temperatura":25.4,"pressao":101,"umidade":65.2}`

### Comunicação LoRa
- **Configuração do Módulo RFM95**:
  - Frequência: 915 MHz (banda ISM brasileira - regulamentada pela Anatel)
  - Largura de banda: 125 kHz para alcance otimizado
  - Fator de espalhamento: SF7 (128 chips/símbolo) equilibrando alcance e taxa de dados
  - Taxa de codificação: CR 4/5 para correção de erros
  - Potência: 17 dBm (50mW) para máximo alcance
- **Processo de Transmissão**:
  1. Aquisição dos dados dos sensores
  2. Formatação em string JSON
  3. Preparação do FIFO interno do RFM95
  4. Transmissão com verificação de conclusão
  5. Limpeza de flags e retorno ao modo standby
- **Características do Link**:
  - Taxa de dados: ~5.5 kbps
  - Sensibilidade do receptor: até -148 dBm
  - Alcance teórico: 10-15 km em campo aberto
  - Consumo em transmissão: ~120 mA @ 3.3V

### Arquitetura de Hardware
- **Interface I2C0**: Comunicação com sensores ambientais (pinos GP0/GP1)
- **Interface SPI**: Comunicação dedicada com módulo RFM95 (pinos MISO/MOSI/SCK)
- **Controle GPIO**: Pinos dedicados para CS (Chip Select) e RST (Reset) do RFM95
- **Verificação de Integridade**: Leitura do registrador de versão para validação da comunicação

---

## Configuração do Hardware

| Componente | Interface | Pinos do RP2040 | Função |
|------------|-----------|----------------|--------|
| Sensores AHT20 e BMP280 | I2C0 | GP0 (SDA), GP1 (SCL) | Medição de temperatura, umidade e pressão |
| Módulo RFM95W | SPI | GP16 (MISO), GP19 (MOSI), GP18 (SCK) | Comunicação LoRa de longo alcance |
| Controle RFM95 | GPIO | GP17 (CS), GP20 (RST) | Seleção de chip e reset do módulo LoRa |

### Especificações dos Sensores
- **AHT20**:
  - Temperatura: -40°C a +85°C (precisão ±0.3°C)
  - Umidade: 0% a 100% RH (precisão ±2% RH)
  - Interface: I2C (endereço 0x38)
  - Resolução: 0.01°C e 0.024% RH
  
- **BMP280**:
  - Pressão: 300-1100 hPa (precisão ±1 hPa)
  - Temperatura: -40°C a +85°C (precisão ±1°C)
  - Interface: I2C (endereço 0x76)
  - Resolução: 0.16 Pa e 0.01°C

### Especificações do Módulo RFM95W
- **Frequência de Operação**: 915 MHz ±20 MHz
- **Modulação**: LoRa Chirp Spread Spectrum
- **Potência de Saída**: +2 a +17 dBm configurável
- **Sensibilidade**: -148 dBm (SF12, BW=125kHz)
- **Interface**: SPI até 10 MHz
- **Consumo**: 120 mA (TX), 12 mA (RX), 0.2 µA (Sleep)

---

## Configuração do Sistema

### Parâmetros LoRa
No arquivo `rfm95_definitions.h`, os parâmetros estão pré-configurados:

```c
// Frequência de operação (915 MHz - Brasil)
#define FREQUENCY_HZ        915000000

// Configurações LoRa otimizadas
#define BANDWIDTH_125K      0x70    // 125 kHz
#define ERROR_CODING_4_5    0x02    // Taxa de codificação 4/5
#define SPREADING_7         0x70    // Fator de espalhamento 7
#define CRC_ON              0x04    // CRC habilitado
```

### Configuração de Potência
No arquivo `main.c`, ajuste a potência de transmissão:

```c
rfm95_set_tx_power(17); // Potência de 2-17 dBm
```

### Pinos de Conexão
Configure os pinos no arquivo de definições conforme sua placa:

```c
// Pinos I2C para sensores
#define I2C_SDA_SENSORS 0
#define I2C_SCL_SENSORS 1

// Pinos SPI e controle para RFM95
#define PIN_MISO 16
#define PIN_MOSI 19  
#define PIN_SCK  18
#define PIN_CS   17
#define PIN_RST  20
```

---

## Estrutura do Repositório

- **`main.c`**: Código-fonte principal com loop de aquisição e transmissão
- **`lib/rfm95/`**: Biblioteca completa para módulo LoRa RFM95
  - **`rfm95.h` e `rfm95.c`**: Funções de controle, configuração e comunicação LoRa
  - **`rfm95_definitions.h`**: Definições de registradores e constantes do RFM95
- **`lib/aht20/`**: Biblioteca para sensor AHT20
  - **`aht20.h` e `aht20.c`**: Controle e leitura do sensor de temperatura e umidade
- **`lib/bmp280/`**: Biblioteca para sensor BMP280
  - **`bmp280.h` e `bmp280.c`**: Controle e leitura do sensor de pressão atmosférica
- **`CMakeLists.txt`**: Configuração do sistema de build
- **`README.md`**: Documentação completa do projeto

---

## Fluxo de Operação

1. **Inicialização do Sistema**:
   - Configuração da interface SPI para comunicação com RFM95
   - Configuração da interface I2C para comunicação com sensores
   - Reset e verificação de comunicação com o módulo RFM95
   - Inicialização dos sensores AHT20 e BMP280

2. **Configuração do Módulo LoRa**:
   - Definição da frequência de operação (915 MHz)
   - Configuração dos parâmetros LoRa (BW, SF, CR, CRC)
   - Ajuste da potência de transmissão
   - Configuração do preâmbulo e modo de operação

3. **Loop Principal de Aquisição e Transmissão**:
   - **Leitura do BMP280**: Obtenção de dados brutos de temperatura e pressão
   - **Processamento BMP280**: Conversão usando parâmetros de calibração internos
   - **Leitura do AHT20**: Obtenção de dados de temperatura e umidade
   - **Processamento de Dados**:
     - Cálculo da média de temperatura entre os dois sensores
     - Conversão de pressão para kPa
     - Limitação da umidade ao máximo de 100%
   - **Formatação JSON**: Criação da string de dados estruturados
   - **Transmissão LoRa**: Envio dos dados via RFM95
   - **Aguardo**: Intervalo de 2 segundos antes da próxima medição

4. **Tratamento de Erros**:
   - Verificação de comunicação SPI com RFM95
   - Validação de leituras dos sensores I2C
   - Tratamento de falhas de transmissão
   - Reinicialização automática em caso de problemas

---

## Dados Transmitidos

### Formato JSON
```json
{
    "temperatura": 25.67,
    "pressao": 101,
    "umidade": 65.23
}
```

### Especificações dos Dados
- **Temperatura**: Valor em graus Celsius com precisão de 0.01°C
- **Pressão**: Valor em kPa (quilopascals) arredondado para número inteiro
- **Umidade**: Valor em porcentagem com precisão de 0.01%
- **Tamanho do Pacote**: Aproximadamente 50-60 bytes por transmissão
- **Taxa de Transmissão**: Uma medição a cada 2 segundos (configurável)

---

## Conceitos Aplicados

- **Tecnologia LoRa**: Modulação por espalhamento espectral de longo alcance
- **Comunicação LPWAN**: Rede de área ampla de baixa potência para IoT
- **Interface SPI**: Comunicação serial síncrona de alta velocidade
- **Interface I2C**: Barramento de comunicação digital multi-dispositivo
- **Sensoriamento Redundante**: Uso de múltiplos sensores para maior confiabilidade
- **Processamento de Sinais**: Média de leituras e filtragem de dados
- **Sistemas Embarcados**: Programação de microcontroladores de baixo nível
- **Protocolos de RF**: Implementação de stack de comunicação sem fio
- **Arquitetura Modular**: Separação de responsabilidades em bibliotecas
- **Tratamento de Erros**: Verificação de integridade e recuperação de falhas
- **Calibração de Sensores**: Uso de parâmetros de compensação interno dos sensores
- **Formatação de Dados**: Estruturação em JSON para interoperabilidade

---

## Objetivos Alcançados

- **Comunicação de Longo Alcance**: Implementação completa do protocolo LoRa para transmissão até 10km
- **Monitoramento Ambiental Preciso**: Sistema de medição com sensores redundantes para maior confiabilidade
- **Transmissão Estruturada**: Dados em formato JSON padronizado para fácil integração
- **Sistema Robusto**: Verificação de integridade e tratamento de erros em todas as camadas
- **Baixo Consumo**: Arquitetura otimizada para operação com bateria em aplicações remotas
- **Modularidade**: Código organizado em bibliotecas reutilizáveis e bem documentadas
- **Configurabilidade**: Parâmetros LoRa e de sistema facilmente ajustáveis
- **Aplicabilidade IoT**: Solução pronta para integração em redes de sensores distribuídos
- **Conformidade Regulatória**: Operação na banda ISM de 915 MHz regulamentada no Brasil
- **Documentação Completa**: Código bem comentado com explicações técnicas detalhadas
- **Escalabilidade**: Arquitetura preparada para expansão com novos sensores e funcionalidades
- **Interoperabilidade**: Formato de dados padrão compatível com diversos sistemas receptores

---

## Aplicações Práticas

- **Agricultura de Precisão**: Monitoramento de condições climáticas em cultivos
- **Estações Meteorológicas Remotas**: Coleta de dados em locais de difícil acesso
- **Monitoramento Industrial**: Controle ambiental em instalações industriais
- **Pesquisa Científica**: Coleta de dados ambientais para estudos climatológicos
- **Smart Cities**: Rede de sensores urbanos para monitoramento da qualidade do ar
- **Conservação Ambiental**: Monitoramento de ecossistemas e áreas protegidas

---


## Desenvolvido por

- Henrique Oliveira dos Santos
- Rodrigo Pirôpo
- Levi Freitas
