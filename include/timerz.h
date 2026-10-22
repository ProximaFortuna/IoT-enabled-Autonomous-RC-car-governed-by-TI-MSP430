/*
 * timers.h
 *
 *  Created on: Feb 17, 2025
 *      Author: Mopel Kitele
 */

#ifndef TIMERZ_H_
#define TIMERZ_H_

//Timer Interrupts
#define TB0CCR0_INTERVAL (25000) //50 milliseconds via SMCLK
#define TB0CCR1_INTERVAL (62500) //125 milliseconds via SMCLK
#define TB0CCR2_INTERVAL (62500) //125 milliseconds via SMCLK

#define TB1CCR0_INTERVAL (25000) //50 milliseconds via SMCLK
#define TB1CCR1_INTERVAL (25000) //50 millisecond via SMCLK
#define TB1CCR2_INTERVAL (65535) //1 second via SMCLK

#define TB2CCR0_INTERVAL (0) // ACLK

#define SW1DB_COUNTER        (6)
#define SW2DB_COUNTER        (6)


//PWM Macros
#define PWM_PERIOD (TB3CCR0)
#define LCD_BACKLITE_DIMING (TB3CCR1)

//More PWM Macros
#define SLOW (10000)
#define FAST (50000)
#define PERCENT_100 (50000)
#define PERCENT_80 (45000) //40000
#define PERCENT_20 (10000)

#endif /* TIMERZ_H_ */
