#ifndef XFPM_H
#define XFPM_H

/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/


/*==================[external data declaration]==============================*/


/*==================[external functions declaration]=========================*/

/** @fn bool Si7007Init(Si7007_config *pins);
 * @brief Initialization function of Si7007.
 * @param[in] *pins
 * @return TRUE if no error.
 */
bool XFPM050Init();

/** @fn uint16_t Si7007MeasureTemperature(void)
 * @brief Measures the current temperature
 * @param[in] No Parameter
 * @return value of temperature in °C
 */
float XFPM050MeasurePressure(void);


/** @fn bool Si7007dEInit(Si7007_config *pins);
 * @brief deinitialization function of Si7007.
 * @param[in] *pins
 * @return TRUE if no error.
 */
bool XFPM050Deinit();

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef SI7007_H */