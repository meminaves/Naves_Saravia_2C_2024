// /*==================[inclusions]=============================================*/
// #include <stdio.h>
// #include <stdint.h>
// #include "led.h"
// #include "servo_sg90.h"
// #include "pwm_mcu.h"
// #include "neopixel_stripe.h"
// #include <gpio_mcu.h>

// /*==================[macros and definitions]=================================*/


// /*==================[internal data definition]===============================*/
// #define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
// #define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
// #define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */
// bool FC1;
// bool FC2;
// // false = puerta abierta
// // true = puerta cerrada

// /*==================[internal functions declaration]=========================*/
// //void ServoMove(servo_out_t servo, int8_t ang)
// //void NeoPixelAllColor(neopixel_color_t color)
// void manejarServosYLEDs(){
//     //Puertas cerradas
//     if (FC1 && FC2)//AND
//     {
//         /* LED1(verde) ON
//         SERVO1 OPEN
//         SERVO2 OPEN */
//         NeoPixelAllOff();
//         NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);
//         ServoMove(SERVO_1, 0);
//         ServoMove(SERVO_2, 0);
//     }
//     //Una puerta abierta
//     if (FC1 != FC2)//XOR
//     {
//         NeoPixelAllOff();
//         NeoPixelAllColor(NEOPIXEL_COLOR_YELLOW);
//         //LED2(amarillo) ON
//         if (FC1 == false)//Puerta 1 abierta
//         {
//             ServoMove(SERVO_1, 0);
//             ServoMove(SERVO_2, 90);
//             //A) SERVO1 OPEN y SERVO2 CLOSED
//         }
//         if (FC2 == false)//Puerta 2 abierta
//         {
//             ServoMove(SERVO_1, 90);
//             ServoMove(SERVO_2, 0);
//             //B) SERVO1 CLOSED y SERVO2 OPEN
//             /* code */
//         }
//     }
//     //Dos puertas abiertas
//     else if (!(FC1 || FC2))//NOR
//     {
//         NeoPixelAllOff();
//         NeoPixelAllColor(NEOPIXEL_COLOR_RED);
//         /* LED3(rojo) ON
//         Alarma sonora */
//     }
    
    
    
// }

// /*==================[external functions definition]==========================*/
// //uint8_t ServoInit(servo_out_t servo, gpio_t gpio);
// //void NeoPixelInit(gpio_t pin, uint16_t len, neopixel_color_t *color_array)
// void app_main(void){
//     ServoInit(SERVO_1, )//Falta definir gpio
//     ServoInit(SERVO_2, )//Falta definir gpio
//    //Iniciar servos
//    //Iniciar neopixel
//     static neopixel_color_t color;
//     NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
// }

// /*==================[end of file]============================================*/
// #include "gpio_mcu.h"

// #define GPIO_FC1 GPIO_X
// #define GPIO_FC2 GPIO_Y

// int8_t FCInit(gpio_t pin){
// 	/* GPIO configurations */
// 	GPIOInit(pin, GPIO_INPUT);	// FC
// 	return true;
// }

// int8_t FCRead(gpio_t pin){

//     return GPIORead(pin);
    
// }
#include "ble_mcu.h"
#define LED_BT	LED_1


float PRESION_HAB_LIMPIA;

float PRESION_HAB_SUCIA;

float DIF_PRESION;

//main
ble_config_t ble_configuration = {
    "ESP_EDU_1",
    read_data
};

BleInit(&ble_configuration);

//para verificar la conexión del bt
 while(1){
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){
            case BLE_OFF:
                LedOff(LED_BT);
            break;
            case BLE_DISCONNECTED:
                LedToggle(LED_BT);
            break;
            case BLE_CONNECTED:
                LedOn(LED_BT);
            break;
        }
    }

void enviar_datos_bt(){

char presion_hab_limpia_str[20];
char presion_hab_sucia_str[20];
char presion_diferencial_str[20];


snprintf(presion_hab_limpia_str, sizeof(presion_hab_limpia_str), "*L%.2f*", PRESION_HAB_LIMPIA);
snprintf(presion_hab_sucia_str, sizeof(presion_hab_sucia_str), "*S%.2f*", PRESION_HAB_SUCIA);
snprintf(presion_diferencial_str, sizeof(presion_diferencial_str), "*D%.2f*", DIF_PRESION);

BleSendString(presion_hab_limpia_str);
BleSendString(presion_hab_sucia_str);

}