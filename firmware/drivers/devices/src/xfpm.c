#include "xfpm.h"

#include "gpio_mcu.h"
#include <stdio.h>
#include <stdint.h>

/*==================[macros and definitions]=================================*/

#define V_REF 3.3                /**< Tensión de referencia*/
#define TOTAL_BITS 1024          /**< Cantidad total de bits*/
#define alpha 0.018
#define betha 0.04
#define P_error 1.25

/*==================[internal data declaration]==============================*/

analog_input_config_t presion_config = {
		.input = CH2,
		.mode = ADC_SINGLE,
	};

typedef struct {

    float presion_min;
    float presion_max;

} PressureValues;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

bool XFPM050KInit()
{
	AnalogInputInit(&presion_config);  
	return true;
}

PressureValues XFPM050MeasurePressure(void)
{
	uint16_t value;
	float presion = 0;
	float valor = 0;
	PressureValues pressure;

	AnalogInputReadSingle(presion_config.input, &value); //value lo da en mV

	valor = (value/1000.0);

	pressure.presion_max = 1/alpha*((valor/V_REF)-betha + P_error*alpha);
	
    pressure.presion_min = 1/alpha*((valor/V_REF)-betha - P_error*alpha);

    return pressure;
}


bool XFPM050Deinit()
{
	return true;
}


/*==================[end of file]============================================*/