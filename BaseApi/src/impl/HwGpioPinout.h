/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaGpioPinout.h
 *   Date     : Oct 17, 2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  ...
 */
/*------------------------------------------------------------------------------
 */
#ifndef HWGPIOPINOUT_H_
#define HWGPIOPINOUT_H_
/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

//Todo: naming convention

// One Wire Bus
#define BA_BUS1W    04
#define BA_W1TEMPFAM 28

// Serial Interface
#define BA_SERTXD0  14
#define BA_SERRXD0  15
#define BA_SER_ALT   0

// I2C GPIOs
#define BA_I2CSDA1  02
#define BA_I2CSCL1  03
#define BA_I2C_ALT   0

// Hardware PWM
#define BA_PWM0     18
#define BA_PWM0_ALT  5
#define BA_PWM1     19 // Not sure
#define BA_PWM1_ALT  5

// GPIOS


#endif // HWGPIOPINOUT_H_
