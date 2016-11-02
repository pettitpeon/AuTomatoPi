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
#ifndef BAGPIOPINOUT_H_
#define BAGPIOPINOUT_H_
/*------------------------------------------------------------------------------
    Defines
 -----------------------------------------------------------------------------*/

//Todo: naming convention

// One Wire Bus
#define BUS1W    04
#define W1TEMPFAM 28

// Serial Interface
#define SERTXD0  14
#define SERRXD0  15
#define SER_ALT   0

// I2C GPIOs
#define I2CSDA1  02
#define I2CSCL1  03
#define I2C_ALT   0

// Hardware PWM
#define PWM0     18
#define PWM0_ALT  5
#define PWM1     19 // Not sure
#define PWM1_ALT  5

#endif // BAGPIOPINOUT_H_
