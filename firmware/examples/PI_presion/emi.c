// /*! @mainpage Proyecto Integrador
//  *
//  * @section genDesc General Description
//  *

//  *
//  * @section changelog Changelog
//  *
//  * |   Date	    | Description                                    |
//  * |:----------:|:-----------------------------------------------|
//  * | 02/04/2024 | Document creation		                         |
//  *
//  * @author Albano Peñalva (albano.penalva@uner.edu.ar)
//  *
//  */

// /*==================[inclusions]=============================================*/
// #include <stdio.h>
// #include <stdint.h>
// #include <xfpm.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "timer_mcu.h"
// #include "uart_mcu.h"
// #include "switch.h"
// #include "neopixel_stripe.h"
// #include "servo_sg90.h"
// #include <gpio_mcu.h>
// #include "ble_mcu.h"
// /*==================[macros and definitions]=================================*/

// typedef struct {

//     float presion_min;
//     float presion_max;

// } PressureValues;

// /*! @brief Período del temporizador en microsegundos */
// #define CONFIG_BLINK_PERIOD_TIMER_A  1000000
// #define CONFIG_BLINK_PERIOD_TIMER_B  500000

// typedef enum lista_estado_puertas
// {
//     AMBAS_CERRADAS = 0,
//     UNA_CERRADA,
//     AMBAS_ABIERTAS,

// } estado_puerta;

// typedef enum estados_servos
// {
//     SERVO_ABIERTO = 0,
//     SERVO_CERRADO = 90,

// } estado_servo;

// estado_servo ESTADO_SERVO_1 = SERVO_ABIERTO;
// estado_servo ESTADO_SERVO_2 = SERVO_ABIERTO;

// int ESTADO_ACTUAL_PUERTAS = AMBAS_CERRADAS;
// int ESTADO_ANTERIOR_PUERTAS = AMBAS_CERRADAS;


// /*==================[internal data definition]===============================*/

// float PRESION_HAB_LIMPIA;

// float PRESION_HAB_SUCIA;

// float DIF_PRESION;

// bool FC1 = true; //A chequear

// bool FC2 = true; //A chequear

// bool ON = true; 

// TaskHandle_t medirPresiones_task_handle = NULL;

// TaskHandle_t perifericos_task_handle = NULL;

// /*==================[internal functions declaration]=========================*/

// void FuncTimerMedirPresiones(void* param)
// {
//     vTaskNotifyGiveFromISR(medirPresiones_task_handle, pdFALSE);    	
// }

// void FuncTimerManejarPerifericos(void* param)
// {
//     vTaskNotifyGiveFromISR(perifericos_task_handle , pdFALSE);    	
// }

// static void medirPresionesTask()
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         //Mido presiones y almaceno en las variables globales
//         PRESIONES_HAB_LIMPIA = XFPM050MeasurePressure(CH1); /*El area limpia debe estar a mayor presión*/
//         PRESIONES_HAB_SUCIA = XFPM050MeasurePressure(CH2);

//         //Hallo el diferencial de presión y lo almaceno en la variable global
//         DIF_PRESION = PRESIONES_HAB_LIMPIA->presion_min - PRESIONES_HAB_SUCIA->presion_max;
//     }
// }

// void tecla1()
// {
//     ON = !ON
// }

// void detectarFC()
// {
// 	uint8_t tecla;
// 	UartReadByte(UART_PC, &tecla);
// 	switch (tecla)
// 	{
// 		case 'A':
// 			FC1 = !FC1;
// 			UartSendByte(UART_PC, (char*)&tecla);
// 			break;
	
// 		case 'B':
// 			FC2 = !FC2;
// 			UartSendByte(UART_PC, (char*)&tecla);
// 			break;
// 	}
// }

// void leerEstadoDePuertas()
// {
//     if(FC1 && FC2)
//     {
//         ESTADO_ACTUAL_PUERTAS = AMBAS_CERRADAS;
//     }
//     else if(FC1 != FC2)
//     {
//         ESTADO_ACTUAL_PUERTAS = UNA_CERRADA;
//     }
//     else if (!(FC1 || FC2))
//     {
//         ESTADO_ACTUAL_PUERTAS = AMBAS_ABIERTAS;
//     }
// }

// static void manejarPerifericosTask(){
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         if(ON == true)
//         {
//             manejarServosYLEDs(); 
//         }
//         else
//         {
//             NeoPixelAllOff();
//         }

//         ESTADO_ANTERIOR_PUERTAS = ESTADO_ACTUAL_PUERTAS;
//     }
// }

// void manejarServosYLEDs()
// {
//     leerEstadoDePuertas();

//     if (ESTADO_ACTUAL_PUERTAS != ESTADO_ANTERIOR_PUERTAS)
//     {
        
//         switch (ESTADO_ACTUAL_PUERTAS)
//         {
//         // Puertas cerradas
//         case AMBAS_CERRADAS:
//             /* LED 1 (verde) encendido
//             SERVO_1 OPEN -> Puerta 1 cerrada
//             SERVO_2 OPEN -> Puerta 2 cerrada */

//             NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);

//             if (ESTADO_SERVO_1 != SERVO_ABIERTO)
//             {
//                 ServoMove(SERVO_1, SERVO_ABIERTO);
//                 ESTADO_SERVO_1 = SERVO_ABIERTO;
//             }
//             if (ESTADO_SERVO_2 != SERVO_ABIERTO)
//             {
//                 ServoMove(SERVO_2, SERVO_ABIERTO);
//                 ESTADO_SERVO_2 = SERVO_ABIERTO;
//             }

//             printf("Ambas puertas cerradas\n");

//             break;

//         // Una puerta abierta
//         case UNA_CERRADA:
//             /*
//             Luz amarilla
//             Puerta 1 abierta -> SERVO_2 cerrado
//             o bien
//             Puerta 2 abierta -> SERVO_1 cerrado
//             */
//             NeoPixelAllColor(NEOPIXEL_COLOR_YELLOW);
//             printf("Una abierta\n");
//             if (FC1 == false) // Puerta 1 abierta
//             {
//                 if (ESTADO_SERVO_2 != SERVO_CERRADO)
//                 {
//                     ServoMove(SERVO_2, SERVO_CERRADO);
//                     ESTADO_SERVO_2 = SERVO_CERRADO;
//                 }

//                 // A) SERVO1 OPEN y SERVO2 CLOSED
//             }
//             if (FC2 == false) // Puerta 2 abierta
//             {
//                 // B) SERVO1 CLOSED y SERVO2 OPEN

//                 if (ESTADO_SERVO_1 != SERVO_CERRADO)
//                 {
//                     ServoMove(SERVO_1, SERVO_CERRADO);
//                     ESTADO_SERVO_1 = SERVO_CERRADO;
//                 }
//             }
//             break;
//         // Dos puertas abiertas
//         case AMBAS_ABIERTAS:
//             /*
//             Error en el sistema, posible falla en el dif de presiones
//             */
//             printf("Ambas abiertas\n");
//             NeoPixelAllColor(NEOPIXEL_COLOR_CYAN);

//             break;
//         }
//     }
// }

// //A AÑADIR AL MAIN JUNTO A LA CONFIGURACION EN EL MAIN

// #define DIF_PRESION_MIN 5 //el diferencial de presión debe ser de al menos 5 kPa

// void enviar_datos_bt_Task()
// {
// while (true)
// {
//     switch(BleStatus())
//     {
//             case BLE_OFF:
//                 LedOff(LED_BT);
//             break;
//             case BLE_DISCONNECTED:
//                 LedToggle(LED_BT);
//             break;
//             case BLE_CONNECTED:
//                 LedOn(LED_BT);
//             break;
//     }

//     char presion_hab_limpia_str[20];
//     char presion_hab_sucia_str[20];
//     char presion_diferencial_str[20];

//     snprintf(presion_hab_limpia_str, sizeof(presion_hab_limpia_str), "*L%.2f*", PRESION_HAB_LIMPIA);
//     snprintf(presion_hab_sucia_str, sizeof(presion_hab_sucia_str), "*S%.2f*", PRESION_HAB_SUCIA);
//     snprintf(presion_diferencial_str, sizeof(presion_diferencial_str), "*D%.2f*", DIF_PRESION);

//     BleSendString(presion_hab_limpia_str);
//     BleSendString(presion_hab_sucia_str);
//     BleSendString(presion_diferencial_str);

//         if (DIF_PRESION < DIF_PRESION_MIN)
//         {
//             char luz[10] = "*FR255G0B0*";
//             BleSendString(luz);
//             char volumen[10] = "*VV100*";
//             BleSendString(volumen);
//         }

//         //vTaskDelay(); Cada un segundo podria ser
// }

// }
// //
// /*==================[external functions definition]==========================*/
// void app_main(void){

//     /*Inicialización de los sensores de presión*/
//     XFPM050Init(CH1);
//     XFPM050Init(CH2);

//     SwitchesInit();
//     SwitchActivInt(SWITCH_1, *tecla1, NULL); 
   
//    	/* Inicialización de timers */
//     timer_config_t timer_medir_presiones = {
//         .timer = TIMER_A,
//         .period = CONFIG_BLINK_PERIOD_TIMER_A,
//         .func_p = FuncTimerMedirPresiones, //Aca va la funcion de interrupcion
//         .param_p = NULL
//     };
//     TimerInit(&timer_medir_presiones);
// 	TimerStart(timer_medir_presiones.timer);

//         timer_config_t timer_per = {
//         .timer = TIMER_B,
//         .period = CONFIG_BLINK_PERIOD_TIMER_B,
//         .func_p = FuncTimerManejarPerifericos, //Aca va la funcion de interrupcion
//         .param_p = NULL
//     };
//     TimerInit(&timer_per);
// 	TimerStart(timer_per.timer);

//     //Puerto Serie
// 		serial_config_t myUart = {
// 		.port = UART_PC,
// 		.baud_rate = 9600,
// 		.func_p = detectarFC,
// 		.param_p = NULL,
// 	};


// 	UartInit(&myUart);
	
//     xTaskCreate(&medirPresionesTask, "Medir Presiones", 2048, NULL, 5, &medirPresiones_task_handle);
//     xTaskCreate(&manejarPerifericosTask, "Servos y LEDs", 2048, NULL, 5, &perifericos_task_handle);
// }

// /*==================[end of file]============================================*/
