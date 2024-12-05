#include "digital.h"
#include "DS18B20.h"
#include "delay.h"

extern "C" void app_main() ;

void app_main() {
	delay_ms(2000);

	std::vector<std::array<uint8_t, 8>> sensores;
	
	DS18B20 meuSensor = DS18B20(GPIO_NUM_4);
	meuSensor.init2();
	sensores = meuSensor.fazScan();
}

