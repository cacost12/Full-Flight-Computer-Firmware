/*******************************************************************************
*                                                                              *
* FILE:                                                                        * 
* 		main.c                                                                 *
*                                                                              *
* DESCRIPTION:                                                                 * 
* 		Processes commands recieved from a host PC, provides fine control over * 
*       flight computer hardware resources                                     *
*                                                                              *
*******************************************************************************/


/*------------------------------------------------------------------------------
 Standard Includes                                                                     
------------------------------------------------------------------------------*/
#include <stdbool.h>
#include "zav_pin_defines_A0002.h"
#include "zav_error.h"


/*------------------------------------------------------------------------------
 Project Includes                                                                     
------------------------------------------------------------------------------*/

/* Drivers */
#include "baro.h"
#include "buzzer.h"
#include "flash.h"
#include "ignition.h"
#include "imu.h"
#include "led.h"
#include "usb.h"

/* Modules */
#include "commands.h"
#include "command_handler.h"
#include "sensor.h"


#include "init.h"
#include "main.h"


/*------------------------------------------------------------------------------
 MCU Peripheral Handlers                                                         
------------------------------------------------------------------------------*/
I2C_HandleTypeDef  baro_hi2c;   /* Baro sensor    */
I2C_HandleTypeDef  imu_hi2c;    /* IMU and GPS    */
SPI_HandleTypeDef  flash_hspi;  /* External flash */
UART_HandleTypeDef usb_huart;   /* USB            */


/*------------------------------------------------------------------------------
 Application entry point                                                      
------------------------------------------------------------------------------*/
int main
	(
 	void
	)
{
/*------------------------------------------------------------------------------
 Local Variables                                                                
------------------------------------------------------------------------------*/
FLASH_CONFIG  flash_config;    /* Flash chip configuration    */
BARO_CONFIG   baro_configs;    /* Baro sensor config settings */
IMU_CONFIG    imu_configs;     /* IMU config settings         */


/*------------------------------------------------------------------------------
 MCU/HAL Initialization                                                                  
------------------------------------------------------------------------------*/
HAL_Init                (); /* Reset peripherals, initialize flash interface 
                               and Systick.                                   */
SystemClock_Config      (); /* System clock                                   */
PeriphCommonClock_Config(); /* Common Peripherals clock                       */
GPIO_Init               (); /* GPIO                                           */
USB_UART_Init           (); /* USB UART                                       */
Baro_I2C_Init           (); /* Barometric pressure sensor                     */
IMU_I2C_Init            (); /* IMU and GPS                                    */
Flash_SPI_Init          (); /* External flash chip                            */


/*------------------------------------------------------------------------------
 Variable Initializations 
------------------------------------------------------------------------------*/

/* Flash Configuration */
flash_config.write_protected   = FLASH_WP_WRITE_ENABLED;
flash_config.bpl_bits          = FLASH_BPL_NONE;
flash_config.bpl_write_protect = FLASH_BPL_READ_WRITE;

/* Baro Configuration */
baro_configs.enable            = BARO_PRESS_TEMP_ENABLED;
baro_configs.mode              = BARO_NORMAL_MODE;
baro_configs.press_OSR_setting = BARO_PRESS_OSR_X8;
baro_configs.temp_OSR_setting  = BARO_TEMP_OSR_X1;
baro_configs.ODR_setting       = BARO_ODR_50HZ;
baro_configs.IIR_setting       = BARO_IIR_COEF_0;

/* IMU Configuration */
imu_configs.sensor_enable      = IMU_ENABLE_GYRO_ACC_TEMP;
imu_configs.acc_odr            = IMU_ODR_100;
imu_configs.gyro_odr           = IMU_ODR_100;
imu_configs.mag_odr            = MAG_ODR_10HZ;
imu_configs.acc_filter         = IMU_FILTER_NORM_AVG4;
imu_configs.gyro_filter        = IMU_FILTER_NORM_AVG4;
imu_configs.acc_filter_mode    = IMU_FILTER_FILTER_MODE;
imu_configs.gyro_filter_mode   = IMU_FILTER_FILTER_MODE;
imu_configs.acc_range          = IMU_ACC_RANGE_16G;
imu_configs.gyro_range         = IMU_GYRO_RANGE_500;
imu_configs.mag_op_mode        = MAG_NORMAL_MODE;
imu_configs.mag_xy_repititions = 9; /* BMM150 Regular Preset Recomendation */
imu_configs.mag_z_repititions  = 15;


/*------------------------------------------------------------------------------
 External Hardware Initializations 
------------------------------------------------------------------------------*/

/* Flash Chip */
if ( flash_init( flash_config ) != FLASH_OK )
	{
	Error_Handler( ERROR_FLASH_INIT_ERROR );
	}

/* Sensor Module - Sets up the sensor sizes/offsets table */
sensor_init();

/* Barometric pressure sensor */
if ( baro_init( &baro_configs ) != BARO_OK )
	{
	Error_Handler( ERROR_BARO_INIT_ERROR );
	}

/* IMU */
if ( imu_init( &imu_configs ) != IMU_OK )
	{
	Error_Handler( ERROR_IMU_INIT_ERROR );
	}

/* Indicate Successful MCU and Peripheral Hardware Setup */
led_set_color( LED_GREEN );


/*------------------------------------------------------------------------------
 Event Loop                                                                  
------------------------------------------------------------------------------*/
while (1)
	{
	command_handler_loop();
	}
} /* main */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/