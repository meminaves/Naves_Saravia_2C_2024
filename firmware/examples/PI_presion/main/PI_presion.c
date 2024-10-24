/*! @mainpage Proyecto Integrador
 *
 * @section genDesc General Description
 *

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
#include "xfpm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*! @brief Período del temporizador en microsegundos */
#define CONFIG_BLINK_PERIOD_TIMER_A 2000 
#define TOTAL_BITS 4096           /**< Cantidad total de bits del ADC */ //A CHEQUEAR


/*==================[internal data definition]===============================*/

float PRESION_HAB_LIMPIA;

float PRESION_HAB_SUCIA;

float DIF_PRESION;

bool FC1 = true;

bool FC2 = true;

TaskHandle_t medirPresiones_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

void FuncTimerMedirPresiones(void* param)
{
    vTaskNotifyGiveFromISR(medirPresiones_task_handle, pdFALSE);    	
}

static void medirPresionesTask()
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        //Mido presiones y almaceno en las variables globales
        PRESION_HAB_LIMPIA = XFPM050MeasurePressure(CH1); /*El area limpia debe estar a mayor presión*/
        printf("PRESION HAB LIMPIA: %f\n",PRESION_HAB_LIMPIA);

        //PRESION_HAB_SUCIA = XFPM050MeasurePressure(CH2);
        //printf("PRESION HAB SUCIA: %f",PRESION_HAB_SUCIA);

        //Hallo el diferencial de presión y lo almaceno en la variable global
        DIF_PRESION = PRESION_HAB_LIMPIA - PRESION_HAB_SUCIA;
        printf("DIFERENCIAL DE PRESIÓN: %f\n",DIF_PRESION);
    }
}
void detectarFC()
{
	uint8_t tecla;
	UartReadByte(UART_PC, &tecla);
	switch (tecla)
	{
		case 'A':
			FC1 = !FC1;
			UartSendByte(UART_PC, (char*)&tecla);
			break;
	
		case 'B':
			FC2 = !FC2;
			UartSendByte(UART_PC, (char*)&tecla);
			break;
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){

    /*Inicialización de los sensores de presión*/
   XFPM050Init(CH1);
   //XFPM050Init(CH2);
   
   	/* Inicialización de timers */
    timer_config_t timer_medir_presiones = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_TIMER_A,
        .func_p = FuncTimerMedirPresiones, //Aca va la funcion de interrupcion
        .param_p = NULL
    };
    TimerInit(&timer_medir_presiones);
	TimerStart(timer_medir_presiones.timer);

    //Puerto Serie
		serial_config_t myUart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = detectarFC,
		.param_p = NULL,
	};

	UartInit(&myUart);
    xTaskCreate(&medirPresionesTask, "Medir Presiones", 2048, NULL, 5, &medirPresiones_task_handle);
}

/*==================[end of file]============================================*/
