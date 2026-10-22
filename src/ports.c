/*
 * ports.c
 *
 *  Created on: Jan 30, 2025
 *      Author: Mopel Kitele
 */

#include  "msp430.h"
#include "ports.h"
#include "functions.h"
#include "macros.h"
//------------------------------------------------------------------------------

void Init_Ports(void){
    Init_Port1();
    Init_Port2();
    Init_Port3();
    Init_Port4();
    Init_Port5();
    Init_Port6();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port1(void){ //Configuration of Port 1
    //P1 initialization
    P1OUT = 0x00;
    P1DIR = 0x00;

    //1.0, GPIO, low, output
    P1SEL0 &= ~RED_LED;
    P1SEL1 &= ~RED_LED;
    P1OUT &= ~RED_LED;
    P1DIR |= RED_LED;

    //1.1, F
    P1SEL0 |= A1_SEEED;
    P1SEL1 |= A1_SEEED;

    //1.2, F
    P1SEL0 |= V_DETECT_L;
    P1SEL1 |= V_DETECT_L;

    //1.3, F
    P1SEL0 |= V_DETECT_R;
    P1SEL1 |= V_DETECT_R;

    //1.4, F
    P1SEL0 |= A4_SEEED;
    P1SEL1 |= A4_SEEED;

    //1.5, F
    P1SEL0 |= V_THUMB;
    P1SEL1 |= V_THUMB;

    //1.6, F
    P1SEL0 |= UCA0RXD;
    P1SEL1 &= ~UCA0RXD;

    //1.7, F
    P1SEL0 |= UCA0TXD;
    P1SEL1 &= ~UCA0TXD;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port2(void){ // Configure Port 2
    P2OUT = 0x00; // P2 set Low
    P2DIR = 0x00; // Set P2 direction to input

    P2SEL0 &= ~SLOW_CLK; // SLOW_CLK GPIO operation
    P2SEL1 &= ~SLOW_CLK; // SLOW_CLK GPIO operation
    P2OUT &= ~SLOW_CLK; // Initial Value = Low / Off
    P2DIR |= SLOW_CLK; // Direction = output

    P2SEL0 &= ~CHECK_BAT; // CHECK_BAT GPIO operation
    P2SEL1 &= ~CHECK_BAT; // CHECK_BAT GPIO operation
    P2OUT &= ~CHECK_BAT; // Initial Value = Low / Off
    P2DIR |= CHECK_BAT; // Direction = output

    P2SEL0 &= ~IR_LED; // P2_2 GPIO operation
    P2SEL1 &= ~IR_LED; // P2_2 GPIO operation
    P2OUT |= IR_LED; // Initial Value = lo
    P2DIR |= IR_LED; // Direction = output

    P2SEL0 &= ~SW2;       // SW2 is GPIO operation
    P2SEL1 &= ~SW2;      // SW2 is GPIO operation
    P2OUT |= SW2;       // Configure pullup resistor
    P2DIR &= ~SW2;     // Direction = input
    P2REN |= SW2;    // Enable pull-up resistor SW2
    P2IES |= SW2;   // SW2 Hi/Lo edge interrupt
    P2IFG &= ~SW2; // IFG SW1 cleared
    P2IE |= SW2;  // SW2 interrupt Enabled

    P2SEL0 &= ~IOT_RUN_RED; // IOT_RUN_CPU GPIO operation
    P2SEL1 &= ~IOT_RUN_RED; // IOT_RUN_CPU GPIO operation
    P2OUT &= ~IOT_RUN_RED; // Initial Value = Low / Off
    P2DIR |= IOT_RUN_RED; // Direction = output

    P2SEL0 &= ~DAC_ENB;  // DAC_ENB GPIO operation
    P2SEL1 &= ~DAC_ENB; // DAC_ENB GPIO operation
    P2OUT |= DAC_ENB;  // Initial Value = Hi
    P2DIR |= DAC_ENB; // Direction = output

    P2SEL0 &= ~LFXOUT; // LFXOUT Clock operation
    P2SEL1 |= LFXOUT; // LFXOUT Clock operation

    P2SEL0 &= ~LFXIN; // LFXIN Clock operation
    P2SEL1 |= LFXIN; // LFXIN Clock operation
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port3(void){ //Configuration of Port 3
    //P3 initialization
    P3OUT = 0x00;
    P3DIR = 0x00;

    //3.0, GPIO, low, input
    P3SEL0 &= ~TEST_PROBE;
    P3SEL1 &= ~TEST_PROBE;
    P3OUT &= ~TEST_PROBE;
    P3DIR &= ~TEST_PROBE;

    //3.1, f
    P3SEL0 &= ~OA2O;
    P3SEL1 &= ~OA2O;
    P3DIR &= ~OA2O;

    //3.2, f
    P3SEL0 &= ~OA2N;
    P3SEL1 &= ~OA2N;
    P3DIR &= ~OA2N;

    //3.3, f
    P3SEL0 &= ~OA2P;
    P3SEL1 &= ~OA2P;
    P3DIR &= ~OA2P;

    //3.4, f, output
    P3SEL0 |= SMCLK_OUT;
    P3SEL1 &= SMCLK_OUT;
    P3DIR |= SMCLK_OUT;

    //3.5, GPIO, low, input
    P3SEL0 &= ~DAC_CTRL;
    P3SEL1 &= ~DAC_CTRL;
    P3OUT &= ~DAC_CTRL;
    P3DIR &= ~DAC_CTRL;

    //3.6, GPIO, low, input
    P3SEL0 &= ~IOT_LINK_CPU;
    P3SEL1 &= ~IOT_LINK_CPU;
    P3OUT &= ~IOT_LINK_CPU;
    P3DIR &= ~IOT_LINK_CPU;

    //3.7, GPIO, low, input
    P3SEL0 &= ~IOT_EN_CPU;
    P3SEL1 &= ~IOT_EN_CPU;
    P3OUT &= ~IOT_EN_CPU;
    P3DIR |= IOT_EN_CPU;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port4(void){ // Configure PORT 4
    P4OUT = 0x00; // P4 set Low
    P4DIR = 0x00; // Set P4 direction to output

    P4SEL0 &= ~RESET_LCD; // RESET_LCD GPIO operation
    P4SEL1 &= ~RESET_LCD; // RESET_LCD GPIO operation
    P4OUT &= ~RESET_LCD; // Initial Value = Low / Off
    P4DIR |= RESET_LCD; // Direction = output

    P4SEL0 &= ~SW1;       // SW1 GPIO operation
    P4SEL1 &= ~SW1;      // SW1 GPIO operation
    P4OUT |= SW1;       // Configure pullup resistor
    P4DIR &= ~SW1;     // Direction = input
    P4REN |= SW1;    // Enable pull-up resistor SW1
    P4IES |= SW1;   // SW1 Hi/Lo edge interrupt
    P4IFG &= ~SW1; // IFG SW1 cleared
    P4IE |= SW1;  // SW1 interrupt Enabled

    P4SEL0 |= UCA1RXD;   // USCI_A1 UART operation
    P4SEL1 &= ~UCA1RXD; // USCI_A1 UART operation

    P4SEL0 |= UCA1TXD;   // USCI_A1 UART operation
    P4SEL1 &= ~UCA1TXD; // USCI_A1 UART operation

    P4SEL0 &= ~UCB1_CS_LCD; // UCB1_CS_LCD GPIO operation
    P4SEL1 &= ~UCB1_CS_LCD; // UCB1_CS_LCD GPIO operation
    P4OUT |= UCB1_CS_LCD; // Set SPI_CS_LCD Off [High]
    P4DIR |= UCB1_CS_LCD; // Set SPI_CS_LCD direction to output

    P4SEL0 |= UCB1CLK; // UCB1CLK SPI BUS operation
    P4SEL1 &= ~UCB1CLK; // UCB1CLK SPI BUS operation

    P4SEL0 |= UCB1SIMO; // UCB1SIMO SPI BUS operation
    P4SEL1 &= ~UCB1SIMO; // UCB1SIMO SPI BUS operation

    P4SEL0 |= UCB1SOMI; // UCB1SOMI SPI BUS operation
    P4SEL1 &= ~UCB1SOMI; // UCB1SOMI SPI BUS operation
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port5(void){ //Configuration of Port 5
    //P5 initialization
    P5OUT = 0x00;
    P5DIR = 0x00;

    //5.0, GPIO, low, input
    P5SEL0 &= ~V_BAT;
    P5SEL1 &= ~V_BAT;
    P5OUT &= ~V_BAT;
    P5DIR &= ~V_BAT;

    //5.1, GPIO, low, input
    P5SEL0 &= ~V_5;
    P5SEL1 &= ~V_5;
    P5OUT &= ~V_5;
    P5DIR &= ~V_5;

    //5.2, GPIO, low, output
    P5SEL0 &= ~CV_DAC;
    P5SEL1 &= ~CV_DAC;
    P5OUT &= ~CV_DAC;
    P5DIR &= ~CV_DAC;

    //5.3, GPIO, low, output
    P5SEL0 &= ~V3_3;
    P5SEL1 &= ~V3_3;
    P5OUT &= ~V3_3;
    P5DIR &= ~V3_3;

    //5.4, GPIO, low, output
    P5SEL0 &= ~IOT_BOOT_CPU;
    P5SEL1 &= ~IOT_BOOT_CPU;
    P5OUT |= IOT_BOOT_CPU;
    P5DIR |= IOT_BOOT_CPU;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void Init_Port6(void){ //Configuration of Port 6
    //P6 initialization
    P6OUT = 0x00;
    P6DIR = 0x00;

    //6.0, GPIO, low, input
    P6SEL0 |= LCD_BACKLITE;
    P6SEL1 &= ~LCD_BACKLITE;
    //P6OUT |= LCD_BACKLITE;
    P6DIR |= LCD_BACKLITE;

    //6.1, GPIO, low, input
    P6SEL0 |= R_FORWARD;
    P6SEL1 &= ~R_FORWARD;
    // P6OUT &= ~R_FORWARD;
    P6DIR |= R_FORWARD;

    //6.2, GPIO, low, input
    P6SEL0 |= R_REVERSE;
    P6SEL1 &= ~R_REVERSE;
    //P6OUT &= ~R_REVERSE;
    P6DIR |= R_REVERSE;

    //6.3, GPIO, low, input
    P6SEL0 |= L_FORWARD;
    P6SEL1 &= ~L_FORWARD;
    // P6OUT &= ~L_FORWARD;
    P6DIR |= L_FORWARD;

    //6.4, GPIO, low, input
    P6SEL0 |= L_REVERSE;
    P6SEL1 &= ~L_REVERSE;
    // P6OUT &= ~L_REVERSE;
    P6DIR |= L_REVERSE;

    //6.5, GPIO, low, output
    P6SEL0 &= ~P6_5;
    P6SEL1 &= ~P6_5;
    P6OUT &= ~P6_5;
    P6DIR &= ~P6_5;

    //6.6, GPIO, low, output
    P6SEL0 &= ~GRN_LED;
    P6SEL1 &= ~GRN_LED;
    P6OUT &= ~GRN_LED;
    P6DIR |= GRN_LED;
}
//------------------------------------------------------------------------------
