/*
 * DAC.c
 *
 *  Created on: Apr 22, 2025
 *      Author: Mopel Kitele
 */
#include "macros.h"
#include  "msp430.h"
#include "ports.h"

unsigned int DAC_data; //Sets speed state of car (3_5 volts is a 'slower' smoother procession, 5 volts is a 'spastic' quicker pace)
unsigned int DAC_target; //

void Init_DAC(void) {
    SAC3DAC = DACSREF_0; // Select VCC as DAC reference
    SAC3DAC |= DACLSEL_0; // DAC latch loads when DACDAT written

    SAC3OA  = NMUXEN; // SAC Negative input MUX control
    SAC3OA |= PMUXEN; // SAC Positive input MUX control
    SAC3OA |= PSEL_1; // 12-bit reference DAC source selected
    SAC3OA |= NSEL_1; // Select negative pin input
    SAC3OA |= OAPM; // Select low speed and low power mode
    SAC3PGA = MSEL_1; // Set OA as buffer mode
    SAC3OA |= SACEN; // Enable SAC
    SAC3OA |= OAEN; // Enable OA

    P3OUT  &= ~DAC_CTRL; //Set
    P3DIR  &= ~DAC_CTRL;
    P3SELC |= DAC_CTRL;

    DAC_data = DAC_BEGIN; // Starting Low value for DAC output [2v]
    DAC_target = DAC_5V; //Initialize the Digital - to - Analog Conversion board at 5 volts
    SAC3DAT = DAC_data;  // Initial DAC data

    TB0CTL |=  TBIE;    // Enable DAC Timer

    SAC3DAC |= DACEN;   // Enable DAC
}

