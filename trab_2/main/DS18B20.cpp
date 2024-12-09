#ifndef __DS18B20__
#define __DS18B20__

#include "DS18B20.h"

// Define um array para armazenar os endereços (até 10 sensores)
#define MAX_SENSORS 10
uint64_t enderecos[MAX_SENSORS];
int num_sensores = 0;

void DS18B20::programa(void) {
	onewire->reset();
	onewire->writeByte(0x4E);
	onewire->writeByte(0x00);
	onewire->writeByte(0x00);
	onewire->writeByte(0x7F);
}

void DS18B20::capturaBit(int posicao,char v[], int valor) {
	unsigned char pbyte = posicao / 8;
	unsigned char pbit  = posicao % 8;
	
	if (valor==1) v[pbyte] |= (1 << pbit);
	else v[pbyte] &= ~(1<< pbit);
}

void DS18B20::init(void) {
	// So pode ser usada se existir apenas um escravo no barramento
	uint8_t serial[6],crc;
	printf("*** Iniciando busca no barramento ***\n");

	delay_ms(500);

	if (onewire->reset()==0) printf("Detectou escravo na linha\n");
	else printf("Nao detectou escravo\n");

	onewire->writeByte(READ_ROM);
	printf("Codigo da Familia: %d\n",onewire->readByte());
	for (uint8_t x=0;x<6;x++) serial[x] = onewire->readByte();

    printf("Numero de Serie: %d %d %d %d %d %d\n",serial[0],serial[1],serial[2],serial[3],serial[4],serial[5]);

 	crc = onewire->readByte();
 	printf("CRC: %d\n",crc);

 	delay_ms(1000);
}

void DS18B20::init2(void) {
	// Pode ser chamada se existir 1 ou mais escravos
	printf("*** Iniciando busca no barramento ***\n");
	delay_ms(500);

	if (onewire->reset()==0) printf("Detectou escravo na linha\n");
	else printf("Nao detectou escravo\n");

 	delay_ms(1000);
}

// Usa o pino GPIO16 para fazer a comunicacao
DS18B20::DS18B20(gpio_num_t pino) {
	DEBUG("DS18B20:Construtor\n");
	onewire = new ONEWIRE(pino);
}

char DS18B20::CRC(char end[]) {
	// calcula o CRC e retorna o resultado
	return 0;
}

void DS18B20::fazScan() {    
    uint8_t last_discrepancy = 0;
    bool last_device_flag = false;
    int num_sensores = 0;


    uint8_t rom_no[8];

    // Limpa o ROM
    memset(rom_no, 0, sizeof(rom_no));

    printf("*** Iniciando SEARCH_ROM ***\n");

    while (!last_device_flag && (num_sensores < MAX_SENSORS)) {
        int id_bit_number = 1;
        int last_zero = 0;
        uint8_t rom_byte_number = 0;
        uint8_t rom_byte_mask = 1;
        bool search_result = false;

        uint8_t id_bit, cmp_id_bit;
        uint8_t direction;

        // Inicia a comunicação
        if (onewire->reset()) {
            break;
        }

        // Envia o comando SEARCH_ROM
        onewire->writeByte(SEARCH_ROM);

        // Loop para ler os 64 bits do ROM
        do {
            // Lê um bit e seu complemento
            id_bit = onewire->readBit();
            cmp_id_bit = onewire->readBit();

            if ((id_bit == 1) && (cmp_id_bit == 1)) {
                // Não há dispositivos no barramento
                break;
            } 
            else {
                if (id_bit != cmp_id_bit) {
                    // Sem conflito, bit é igual ao id_bit
                    direction = id_bit;
                } 
                else { // (id_bit == 0) && (cmp_id_bit == 0)
                    // Conflito, precisa decidir o caminho
                    if (id_bit_number < last_discrepancy) {
                        // Se a posição atual é menor que a última discrepância, segue o mesmo caminho
                        direction = ((rom_no[rom_byte_number] & rom_byte_mask) > 0);
                    } 
                    else if (id_bit_number == last_discrepancy) {
                        // Na posição da última discrepância, escolhe 1
                        direction = 1;
                    } 
                    else {
                        // Nas outras posições, escolhe 0
                        direction = 0;
                    }

                    // Se a direção escolhida é 0, marca o último zero encontrado
                    if (direction == 0) {
                        last_zero = id_bit_number;
                    }
                }

                // Define o bit no ROM
                if (direction == 1) {
                    rom_no[rom_byte_number] |= rom_byte_mask;
                } 
                else {
                    rom_no[rom_byte_number] &= ~rom_byte_mask;
                }

                // Envia o bit escolhido para o barramento
                onewire->writeBit(direction);

                // Atualiza para o próximo bit
                id_bit_number++;
                rom_byte_mask <<= 1;

                if (rom_byte_mask == 0) {
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        } while (rom_byte_number < 8); // Repete até ler todos os bytes

        // Verifica se a busca foi bem-sucedida
        if (id_bit_number >= 65) {
            // Busca bem-sucedida
            last_discrepancy = last_zero;

            if (last_discrepancy == 0) {
                last_device_flag = true;
            }

            search_result = true;
            num_sensores++;

            // Armazena o endereço encontrado no vetor
            std::array<uint8_t, 8> sensor_address;
            memcpy(sensor_address.data(), rom_no, 8);

            // Exibe o endereço encontrado
            printf("Sensor %d: Endereco = ", num_sensores);
            for (const auto& byte : sensor_address) { // Itera pelos bytes do array
                printf("%02X", byte);
            }
            printf("\n");

            // leitura da temperatura
            onewire->reset();
            onewire->writeByte(MATCH_ROM);
            for (const auto& byte : sensor_address) {
                onewire->writeByte(byte);
            }
            onewire->writeByte(INICIA_CONVERSAO_TEMP); 

            delay_ms(750);

            onewire->reset();
            onewire->writeByte(MATCH_ROM);
            for (const auto& byte : sensor_address) {
                onewire->writeByte(byte);
            }
            onewire->writeByte(READ_TEMP_MEMORY);

            uint8_t data[9];
            for (int i = 0; i < 9; i++) {
                data[i] = onewire->readByte();
            }

            int16_t raw_temp = (data[1] << 8) | data[0];
            float temperatura = raw_temp / 16.0;

            printf("Temperatura Sensor %d: %.2f Graus Celsius\n\n", num_sensores, temperatura);

        }

        if (!search_result) {
            printf("Erro na busca de dispositivos.\n");
            break;
        }

    }
}




#endif
