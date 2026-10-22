/*
 * led.c
 *
 *  Created on: Jan 30, 2025
 *      Author: Mopel Kitele
 */
#include "ports.h"
#include  "msp430.h"


void Init_LEDs(void){
//------------------------------------------------------------------------------
// LED Configurations
//------------------------------------------------------------------------------
// Turns on both LEDs
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
//------------------------------------------------------------------------------
}

