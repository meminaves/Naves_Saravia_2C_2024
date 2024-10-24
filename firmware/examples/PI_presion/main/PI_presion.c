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

#include <stdbool.h>
#include "led.h"
#include "servo_sg90.h"
#include "pwm_mcu.h"
#include "neopixel_stripe.h"




/*==================[macros and definitions]=================================*/

/*! @brief Período del temporizador en microsegundos */
#define CONFIG_BLINK_PERIOD_TIMER_A 1000000
#define TOTAL_BITS 4096           /**< Cantidad total de bits del ADC */ //A CHEQUEAR

#define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
#define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
#define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */

#define RETARDO_SERVOS 1000

#define MIN_ANG		-90
#define MAX_ANG		90


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

TaskHandle_t medirPresiones_task_handle = NULL;

TaskHandle_t servosyLEDs_task_handle = NULL;

 
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

        //manejarServosYLEDs();

        // //Mido presiones y almaceno en las variables globales
        // PRESION_HAB_LIMPIA = XFPM050MeasurePressure(CH1); /*El area limpia debe estar a mayor presión*/
        // printf("PRESION HAB LIMPIA: %f\n",PRESION_HAB_LIMPIA);

        // PRESION_HAB_SUCIA = XFPM050MeasurePressure(CH2);
        // printf("PRESION HAB SUCIA: %f",PRESION_HAB_SUCIA);

        // //Hallo el diferencial de presión y lo almaceno en la variable global
        // DIF_PRESION = PRESION_HAB_LIMPIA - PRESION_HAB_SUCIA;
        // printf("DIFERENCIAL DE PRESIÓN: %f\n",DIF_PRESION);
    }
}
void manejarServosYLEDs(){
    //Puertas cerradas
    printf("Arranca función");
    if (FC1 && FC2)//AND
    {
        printf("LED VERDE\n");
        /* LED1(verde) ON
        SERVO1 OPEN
        SERVO2 OPEN */
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);
        printf("Se mueve SERVO_1  a 0°(ABIERTO)\n");
        ServoMove(SERVO_1, 0);
        printf("Se mueve SERVO_2  a 0°(ABIERTO)\n");
        ServoMove(SERVO_2, 0);
    }
    //Una puerta abierta
    if (FC1 != FC2)//XOR
    {
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_YELLOW);
        printf("LED AMARILLO\n");
        //LED2(amarillo) ON
        if (FC1 == false)//Puerta 1 abierta
        //FC2=1 y FC0=1
        {
            printf("Se mueve SERVO_1  a 0°(ABIERTO)\n");
            ServoMove(SERVO_1, 0);
            printf("Se mueve SERVO_2  a 90°(CERRADO)\n");
            ServoMove(SERVO_2, 90);
            //A) SERVO1 OPEN y SERVO2 CLOSED
        }
        if (FC2 == false)//Puerta 2 abierta
        //FC2=0 y FC1=1
        {
            printf("Se mueve SERVO_1  a 90°(CERRADO)\n");
            ServoMove(SERVO_1, 90);
            printf("Se mueve SERVO_2  a 0°(ABIERTO)\n");
            ServoMove(SERVO_2, 0);
            //B) SERVO1 CLOSED y SERVO2 OPEN
            /* code */
        }
    }
    //Dos puertas abiertas
    if (!(FC1 || FC2))//NOR
    {
        printf("LED ROJO\n");
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_RED);
        /* LED3(rojo) ON
        Alarma sonora */
    }
}

static void servosyLEDS_task(){
    while (true)
    {
        
        manejarServosYLEDs();
        vTaskDelay(RETARDO_SERVOS / portTICK_PERIOD_MS);
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
            printf("Cambia estado puerta 1\n");
            
            printf("FC1 esta en: %d\n",FC1);
			break;
	
		case 'B':
			FC2 = !FC2;
			UartSendByte(UART_PC, (char*)&tecla);
            printf("Cambia estado puerta 2\n");
            printf("FC2 esta en: %d\n",FC2);
			break;
	}
}

//void ServoMove(servo_out_t servo, int8_t ang)
//void NeoPixelAllColor(neopixel_color_t color)

/*==================[external functions definition]==========================*/
void app_main(void){

    /*Inicialización de los sensores de presión*/
   XFPM050Init(CH1);
   XFPM050Init(CH2);

    ServoInit(SERVO_1, GPIO_0);//Falta definir gpio
    ServoMove(SERVO_1, 45);
    //ServoInit(SERVO_2, 3)//Falta definir gpio
    static neopixel_color_t color;
    color = NEOPIXEL_COLOR_BLUE;
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
    NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);
    
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

    xTaskCreate(&servosyLEDS_task, "Servos y LEDs", 2048, NULL, 5, &servosyLEDs_task_handle);

//uint8_t ServoInit(servo_out_t servo, gpio_t gpio);
//void NeoPixelInit(gpio_t pin, uint16_t len, neopixel_color_t *color_array)
  
    

   
   //Iniciar servos
   //Iniciar neopixel
   

    
}
/*==================[end of file]============================================*/
