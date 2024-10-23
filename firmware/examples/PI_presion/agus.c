/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include "servo_sg90.h"
#include "pwm_mcu.h"
#include "neopixel_stripe.h"
#include <gpio_mcu.h>

/*==================[macros and definitions]=================================*/


/*==================[internal data definition]===============================*/
#define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
#define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
#define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */
bool FC1;
bool FC2;
// false = puerta abierta
// true = puerta cerrada

/*==================[internal functions declaration]=========================*/
//void ServoMove(servo_out_t servo, int8_t ang)
//void NeoPixelAllColor(neopixel_color_t color)
void manejarServosYLEDs(){
    //Puertas cerradas
    if (FC1 && FC2)//AND
    {
        /* LED1(verde) ON
        SERVO1 OPEN
        SERVO2 OPEN */
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_GREEN);
        ServoMove(SERVO_1, 0);
        ServoMove(SERVO_2, 0);
    }
    //Una puerta abierta
    if (FC1 != FC2)//XOR
    {
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_YELLOW);
        //LED2(amarillo) ON
        if (FC1 == false)//Puerta 1 abierta
        {
            ServoMove(SERVO_1, 0);
            ServoMove(SERVO_2, 90);
            //A) SERVO1 OPEN y SERVO2 CLOSED
        }
        if (FC2 == false)//Puerta 2 abierta
        {
            ServoMove(SERVO_1, 90);
            ServoMove(SERVO_2, 0);
            //B) SERVO1 CLOSED y SERVO2 OPEN
            /* code */
        }
    }
    //Dos puertas abiertas
    else if (!(FC1 || FC2))//NOR
    {
        NeoPixelAllOff();
        NeoPixelAllColor(NEOPIXEL_COLOR_RED);
        /* LED3(rojo) ON
        Alarma sonora */
    }
    
    
    
}

/*==================[external functions definition]==========================*/
//uint8_t ServoInit(servo_out_t servo, gpio_t gpio);
//void NeoPixelInit(gpio_t pin, uint16_t len, neopixel_color_t *color_array)
void app_main(void){
    ServoInit(SERVO_1, )//Falta definir gpio
    ServoInit(SERVO_2, )//Falta definir gpio
   //Iniciar servos
   //Iniciar neopixel
    static neopixel_color_t color;
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, BUILT_IN_RGB_LED_LENGTH, &color);
}

/*==================[end of file]============================================*/
