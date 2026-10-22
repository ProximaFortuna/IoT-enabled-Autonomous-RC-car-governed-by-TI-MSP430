/*
 * Timerz.c
 *
 *  This file contains the instructions for the timer interrupts based on the 8mhz frequency of the micro-controller
 *  Created on: Feb 17, 2025
 *      Author: Mopel Kitele
 */

#include "timerz.h"
#include "functions.h"
#include "msp430.h"
#include  "motion.h"
#include "macros.h"
#include "CerealPort.h"
#include "ports.h"

//Variable Declarations
extern volatile unsigned char update_display;
unsigned int LYN_DETECT_TMR = 0;
unsigned int half_sec_clk;
unsigned int sw1_debounce;
unsigned int sw2_debounce;
unsigned int m = 0;
unsigned int displaytymmanipulator;
unsigned int secondpassed = 0;

//Project 7 Variables
unsigned int array[4];
extern char display_line[4][11]; //Array that assists in the display of text
extern volatile unsigned char display_changed;
extern unsigned int iteration;
unsigned int ADC_Left_Detect;
unsigned int ADC_Right_Detect;

//Project 9 Variables
extern unsigned int Command_Complete;
extern unsigned int Boot_Kick;
extern unsigned int Boot_Up;

//Project 10 Variables
extern unsigned int Hi;
extern unsigned int DAC_data;
extern unsigned int DAC_target;

void Init_Timers(void){
    init_tb0();
    init_tb1();
    init_tb2();
    init_tb3();
}

void init_tb0(void){

    TB0CTL = TBSSEL__SMCLK;           //SMCLK uses 8mhz, set this to 8mhz then manipulate it into seconds via division
    TB0CTL |= TBCLR;                 //This line clears the counter of TB0
    TB0CTL |= MC__CONTINUOUS;       //This line sets the (all?) clocks to continuous mode
    TB0CTL &= ~TBIE;               //This line makes certain that the interrupt doesn't occur in the capture/compare case of overflow
    TB0CTL &= ~TBIFG;             //The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!
    TB0CTL |= ID__2;             //This is the Main Clock Divider
    TB0EX0 |= TBIDEX__8;        //This is the Extra Clock Divider

    TB0CCR0 = TB0CCR0_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB0CCTL0 &= ~CCIFG;           //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)
    TB0CCTL0 |= CCIE;            //CCR0 Enable interrupt

    TB0CCR1 = TB0CCR1_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB0CCTL1 &= ~CCIFG;           //The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!
    TB0CCTL1 |= CCIE;            //CCR1 Enable interrupt

    TB0CCR2 = TB0CCR2_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB0CCTL2 &= ~CCIFG;           //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)
    TB0CCTL2 |= CCIE;            //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)

}

void init_tb1(void){                    //Potential Setup for when you wish to utilize TIMER B1

    TB1CTL = TBSSEL__SMCLK;           //SMCLK uses 8mhz, set this to 8mhz then manipulate it into seconds via division
    TB1CTL |= TBCLR;                 //This line clears the counter of TB0
    TB1CTL |= MC__CONTINUOUS;       //This line sets the (all?) clocks to continuous mode
    TB1CTL |= TBIE;                //This line makes certain that the interrupt doesn't occur in the capture/compare case of overflow
    TB1CTL &= ~TBIFG;             //The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!
    TB1CTL |= ID__2;             //This is the Main Clock Divider
    TB1EX0 |= TBIDEX__8;        //This is the Extra Clock Divider

    TB1CCR0 = TB0CCR0_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB1CCTL0 &= ~CCIFG;           //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)
    TB1CCTL0 |= CCIE;            //CCR0 Enable interrupt

    TB1CCR1 = TB0CCR1_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB1CCTL1 &= ~CCIFG;           //The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!
    TB1CCTL1 |= CCIE;            //CCR1 Enable interrupt

    TB1CCR2 = TB1CCR2_INTERVAL;     //This is the value that the clock will reach to trigger the interrupt
    TB1CCTL2 &= ~CCIFG;            //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)
    TB1CCTL2 |= CCIE;           //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)

}

void init_tb2(void){                   //This is the only timer that uses 32768 Hertz

    TB2CTL = TBSSEL__ACLK;            //ACLK uses 8mhz, set this to 8mhz then manipulate it into seconds via division
    TB2CTL |= TBCLR;                 //This line clears the counter of TB0
    TB2CTL |= MC__STOP;             //This line sets the m clock to stop mode
    TB2CTL &= ~TBIE;               //This line makes certain that the interrupt doesn't occur in the capture/compare case of overflow
    TB2CTL &= ~TBIFG;             //The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!
    TB2CTL |= ID__4;             //This is the Main Clock Divider
    TB2EX0 |= TBIDEX__8;        //This is the Extra Clock Divider

    TB2CCR0 = TB2CCR0_INTERVAL;    //This is the value that the clock will reach to trigger the interrupt
    TB2CCTL0 &= ~CCIFG;           //Clear possible pending interrupt (//The TRAP VECTOR linkage from the current instruction set to the one containing an interrupt DOESN'T HAPPEN!)
    TB2CCTL0 &= ~CCIE;            //CCR0 Enable interrupt
}

void init_tb3(void) { //PWM TIMER SETUP
    //------------------------------------------------------------------------------
    // SMCLK source, up count mode, PWM Right Side
    // TB3.1 P6.0 LCD_BACKLITE
    // TB3.2 P6.1 R_FORWARD
    // TB3.3 P6.2 R_REVERSE
    // TB3.4 P6.3 L_FORWARD
    // TB3.5 P6.4 L_REVERSE
    //------------------------------------------------------------------------------
    TB3CTL = TBSSEL__SMCLK; // SMCLK
    TB3CTL |= MC__UP; // Up Mode
    TB3CTL |= TBCLR; // Clear TAR
    PWM_PERIOD = WHEEL_PERIOD; // PWM Period [Set this to 50005]

    TB3CCTL1 = OUTMOD_7; // CCR1 reset/set
    LCD_BACKLITE_DIMING = PERCENT_20; // P6.0 Right Forward PWM duty cycle

    TB3CCTL2 = OUTMOD_7; // CCR2 reset/set
    RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM duty cycle

    TB3CCTL3 = OUTMOD_7; // CCR3 reset/set
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM duty cycle

    TB3CCTL4 = OUTMOD_7; // CCR4 reset/set
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM duty cycle

    TB3CCTL5 = OUTMOD_7; // CCR5 reset/set
    LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM duty cycle
    //------------------------------------------------------------------------------
}

#pragma vector = TIMER0_B0_VECTOR     // Timer B0, but only including code for the Capture Compare register 0
__interrupt void Timer0_B0_ISR(void){
    if (displaytymmanipulator++ >= 4){
        update_display = 1;
        displaytymmanipulator = 0;
    }
    if(Boot_Up && Boot_Up < 150) { //Boot the vehicle up. Slowly
        Boot_Kick = TRUE;
        Boot_Up++;
    }
    else Boot_Up = RESET;
    TB0CCR0 += TB0CCR0_INTERVAL; // Add Offset to TBCCR0
    //----------------------------------------------------------------------------
}

#pragma vector = TIMER0_B1_VECTOR    //Timer B0 still, but here lies the code for Capture Compare register 1 and Capture Compare register 2
__interrupt void TIMER0_B1_ISR(void){
    //----------------------------------------------------------------------------
    // TimerB0 1-2, Overflow Interrupt Vector (TBIV) handler
    //----------------------------------------------------------------------------
    switch(__even_in_range(TB0IV,14)){
    case 0:
        break; // No interrupt
    case 2: // CCR1
        if(++sw1_debounce >= SW1DB_COUNTER){
            TB0CCTL1 &= ~CCIE;            //CCR2 Disable interrupt because it has already triggered (think of this as a debounce)
            P4IE |= SW1;                 //Call SW1 Interrupt
            P4IFG &= ~SW1;              //Clear Switch Interrupt Flag
            sw1_debounce = RESET_STATE;      //Reset SW1 De-bounce
        }
        TB0CCR1 += TB0CCR1_INTERVAL;    // Add Offset to TBCCR1
        break;

    case 4: // CCR2
        if(++sw2_debounce >= SW2DB_COUNTER){
            P2IFG &= ~SW2;           //Clear Switch Interrupt Flag
            P2IE |= SW2;                    //Call SW2 Interrupt
            TB0CCTL2 &= ~CCIE;               //CCR2 Disable interrupt because it has already triggered (think of this as a debounce)
            sw2_debounce = RESET_STATE;      //Reset SW1 De-bounce
        }
        TB0CCR2 += TB0CCR2_INTERVAL;   // Add Offset to TBCCR2
        break;

    case 14: //Digital - to - analog speed moderator interrupt algorithm
        if (SAC3DAC & DACEN){
            if(DAC_data < DAC_target){
                DAC_data = DAC_data + 100;
            }

            if(DAC_data > DAC_target){
                DAC_data = DAC_data - 100;
                if (DAC_data <= DAC_target){
                    DAC_data = DAC_target;
                }
            }
            if(DAC_data <= 800){
                DAC_data = 800;
            }
            SAC3DAT = DAC_data;
        }
        break;

    default:break;

    //----------------------------------------------------------------------------
    }
}

#pragma vector = TIMER1_B0_VECTOR     // Timer B1, but only including code for the Capture Compare register 0
__interrupt void Timer1_B0_ISR(void){
    switch (m++){
    case 1: //lo - to - hi initialization protocol of IOT
        P3OUT &= ~IOT_EN_CPU;
        break;
    case 3:
        P3OUT |= IOT_EN_CPU;
        TB1CCR0 += TB1CCR0_INTERVAL; // Add Offset to TBCCR0
        break;
    }
}

#pragma vector = TIMER1_B1_VECTOR    //Timer B0 still, but here lies the code for Capture Compare register 1 and Capture Compare register 2
__interrupt void TIMER1_B1_ISR(void){
    //----------------------------------------------------------------------------
    // TimerB0 1-2, Overflow Interrupt Vector (TBIV) handler
    //----------------------------------------------------------------------------
    switch(__even_in_range(TB1IV,14)){
    case 0:
        break; // No interrupt
    case 2: // CCR1
        ADCCTL0 |= ADCSC; // This line starts next sample
        TB1CCR1 += TB1CCR1_INTERVAL;    // Add Offset to TBCCR1
        break;
    case 4: // CCR2
        if (++secondpassed == 8){ //truly making lynDetect increment once per second
            LYN_DETECT_TMR++;
            secondpassed = 0;
        }
        Hi++;
        if(++array[3] > 9){
            if(++array[2] > 9){
                if(++array[1] > 9){
                    if(++array[0] > 9){
                        array[0] = 0;
                    }
                    array[1] = 0;
                }
                array[2] = 0;
            }
            array[3] = 0;
        }
        TB1CCR2 += TB1CCR2_INTERVAL;    // Add Offset to TBCCR1
        break;
    default:break;
    }
    //----------------------------------------------------------------------------
}

#pragma vector = TIMER2_B0_VECTOR     // Timer B2, but only including code for the Capture Compare register 0
__interrupt void Timer2_B0_ISR(void){
    TB2CCTL0 &= ~CCIE;            //CCR0 Disable interrupt
    TB2CCR0 = 0;
    TB2CTL &= ~MC; //Prevent clock from ticking...
    TB2R = 0; //Reset clock value to 0
    Command_Complete = TRUE; ////Command interpretation complete, allow new command
    wheels_off();
    //----------------------------------------------------------------------------
}
