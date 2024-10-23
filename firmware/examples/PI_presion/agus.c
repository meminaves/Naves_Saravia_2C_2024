/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include "servo_sg90.h"
#include "pwm_mcu.h"

/*==================[macros and definitions]=================================*/


/*==================[internal data definition]===============================*/

bool FC1;
bool FC2;

/*==================[internal functions declaration]=========================*/
//void ServoMove(servo_out_t servo, int8_t ang)

void manejoServos(){
    if (FC1 && FC2)//Puertas cerradas
    {
        /* LED1(verde) ON
        SERVO1 OPEN */

    }
    else if (FC1 != FC2)//Una puerta abierta
    {
        //LED2(amarillo) ON
        if (FC1 == OFF)//Puerta 1 abierta
        {
            ServoMove(SERVO_2, 90);
            //A) SERVO1 OPEN y SERVO2 CLOSED
            /* code */
        }
        if (FC2 == OFF)//Puerta 2 abierta
        {
            //B) SERVO1 CLOSED y SERVO2 OPEN
            /* code */
        }
    }
    else if (!(FC1 || FC2))//Dos puertas abiertas
    {
        /* LED3(rojo) ON
        Alarma sonora */
    }
    
    
    
}

/*==================[external functions definition]==========================*/
void app_main(void){
   //Iniciar servos
   
}

/*==================[end of file]============================================*/
