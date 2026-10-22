/*
 * motion.c
 *
 * This file contains the functions associated with movement of the car.
 *  Created on: Feb 5, 2025
 *      Author: Mopel Kitele
 */

#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "LCD.h"
#include  "ports.h"
#include  "macros.h"
#include  "led.h"
#include  "CerealPort.h"
#include  "init.h"
#include  "display.h"
#include  "motion.h"
#include  "timerz.h"

//Display Variables
extern char display_line[4][11]; //Array that assists in the display of text
extern char *display[4];
extern volatile unsigned char display_changed;

//Timer Variables
extern unsigned int LYN_DETECT_TMR;

//Project 7 variables
unsigned int End_Step = 0;
unsigned int ADC_Channel;
unsigned int ADC_Left_Detect;
unsigned int ADC_Right_Detect;
unsigned int V_Thumb_Detect;

unsigned int BL_Step = 0;
extern unsigned int GRAY;
extern unsigned int BLACK;
extern unsigned int WHITE;

//Project 10 Variables
extern char CMD_Buf[5][15]; //Command Buffer
extern char disp[4][11];
extern unsigned int C_index; //Row indicating index, for Command Buffer
unsigned int ONDAYT;
extern unsigned int XIT;
extern unsigned int DAC_target;
unsigned int last_seen_side;
unsigned int left;
unsigned int right;
unsigned int none;
unsigned int white_counter;

void wheel_enable(void){ //Enable ALL wheels to accept output
    P6DIR |= R_REVERSE;                 // Enable Right Wheel to accept Reverse Output
    P6DIR |= L_REVERSE;                // Enable Left Wheel to accept Reverse Output
    P6DIR |= R_FORWARD;               // Enable Right Wheel to accept Forward Output
    P6DIR |= L_FORWARD;              // Enable Left Wheel to accept Forward Output
}

void wheels_disable(void){ //Disable ALL wheels from accepting any types of Output
    P6DIR &= ~R_REVERSE;           // Disable Right Wheel from accepting Reverse Output
    P6DIR &= ~L_REVERSE;          // Disable Left Wheel from accepting Reverse Output
    P6DIR &= ~R_FORWARD;         // Disable Right Wheel from accepting Forward Output
    P6DIR &= ~L_FORWARD;        // Disable Left Wheel from accepting Forward Output
}

void wheels_off(void){ // Turn wheels completely off
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    LEFT_FORWARD_SPEED = WHEEL_OFF;
}

void forward(unsigned int rightspeed, unsigned int leftspeed){ //Forward function that allows you to move the car forward at any speed value you please
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    RIGHT_FORWARD_SPEED = rightspeed;
    LEFT_FORWARD_SPEED = leftspeed;
}

void backward(unsigned int rightspeed, unsigned int leftspeed){ //Backward function that allows you to move the car backward at any speed value you please
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = rightspeed;
    LEFT_REVERSE_SPEED = leftspeed;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    LEFT_FORWARD_SPEED = WHEEL_OFF;
}

void FrontSide180(unsigned int speed){ //Function that allows you to rotate the vehicle frontside at any speed value you please
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = speed;
    RIGHT_FORWARD_SPEED = speed;
    LEFT_FORWARD_SPEED = WHEEL_OFF;

}

void BackSide180(unsigned int speed){ //Function that allows you to rotate the vehicle backside at any speed value you please
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = speed;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    LEFT_FORWARD_SPEED = speed;
}

void RightRev(unsigned int speed){  // Reverse Car Right
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = speed;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    LEFT_FORWARD_SPEED = WHEEL_OFF;
}

void LeftRev(unsigned int speed){ // Reverse Car Left
    DAC_target = DAC_3_5;
    RIGHT_REVERSE_SPEED = speed;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    LEFT_FORWARD_SPEED = WHEEL_OFF;
}

void end_case(void){ //End case that terminates the frenzy of *insert thunderous reverb* BLACK LINEEEE NAVIGATION!
    TB1CCTL1 &= ~CCIE;
    switch (End_Step){
    case 0:
        wheels_off();
        strcpy(display_line[0], "BL Exit:  ");
        strcpy(display_line[3], "   ('_')  ");
        strcpy(display_line[2], "    ('-') ");
        strcpy(display_line[2], "     ('.')");
        display_changed = TRUE;
        LYN_DETECT_TMR = 0;
        break;
    default:break;
    }
}

void traversal(void){
    if (ADC_Right_Detect > GRAY && ADC_Left_Detect > GRAY) forward(20000, 30000);
    else if (ADC_Right_Detect <= GRAY && ADC_Left_Detect > GRAY) FrontSide180(25000);
    else if (ADC_Right_Detect > GRAY && ADC_Left_Detect <= GRAY) BackSide180(25000);
    else {
        if (ADC_Right_Detect > ADC_Left_Detect) backward(20000, 0);
        else backward(0, 20000);
    }
}
        void Black_Line(void){
            switch (BL_Step){
            case 0: //Display that the car has been hypnotized...
                strcpy(disp[0], "BL Start:");
                strcpy(disp[1], "");
                strcpy(disp[2], "");
                strcpy(disp[3], "");
                Change_Display();
                if (LYN_DETECT_TMR >= 5){
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 1;
                }
                break;
            case 1: //Move in an arcing manner to 'snipe' line... (based on timer)
                forward(23500,19300);
                strcpy(disp[0], "BL Turn:");
                Change_Display();
                if (LYN_DETECT_TMR >= 8){
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 2;
                }
                break;

            case 2:
                forward(18000,20000);
                strcpy(disp[0], "BL Boost:"); //Sneakily boost forward to ACTUALLY snipe a suspiciously dark value
                Change_Display();
                if(ADC_Right_Detect >= 800 || ADC_Left_Detect >= 800){
                    wheels_off();
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 3;
                }
                break;

            case 3: // Stop car because its intercepted line, display intercept
                strcpy(disp[0], "Intercept");
                strcpy(disp[1], "");
                strcpy(disp[2], "");
                strcpy(disp[3], "");
                Change_Display();

                if (LYN_DETECT_TMR >= 6){
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 4;
                }
                break;

            case 4:// Align car onto the line by calling pivot function, display aligning
                strcpy(disp[0], "BL Align");
                strcpy(disp[1], "");
                strcpy(disp[2], "");
                strcpy(disp[3], "");
                Change_Display();

                if(ADC_Right_Detect < 800 || ADC_Left_Detect < 800 ){
                    BackSide180(23000);
                }
                else{
                    wheels_off();
                    BL_Step = 5;
                    LYN_DETECT_TMR = RESET;
                }
                break;

            case 5: // begin moving forward and traverse line
                strcpy(disp[0], "BL Travel");
                strcpy(disp[1], "");
                strcpy(disp[2], "");
                strcpy(disp[3], "");
                Change_Display();
                traversal();
                if (LYN_DETECT_TMR == 30) {
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 6;
                }
                break;

            case 6: //stop, wait, and display that the car has entered the circle
                strcpy(disp[0], "BL Circle");
                strcpy(disp[1], "");
                strcpy(disp[2], "");
                strcpy(disp[3], "");
                Change_Display();
                wheels_off();
                if (LYN_DETECT_TMR == 6 ) {
                    LYN_DETECT_TMR = RESET;
                    BL_Step = 7;
                }
                break;

            case 7: //Resume traversal of circle for eternity
                traversal();
                if (XIT == TRUE) BL_Step = 8;
                break;

            case 8:
                end_case(); //Free the car from the shackles of mind control
                break;

            default: break;
            }
        }

        void magicsmoke(void){
            if ((P6IN & L_FORWARD) && (P6IN & L_REVERSE)){
                P6OUT &= ~L_FORWARD;                  //Make Left Wheel Rotate Forward Pin Lo
                P6OUT &= ~L_REVERSE;                 //Make Left Wheel Rotate Reverse Pin Lo
                P1OUT &= ~RED_LED;

            }

            if ((P6IN & R_FORWARD) && (P6IN & R_REVERSE)){
                P6OUT &= ~R_FORWARD;                  //Make Left Wheel Rotate Forward Pin Lo
                P6OUT &= ~R_REVERSE;                 //Make Left Wheel Rotate Reverse Pin Lo
                P1OUT &= ~RED_LED;
            }
        }
