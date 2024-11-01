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
#include "ble_mcu.h"

#include "led.h"
#include "servo_sg90.h"
#include "pwm_mcu.h"
#include "neopixel_stripe.h"
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*! @brief Período del temporizador en microsegundos */
#define CONFIG_BLINK_PERIOD_TIMER_A 1000000
#define CONFIG_BLINK_PERIOD_TIMER_B 1000000
#define CONFIG_BLINK_PERIOD_TIMER_C 500

#define TOTAL_BITS 4096           /**< Cantidad total de bits del ADC */ //A CHEQUEAR

#define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
#define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
#define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */

#define RETARDO_SERVOS 1000
#define LED_BT LED_1

float DIF_PRESION_MIN = 5.0f;

// bool FC1;
// bool FC2;
// false = puerta abierta
// true = puerta cerrada

/*==================[internal data definition]===============================*/
#define GPIO_FC1 GPIO_20
#define GPIO_FC2 GPIO_22

int8_t FCInit(gpio_t pin)
{
	/* GPIO configurations */
	GPIOInit(pin, GPIO_INPUT);	// FC
	return true;
}

int8_t FCRead(gpio_t pin)
{
    return GPIORead(pin);
}

float PRESION_HAB_LIMPIA;

float PRESION_HAB_SUCIA;

float DIF_PRESION;

bool FC1;

bool FC2;

bool ON = true;

TaskHandle_t medirPresiones_task_handle = NULL;

TaskHandle_t FCs_task_handle = NULL;

TaskHandle_t perifericos_task_handle = NULL;

typedef enum lista_estado_puertas
{
    AMBAS_CERRADAS,
    UNA_CERRADA,
    AMBAS_ABIERTAS,

} estado_puerta;

typedef enum estados_servos
{
    SERVO_ABIERTO = 45,
    SERVO_CERRADO = -45,

} estado_servo;

estado_servo ESTADO_SERVO_1 = SERVO_ABIERTO;
estado_servo ESTADO_SERVO_2 = SERVO_ABIERTO;

estado_puerta ESTADO_ACTUAL_PUERTAS = -1;
estado_puerta ESTADO_ANTERIOR_PUERTAS = -1;

bool ESTADO_ACTUAL_DIFERENCIAL_PRESION ;
bool ESTADO_ANTERIOR_DIFERENCIAL_PRESION = -1;
 
/*==================[internal functions declaration]=========================*/

void FuncTimerMedirPresiones(void* param)
{
    vTaskNotifyGiveFromISR(medirPresiones_task_handle, pdFALSE);    	
}

void FuncTimerManejarPerifericos(void* param)
{
    vTaskNotifyGiveFromISR(perifericos_task_handle , pdFALSE);    	
}

// void FuncTimerFCs(void* param)
// {
//     vTaskNotifyGiveFromISR(FCs_task_handle , pdFALSE);    	
// }

static void medirPresionesTask()
{
    while (true)
    {
       ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        //Mido presiones y almaceno en las variables globales
        PRESION_HAB_LIMPIA = XFPM050MeasurePressure(CH1); /*El area limpia debe estar a mayor presión*/
        printf("PRESION HAB LIMPIA: %f\n",PRESION_HAB_LIMPIA);

        PRESION_HAB_SUCIA = XFPM050MeasurePressure(CH2);
        printf("PRESION HAB SUCIA: %f\n",PRESION_HAB_SUCIA);

        //Hallo el diferencial de presión y lo almaceno en la variable global
        DIF_PRESION = PRESION_HAB_LIMPIA - PRESION_HAB_SUCIA;
        printf("DIFERENCIAL DE PRESIÓN: %f\n",DIF_PRESION);
    }
}

void leerEstadoDePuertas()
{
    //ESTADO DE PUERTAS
    if(FC1 && FC2)
    {
        ESTADO_ACTUAL_PUERTAS = AMBAS_ABIERTAS;
    }
    else if(FC1 != FC2)
    {
        ESTADO_ACTUAL_PUERTAS = UNA_CERRADA;
    }
    else if (!(FC1 || FC2))
    {
        ESTADO_ACTUAL_PUERTAS = AMBAS_CERRADAS;
    }
}

void leerEstadoDiferencial()
{
    if (DIF_PRESION < DIF_PRESION_MIN)
    {
        ESTADO_ACTUAL_DIFERENCIAL_PRESION = false;
    }
    else 
    {
        ESTADO_ACTUAL_DIFERENCIAL_PRESION = true;
    }
    
}
void moverServos()
{
      switch (ESTADO_ACTUAL_PUERTAS)
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
                NeoPixelAllColor(NEOPIXEL_COLOR_RED);

                break;
            }
}
void manejarServosYLEDs()
{
    leerEstadoDiferencial();

    printf("ESTADO_DIF: %d", ESTADO_ACTUAL_DIFERENCIAL_PRESION);
    printf("\n");
    printf("ESTADO_PUERTAS_Actual: %d", ESTADO_ACTUAL_PUERTAS);
    printf("ESTADO_PUERTAS_Anterior: %d", ESTADO_ANTERIOR_PUERTAS);
    printf("\n");

    if (ESTADO_ACTUAL_DIFERENCIAL_PRESION != ESTADO_ANTERIOR_DIFERENCIAL_PRESION)
    {
            if (ESTADO_ACTUAL_DIFERENCIAL_PRESION == false)
            {
                NeoPixelAllColor(NEOPIXEL_COLOR_ORANGE);
                
                if (ESTADO_SERVO_2 != SERVO_CERRADO)
                {
                    ServoMove(SERVO_2, SERVO_CERRADO);
                    ESTADO_SERVO_2 = SERVO_CERRADO;
                }

                if (ESTADO_SERVO_1 != SERVO_CERRADO)
                {
                    ServoMove(SERVO_1, SERVO_CERRADO);
                    ESTADO_SERVO_1 = SERVO_CERRADO;
                }    
            }

            else
            {
                moverServos();
            }
    }
    // Si se cumple el diferencial de presion...
    if(ESTADO_ACTUAL_DIFERENCIAL_PRESION == true)
    { 

        leerEstadoDePuertas();

        if (ESTADO_ACTUAL_PUERTAS != ESTADO_ANTERIOR_PUERTAS)
        {
            moverServos();
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

        ESTADO_ANTERIOR_PUERTAS = ESTADO_ACTUAL_PUERTAS;
        ESTADO_ANTERIOR_DIFERENCIAL_PRESION = ESTADO_ACTUAL_DIFERENCIAL_PRESION;
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
void enviar_datos_bt()
{
    // switch(BleStatus())
    // {
    //         case BLE_OFF:
    //             LedOff(LED_BT);
    //         break;
    //         case BLE_DISCONNECTED:
    //             LedToggle(LED_BT);
    //         break;
    //         case BLE_CONNECTED:
    //             LedOn(LED_BT);
    //         break;
    // }

    char presion_hab_limpia_str[20];
    char presion_hab_sucia_str[20];
    char presion_diferencial_str[20];

    snprintf(presion_hab_limpia_str, sizeof(presion_hab_limpia_str), "*L%.2f*", PRESION_HAB_LIMPIA);
    snprintf(presion_hab_sucia_str, sizeof(presion_hab_sucia_str), "*S%.2f*", PRESION_HAB_SUCIA);
    snprintf(presion_diferencial_str, sizeof(presion_diferencial_str), "*D%.2f*", DIF_PRESION);

    BleSendString(presion_hab_limpia_str);
    BleSendString(presion_hab_sucia_str);
    BleSendString(presion_diferencial_str);

        if (DIF_PRESION < DIF_PRESION_MIN)
        {
            // char luz[20] = "*FR255G0B0*";
            // BleSendString(luz);
            // char volumen[10] = "*VV100*";
            // BleSendString(volumen);
        }

        //vTaskDelay(CONFIG_BLINK_PERIOD_TIMER_C/portTICK_PERIOD_MS); 
}


void leerFCsTask()
{
    while (true)
    {
 

        printf("Leyendo finales de carrera...\n");
        printf("FC1: %d", FC1);
        printf("\n");
        printf("FC2: %d", FC2);
        printf("\n");

        FC1 = FCRead(GPIO_FC1);
        FC2 = FCRead(GPIO_FC2);

        vTaskDelay(CONFIG_BLINK_PERIOD_TIMER_C / portTICK_PERIOD_MS);

    }
}

/*==================[external functions definition]==========================*/
void app_main(void){

    /*Inicialización de los sensores de presión*/
    XFPM050Init(CH1);
    XFPM050Init(CH2);

    ble_config_t ble_configuration = {
    "ESP_EDU_1",
    BLE_NO_INT
    };

BleInit(&ble_configuration);


    ServoInit(SERVO_1, GPIO_23);//Falta definir gpio
    ServoInit(SERVO_2, GPIO_21);
    ServoMove(SERVO_1, SERVO_ABIERTO);
    ServoMove(SERVO_2, SERVO_ABIERTO);

    static neopixel_color_t color;
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
    NeoPixelAllColor(NEOPIXEL_COLOR_BLUE);
   	
    FCInit(GPIO_FC1);
    FCInit(GPIO_FC2);

    /* Inicialización de timers */
    timer_config_t timer_medir_presiones = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_TIMER_A,
        .func_p = FuncTimerMedirPresiones, //Aca va la funcion de interrupcion
        .param_p = NULL
    };
    TimerInit(&timer_medir_presiones);
	TimerStart(timer_medir_presiones.timer);

        timer_config_t timer_per = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_TIMER_B,
        .func_p = FuncTimerManejarPerifericos, //Aca va la funcion de interrupcion
        .param_p = NULL
    };
    TimerInit(&timer_per);
	TimerStart(timer_per.timer);

    //     timer_config_t timer_FCs = {
    //     .timer = TIMER_C,
    //     .period = CONFIG_BLINK_PERIOD_TIMER_C,
    //     .func_p = FuncTimerFCs, //Aca va la funcion de interrupcion
    //     .param_p = NULL
    // };
    // TimerInit(&timer_FCs);
	// TimerStart(timer_FCs.timer);

    //Puerto Serie
		serial_config_t myUart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = detectarFC,
		.param_p = NULL,
	};

	UartInit(&myUart);

    xTaskCreate(&medirPresionesTask, "Medir Presiones", 2048, NULL, 5, &medirPresiones_task_handle);

    xTaskCreate(&leerFCsTask, "Leer FCs", 2048, NULL, 5, &FCs_task_handle);

    xTaskCreate(&manejarPerifericosTask, "Servos y LEDs", 2048, NULL, 5, &perifericos_task_handle);  
}
/*==================[end of file]============================================*/
