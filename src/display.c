/*
 * Display.c
 *
 *  Created on: Jan 30, 2025
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
#include  "timerz.h"

//Diplay Variables
char display_line[4][11]; //Array that assists in the display of text
char *display[4];
unsigned char display_mode;
volatile char display_changed; //This ENSURES the display shows what you want it to...
volatile char update_display;
volatile unsigned int update_display_count;

//Project 7 variables
extern unsigned int ADC_Left_Detect;
extern unsigned int ADC_Right_Detect;
extern unsigned int V_Thumb_Detect;
unsigned int BLACK;
unsigned int WHITE;
unsigned int GRAY;

//Project 9 variables
char disp[4][11] = {"", "", "", ""};
unsigned int word_length;

//Project 10 variables
extern unsigned char padnum; //Pad number the car has arrived @
unsigned int array[3];
extern unsigned int C_index; //Row indicating index, for Command Buffer
extern char CMD_Buf[5][15]; //Command Buffer
extern unsigned int Cali_Mode;
unsigned int yt_time; //
unsigned int blk_time;

void Display_Process(void){ //Keeping the Display Up to date with pertinent information
    if(update_display){
        update_display = 0;
        if(display_changed){
            display_changed = 0;
            Display_Update(0,0,0,0);
        }
    }
}

void IDLESCREEN115K(void){ //Initial screen presenting the baudrate and the need for detector calibration
    strcpy(display_line[0], "Calibrate ");
    strcpy(display_line[1], "Detectors!");
    strcpy(display_line[2], "BAUD RATE:");
    strcpy(display_line[3], " 115,200  ");
    display_changed = TRUE;
}

void Change_Display(void) { //This function displays anything dumped into the array 'disp' onto the screen in a centered alignment
    unsigned int i;
    for (i = 3; i < 4; i--) {
        char disp_word[11] = "          ";
        word_length = strlen(disp[i]);
        unsigned int spaces = (10 - word_length) >> 1;
        unsigned int j;
        for (j = 9; j < 10; j--) {
            if (j < spaces || j >= spaces + word_length) disp_word[j] = ' ';
            else disp_word[j] = disp[i][j - spaces];
        }
        strcpy(display_line[i], disp_word);
    }

    display_changed = TRUE;
}

void display_timer(char line, char location){ //This function displays a second timer to note how long the vehicle has been in use
    //-------------------------------------------------------------
    int i;
    for(i=0; i < 3; i++) {
        display_line[line][i+location] = array[i]+ 0x30;
        display_line[line][i+location+1] = 0;
        display_changed = TRUE;
    }
}

void Clear_Display(void){ //Eradicates everything from the display
    strcpy(disp[0], "");
    strcpy(disp[1], "");
    strcpy(disp[2], "");
    strcpy(disp[3], "");
    Change_Display();
}

void Arrived(padnum){ //Displays 'arrived at (Pad of Choice)'
    Clear_Display();
    strcpy(disp[0],"Arrived");
    strcpy(disp[1],"At");
    disp[2][0] = padnum;
    disp[2][1] = 0; //Have to append null character because I am not casting the padnum to the screen via strcpy(which automatically appends null, but rather hard pasting it.
    Change_Display();
}

void yt_catch(void){  //This function calibrates the 'white' values for the detectors
    ADCCTL0 |= ADCSC;
    while (ADCCTL0 & ADCBUSY);
    WHITE = ((ADC_Left_Detect + ADC_Right_Detect)>>1);
    HEXtoBCD(WHITE);
    adc_line(3,6);
    display_changed = TRUE;

}

void blk_catch(void){  //This function calibrates the 'black' values for the detectors
    ADCCTL0 |= ADCSC;
    while (ADCCTL0 & ADCBUSY);
    BLACK = ((ADC_Left_Detect + ADC_Right_Detect)>>1);
    HEXtoBCD(BLACK);
    adc_line(3,6);
    display_changed = TRUE;

    GRAY = ((BLACK + WHITE)>>1); //Gray automatically calculated and stored when black is calibrated


}

void Start_Menu(void){ //This function casts a calibration menu to the display that is governed by the thumbwheel
    TB1CCTL1 |= CCIE; //CCR1 Enable interrupt (Thumbwheel won't work without it)
    lcd_BIG_mid();
    if(0 < V_Thumb_Detect && V_Thumb_Detect < 511 ){
        strcpy(disp[0], "ADC Val:");
        strcpy(disp[1], "WHITE");
        strcpy(disp[2], "");
        yt_time = 1; //This links the white menu to calling the white catch function
        Change_Display();
    }
    else if (511 < V_Thumb_Detect && V_Thumb_Detect < 1023 ){
        strcpy(disp[0], "ADC Val:");
        strcpy(disp[1], "BLACK");
        strcpy(disp[2], "");
        blk_time = 1; //This links the black menu to calling the black catch function
        Change_Display();
    }
}
