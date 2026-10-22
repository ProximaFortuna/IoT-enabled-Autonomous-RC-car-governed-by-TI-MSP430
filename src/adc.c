/*
 * adc.c
 *This file contains the code associated with the Analog to Digital Conversion occurring via the Thumbwheel position
 *  Created on: Feb 26, 2025
 *      Author: Mopel Kitele
 */

#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "LCD.h"
#include  "ports.h"
#include  "macros.h"
#include  "led.h"
#include  "init.h"
#include  "display.h"
#include  "motion.h"
#include "timerz.h"

//Global Variables
unsigned int ADC_Channel; //Decides which channel the adc is detecting and converting input from (LFT DeTCT, RYT DeTCT, or THMBWHL)
unsigned int ADC_Left_Detect; //The reading from the left detector
unsigned int ADC_Right_Detect; //The reading from the right detector
unsigned int V_Thumb_Detect; //The reading from the thumbwheel
char adc_char [4]; //Array that assists in the display of adc readings
extern char display_line[4][11]; //Array that assists in the display of text
extern volatile char display_changed; //This ENSURES the display shows what you want it to...

void HEXtoBCD(int hex_value){
    //-----------------------------------------------------------------
    // Hex to BCD Conversion
    // Converts a Hex number to a BCD for display on the LCD
    //-----------------------------------------------------------------
    int value = 0;
    int i;
    for(i=0; i < 4; i++) {
        adc_char[i] = '0';
    }
    while (hex_value > 999){
        hex_value = hex_value - 1000;
        value = value + 1;
        adc_char[0] = 0x30 + value;
    }
    value = 0;
    while (hex_value > 99){
        hex_value = hex_value - 100;
        value = value + 1;
        adc_char[1] = 0x30 + value;
    }
    value = 0;
    while (hex_value > 9){
        hex_value = hex_value - 10;
        value = value + 1;
        adc_char[2] = 0x30 + value;
    }
    adc_char[3] = 0x30 + hex_value;
}

void adc_line(char line, char location){
    // ADC Line insert
    // Take the HEX to BCD value in the array adc_char and place it in the desired location on the desired line of the display.
    // char line => Specifies the line 1 thru 4
    // char location => Is the location 0 thru 9
    int i;
    unsigned int real_line;
    real_line = line - 1;
    for(i=0; i < 4; i++) {
        display_line[real_line][i+location] = adc_char[i];
        display_changed = TRUE;
    }
}

void Init_ADC(void){ //Initializes the analog - to - digital conversion of the black LED detector values on the underside of the chassis as well as the input sensed by the thumbwheel
    //------------------------------------------------------------------------------
    // V_DETECT_L (0x04) // Pin 2 A2
    // V_DETECT_R (0x08) // Pin 3 A3
    // V_THUMB (0x20) // Pin 5 A5
    //------------------------------------------------------------------------------
    // ADCCTL0 Register
    ADCCTL0 = 0; // Reset
    ADCCTL0 |= ADCSHT_2; // 16 ADC clocks
    ADCCTL0 |= ADCMSC; // MSC
    ADCCTL0 |= ADCON; // ADC ON
    // ADCCTL1 Register
    ADCCTL1 = 0; // Reset
    ADCCTL1 |= ADCSHS_0; // 00b = ADCSC bit
    ADCCTL1 |= ADCSHP; // ADC sample-and-hold SAMPCON signal from sampling timer.
    ADCCTL1 &= ~ADCISSH; // ADC invert signal sample-and-hold.
    ADCCTL1 |= ADCDIV_0; // ADC clock divider - 000b = Divide by 1
    ADCCTL1 |= ADCSSEL_0; // ADC clock MODCLK
    ADCCTL1 |= ADCCONSEQ_0; // ADC conversion sequence 00b = Single-channel single-conversion
    // ADCCTL1 & ADCBUSY identifies a conversion is in process
    // ADCCTL2 Register
    ADCCTL2 = 0; // Reset
    ADCCTL2 |= ADCPDIV0; // ADC pre-divider 00b = Pre-divide by 1
    ADCCTL2 |= ADCRES_2; // ADC resolution 10b = 12 bit (14 clock cycle conversion time)
    ADCCTL2 &= ~ADCDF; // ADC data read-back format 0b = Binary unsigned.
    ADCCTL2 &= ~ADCSR; // ADC sampling rate 0b = ADC buffer supports up to 200 ksps
    // ADCMCTL0 Register
    ADCMCTL0 |= ADCSREF_0; // VREF - 000b = {VR+ = AVCC and VR– = AVSS }
    ADCMCTL0 |= ADCINCH_5; // V_THUMB (0x20) Pin 5 A5
    ADCIE |= ADCIE0; // Enable ADC conv complete interrupt
    ADCCTL0 |= ADCENC; // ADC enable conversion.
    ADCCTL0 |= ADCSC; // ADC start conversion.
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void){
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG)){
    case ADCIV_NONE:
        break;
    case ADCIV_ADCOVIFG: // When a conversion result is written to the ADCMEM0
        // before its previous conversion result was read.
        break;
    case ADCIV_ADCTOVIFG: // ADC conversion-time overflow
        break;
    case ADCIV_ADCHIIFG: // Window comparator interrupt flags
        break;
    case ADCIV_ADCLOIFG: // Window comparator interrupt flag
        break;
    case ADCIV_ADCINIFG: // Window comparator interrupt flag
        break;
    case ADCIV_ADCIFG: // ADCMEM0 memory register with the conversion result
        ADCCTL0 &= ~ADCENC; // Disable ENC bit.
        switch (ADC_Channel++){
        case 0x00: // Channel A2 Interrupt
            ADC_Left_Detect = ADCMEM0; // Move result into Global Values
            ADC_Left_Detect = ADC_Left_Detect >> 2; // Divide result by 4
            ADCMCTL0 &= ~ADCINCH_2; // Disable Last channel A2
            ADCMCTL0 |= ADCINCH_3; // Enable Next channel A3
            ADCCTL0 |= ADCSC; // This line starts next sample
            break;
        case 0x01: // Channel A3 Interrupt
            ADC_Right_Detect = ADCMEM0; // Move result into Global Values
            ADC_Right_Detect = ADC_Right_Detect >> 2; //Divide result by 4
            ADCMCTL0 &= ~ADCINCH_3; // Disable Last channel A2
            ADCMCTL0 |= ADCINCH_5; // Enable Next channel A4
            ADCCTL0 |= ADCSC; // This line starts next sample
            break;
        case 0x02: //Channel A5 Interrupt
            V_Thumb_Detect = ADCMEM0; // Move result into Global Values
            V_Thumb_Detect = V_Thumb_Detect >> 2; // Divide Result By 4
            ADCMCTL0 &= ~ADCINCH_5; //Disable Last Channel A4
            ADCMCTL0 |= ADCINCH_2; // Enable NEXT channel 2
            ADC_Channel = 0; //Set Last Channel To Beginning
            break;
        default:break;
        }
        ADCCTL0 |= ADCENC; // Enable Conversions
        break;
        default:break;
    }
}
