/*
 * switches.c
 *
 * This file contains the functions for operating the switches on the MSP430
 *  Created on: Feb 12, 2025
 *      Author: Mopel Kitele
 */

#include "functions.h"
#include <string.h>
#include "switches.h"
#include "msp430.h"
#include "macros.h"
#include "motion.h"
#include "ports.h"

//Project 10 Variables
unsigned int Cali_Mode; //flag 4 calibration mode
extern unsigned int yt_time; //flag 4 white calibr8
extern unsigned int blk_time; //flag 4 black calibr8

// Switch Interrupt Code
#pragma vector = PORT4_VECTOR
__interrupt void switchP4_interrupt(void){
    // Interrupt for Switch 1
    if (P4IFG & SW1) {
        //SWITCH REQUIREMENTS
        //------------------------------------------------------------------------------
        P4IFG &= ~SW1;                // IFG SW1 cleared
        P4IE &= ~SW1;                //Disable Switch Interrupt
        TB0CCTL1 |= CCIE;           //Turn on debounce timer for SW1 press
        //------------------------------------------------------------------------------
        //NEVER CHANGE!!
        if (yt_time){ //This flag is set to 1 in the white menu
            yt_catch(); yt_time = 0; //This links the white menu to calling the white catch function
        }
        else if(blk_time){ //This flag is set to 1 in the black menu
            blk_catch(); blk_time = 0; //This links the white menu to calling the white catch function
        }
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void switchP2_interrupt(void){
    // Interrupt for Switch 2
    if (P2IFG & SW2) {
        //SWITCH REQUIREMENTS
        //------------------------------------------------------------------------------
        P2IFG &= ~SW2;                // IFG SW2 cleared
        P2IE &= ~SW2;                //Disable Switch Interrupt
        TB0CCTL2 |= CCIE;           //Turn on debounce timer for SW2 press
        //------------------------------------------------------------------------------
        //NEVER CHANGE!!
        if (Cali_Mode == 0) Cali_Mode = 1; //Switch functionality to enable toggling between vehicle calibration mode
        else if (Cali_Mode == 1) Cali_Mode = 0; //Switch functionality to enable toggling between vehicle calibration mode
    }
}
