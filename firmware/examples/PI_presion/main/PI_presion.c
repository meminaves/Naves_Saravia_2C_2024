/*! @mainpage Ejemplo Bluetooth - FFT
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del módulo de comunicación 
 * Bluetooth Low Energy (BLE), junto con el de cálculo de la FFT 
 * de una señal.
 * Permite graficar en una aplicación móvil la FFT de una señal. 
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 02/04/2024 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <xfpm.h>


/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct {

    float presion_min;
    float presion_max;

} PressureValues;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){

   XFPM050Init();
   PressureValues* presion;
   *presion = XFP050MeassurePressure();
}

/*==================[end of file]============================================*/
