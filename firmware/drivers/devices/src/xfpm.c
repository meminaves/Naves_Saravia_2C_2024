#include "xfpm.h"
#include "gpio_mcu.h"
#include <stdio.h>
#include <stdint.h>

/*==================[macros and definitions]=================================*/

#define V_REF 3.3f                /**< Tensión de referencia */
#define TOTAL_BITS 4096           /**< Cantidad total de bits del ADC */ //A CHEQUEAR
#define ALPHA 0.018f
#define BETA  0.04f
#define PRESSURE_ERROR 1.25f

/*==================[internal data declaration]==============================*/

analog_input_config_t presion_config = {
    .input = CH2,
    .mode = ADC_SINGLE,
};

typedef struct {
    float presion_min;
    float presion_max;
} PressureValues;

/*==================[external functions definition]==========================*/

bool XFPM050KInit(adc_input_t input)
{
	analog_input_config_t presion_config = {
        .input = input,
        .mode = ADC_SINGLE,
    };
	
    if (AnalogInputInit(&presion_config) != SUCCESS) {
        printf("Error: ADC Initialization failed.\n");
        return false;
    }
    return true;
}

PressureValues XFPM050MeasurePressure(adc_input_t input)
{
	    analog_input_config_t presion_config = {
        .input = input,
        .mode = ADC_SINGLE,
    };

    uint16_t valor_sensor;
    float presion = 0;
    float valor = 0;
    PressureValues pressure = {0};

    if (AnalogInputReadSingle(presion_config.input, &valor_sensor) != SUCCESS) {
        printf("Error: lectura fallida ADC .\n");
        return pressure;  // Devuelve un valor por defecto en caso de error
    }

    if (value > TOTAL_BITS || value < 0) {
        printf("Error: lectura fuera de rango.\n");
        return pressure;
    }

    valor = (valor_sensor / 1000.0f);  // Lectura en mV
    float valor_escalado = valor / V_REF;
    float termino_comun = 1 / ALPHA * (valor_escalado - BETA);

    pressure.presion_max = termino_comun + PRESSURE_ERROR;
    pressure.presion_min = termino_comun - PRESSURE_ERROR;

    return pressure;
}

bool XFPM050Deinit()
{
    AnalogInputDeinit(&presion_config);  // Libera recursos del ADC si es necesario
    return true;
}

/*==================[end of file]============================================*/