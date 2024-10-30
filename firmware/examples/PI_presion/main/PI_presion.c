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


#include "led.h"
#include "servo_sg90.h"
#include "pwm_mcu.h"
#include "neopixel_stripe.h"
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*! @brief Período del temporizador en microsegundos */
#define CONFIG_BLINK_PERIOD_TIMER_A 1000000
#define CONFIG_BLINK_PERIOD_TIMER_B 10000000

#define TOTAL_BITS 4096           /**< Cantidad total de bits del ADC */ //A CHEQUEAR

#define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
#define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
#define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */

#define RETARDO_SERVOS 1000

// bool FC1;
// bool FC2;
// false = puerta abierta
// true = puerta cerrada

/*==================[internal data definition]===============================*/

float PRESION_HAB_LIMPIA;

float PRESION_HAB_SUCIA;

float DIF_PRESION;

bool FC1 = true;

bool FC2 = true;

bool ON = true;

TaskHandle_t medirPresiones_task_handle = NULL;

TaskHandle_t servosyLEDs_task_handle = NULL;

TaskHandle_t perifericos_task_handle = NULL;

typedef enum lista_estado_puertas
{
    AMBAS_CERRADAS = 0,
    UNA_CERRADA,
    AMBAS_ABIERTAS,

} estado_puerta;

typedef enum estados_servos
{
    SERVO_ABIERTO = 0,
    SERVO_CERRADO = 90,
} estado_servo;

estado_servo ESTADO_SERVO_1 = SERVO_ABIERTO;
estado_servo ESTADO_SERVO_2 = SERVO_ABIERTO;

estado_puerta ESTADO_ACTUAL = AMBAS_CERRADAS;
estado_puerta ESTADO_ANTERIOR = AMBAS_CERRADAS;
 
/*==================[internal functions declaration]=========================*/

void FuncTimerMedirPresiones(void* param)
{
    vTaskNotifyGiveFromISR(medirPresiones_task_handle, pdFALSE);    	
}
void FuncTimerManejarPerifericos(void* param)
{
    vTaskNotifyGiveFromISR(perifericos_task_handle , pdFALSE);    	
}
// static void medirPresionesTask()
// {
//     while (true)
//     {
       // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        //manejarServosYLEDs();

        // //Mido presiones y almaceno en las variables globales
        // PRESION_HAB_LIMPIA = XFPM050MeasurePressure(CH1); /*El area limpia debe estar a mayor presión*/
        // printf("PRESION HAB LIMPIA: %f\n",PRESION_HAB_LIMPIA);

        // PRESION_HAB_SUCIA = XFPM050MeasurePressure(CH2);
        // printf("PRESION HAB SUCIA: %f",PRESION_HAB_SUCIA);

        // //Hallo el diferencial de presión y lo almaceno en la variable global
        // DIF_PRESION = PRESION_HAB_LIMPIA - PRESION_HAB_SUCIA;
        // printf("DIFERENCIAL DE PRESIÓN: %f\n",DIF_PRESION);
//     }
// }

void leerEstadoDePuertas()
{
    if(FC1 && FC2)
    {
        ESTADO_ACTUAL = AMBAS_CERRADAS;
    }
    else if(FC1 != FC2)
    {
        ESTADO_ACTUAL = UNA_CERRADA;
    }
    else if (!(FC1 || FC2))
    {
        ESTADO_ACTUAL = AMBAS_ABIERTAS;
    }
}

void manejarServosYLEDs()
{
    leerEstadoDePuertas();

    if (ESTADO_ACTUAL != ESTADO_ANTERIOR)
    {
        
        switch (ESTADO_ACTUAL)
        {
        // Puertas cerradas
        case AMBAS_CERRADAS:
            /* LED 1 (verde) encendido
            SERVO_1 OPEN -> Puerta 1 cerrada
            SERVO_2 OPEN -> Puerta 2 cerrada */

            NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);

            if (ESTADO_SERVO_1 != SERVO_ABIERTO)
            {
                ServoMove(SERVO_1, SERVO_ABIERTO);
                ESTADO_SERVO_1 = SERVO_ABIERTO;
            }
            if (ESTADO_SERVO_2 != SERVO_ABIERTO)
            {
                ServoMove(SERVO_2, SERVO_ABIERTO);
                ESTADO_SERVO_2 = SERVO_ABIERTO;
            }

            printf("Ambas puertas cerradas\n");

            break;

        // Una puerta abierta
        case UNA_CERRADA:
            /*
            Luz amarilla
            Puerta 1 abierta -> SERVO_2 cerrado
            o bien
            Puerta 2 abierta -> SERVO_1 cerrado
            */
            NeoPixelAllColor(NEOPIXEL_COLOR_YELLOW);
            printf("Una abierta\n");
            if (FC1 == false) // Puerta 1 abierta
            {
                if (ESTADO_SERVO_2 != SERVO_CERRADO)
                {
                    ServoMove(SERVO_2, SERVO_CERRADO);
                    ESTADO_SERVO_2 = SERVO_CERRADO;
                }

                // A) SERVO1 OPEN y SERVO2 CLOSED
            }
            if (FC2 == false) // Puerta 2 abierta
            {
                // B) SERVO1 CLOSED y SERVO2 OPEN

                if (ESTADO_SERVO_1 != SERVO_CERRADO)
                {
                    ServoMove(SERVO_1, SERVO_CERRADO);
                    ESTADO_SERVO_1 = SERVO_CERRADO;
                }
            }
            break;
        // Dos puertas abiertas
        case AMBAS_ABIERTAS:
            /*
            Error en el sistema, posible falla en el dif de presiones
            */
            printf("Ambas abiertas\n");
            NeoPixelAllColor(NEOPIXEL_COLOR_CYAN);

            break;
        }
    }
}

static void manejarPerifericosTask(){

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ON == true)
        {
            manejarServosYLEDs(); 
        }
        else
        {
            NeoPixelAllOff();
        }

        ESTADO_ANTERIOR = ESTADO_ACTUAL;
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
            printf("Cambia estado puerta 1");    
            printf("FC1 esta en%d: \n",FC1);
			break;
	
		case 'B':
			FC2 = !FC2;
			UartSendByte(UART_PC, (char*)&tecla);
            printf("Cambia estado puerta 2");
            printf("FC2 esta en%d: \n",FC2);
			break;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

    /*Inicialización de los sensores de presión*/
    XFPM050Init(CH1);
    XFPM050Init(CH2);

    ServoInit(SERVO_1, GPIO_22);//Falta definir gpio
    ServoMove(SERVO_1, 90);
    //ServoMove(SERVO_1, -90);

    //ServoInit(SERVO_2, 3)//Falta definir gpio
    static neopixel_color_t color;
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
    NeoPixelAllColor(NEOPIXEL_COLOR_BLUE);
   	
    /* Inicialización de timers */
    timer_config_t timer_medir_presiones = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_TIMER_A,
        .func_p = FuncTimerMedirPresiones, //Aca va la funcion de interrupcion
        .param_p = NULL
    };

            timer_config_t timer_per = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_TIMER_B,
        .func_p = FuncTimerManejarPerifericos, //Aca va la funcion de interrupcion
        .param_p = NULL
    };
    TimerInit(&timer_per);
	TimerStart(timer_per.timer);

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

    //xTaskCreate(&medirPresionesTask, "Medir Presiones", 2048, NULL, 5, &medirPresiones_task_handle);

    xTaskCreate(&manejarPerifericosTask, "Servos y LEDs", 2048, NULL, 5, &perifericos_task_handle);  
}
/*==================[end of file]============================================*/
