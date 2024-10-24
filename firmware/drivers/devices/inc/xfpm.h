#ifndef XFPM_H
#define XFPM_H

/*==================[inclusiones]============================================*/

#include <stdint.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef struct {
    float presion_min;
    float presion_max;
} PressureValues;

/*==================[declaración de datos externos]==========================*/

/*==================[declaración de funciones externas]======================*/

/** 
 * @fn bool XFPM050Init(void);
 * @brief Función de inicialización del sensor de presión XFPM-050KPG.
 * 
 * Inicializa el sensor de presión configurando los pines y ADC necesarios para su funcionamiento.
 * 
 * @return Devuelve TRUE si la inicialización es exitosa, FALSE en caso contrario.
 */
bool XFPM050Init(adc_ch_t input);

/** 
 * @fn float XFPM050MeasurePressure(void);
 * @brief Mide la presión actual detectada por el sensor XFPM-050KPG.
 * 
 * Realiza una lectura del ADC y calcula el valor de presión correspondiente. 
 * La función devuelve el valor en unidades de presión (kPa)
 * 
 * @return Devuelve el valor de la presión medida en el sensor, en kPa.
 */
float XFPM050MeasurePressure(adc_ch_t input);

/** 
 * @fn bool XFPM050Deinit(void);
 * @brief Función de desinicialización del sensor de presión XFPM-050KPG.
 * 
 * Libera los recursos utilizados por el sensor y apaga los pines/ADC utilizados.
 * 
 * @return Devuelve TRUE si la desinicialización es exitosa, FALSE en caso contrario.
 */
//bool XFPM050Deinit(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */

/*==================[fin del archivo]========================================*/
#endif /* #ifndef XFPM_H */