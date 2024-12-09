#include <inttypes.h>
#include <stdio.h>
#include <cstring>
#include <array>

#include "onewire.h"
#include "digital.h"
#include "debug.h"
#include "delay.h"
#include "onewire.h"

class DS18B20 {
private:
	ONEWIRE *onewire;
	char CRC(char end[]);
	void capturaBit(int posicao, char v[], int valor);

public:
	DS18B20(gpio_num_t pino);
	void programa(void);
	void init(void);
	void init2(void);
	void fazScan(void);
};
