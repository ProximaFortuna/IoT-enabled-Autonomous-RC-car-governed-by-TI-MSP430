//------------------------------------------------------------------------------
//Mopel Kitele's Playground (;.;)
//Created on Jan 2025
//This file contains the Main Routine - "While(1)" OS
//All Interrupts in this program have been init'd and setup in the SAME FILE!
//Built with Code Composer Version: CCS12.4.0.00007_win64
//------------------------------------------------------------------------------

#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "LCD.h"
#include  "ports.h"
#include  "macros.h"
#include  "led.h"
#include  "init.h"
#include "CerealPort.h"
#include  "display.h"
#include  "motion.h"
#include  "timerz.h"

//Project Initiating Variables
extern unsigned int Boot_Up; //Boot up timer ('volatile flag') that immediately starts off as true to begin connecting the car to the internet
extern unsigned int Cali_Mode; //flag 4 calibration mode
extern unsigned int BLTILIDIE; //Places the car in a frenzy to track and navigate Jim Carlson's Maze.
unsigned int DOIT; //Hypnotizing the car to locate and traverse a black line

void main(void){
    wheels_off();                              //Shut motorz off
    wheel_enable();                           //Motors are off... Commence system initialization.
    //  WDTCTL = WDTPW | WDTHOLD;            //Stop da watchdog timer
    //------------------------------------------------------------------------------
                                           //This is the main routine for the program.
                                          // Execution of code begins here.
                                         // OS is Back Ground Fore Ground.
    //------------------------------------------------------------------------------
    PM5CTL0 &= ~LOCKLPM5;              // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    Init_Ports();                     // Initialize Ports
    Init_Clocks();                   // Initialize Clock System
    Init_Conditions();              // Initialize Variables and Initial Conditions
    Init_Timers();                 // Initialize Timers
    Init_ADC();                   // Initialize ADC Converter
    Init_DAC();                  // Initialize Digital - to - Analog Converter
    Init_LCD();                 // Initialize LCD
    Init_Serial();             // Initialize Serial Portz

    IDLESCREEN115K();        //Startup Screen Portraying BaudRate
    //------------------------------------------------------------------------------
                            // Beginning of "While(1)" OS
    //------------------------------------------------------------------------------
    while(ALWAYS){ //Here lies the central functions necessary for the vehicle's performance.
        Display_Process(); //Update Display!
        if(Boot_Up)Startup_Commands(); //Commence internet initialization
        PING(); //Maintains consistent communication between the vehicle and any server it has connected to
        Cereal_Process(); //Enables serial communication between the vehicle and any other device with an internet connection and a TCP terminal
        Command_Parser(); //Robust recognition of multiple commands in succession
        magicsmoke(); //Prevents spontaneous combustion!
        if(Cali_Mode)Start_Menu(); //Splashes the start menu onto the display
        else {
            lcd_4line(); //Eradicates the start menu from the display
            TB1CCTL1 &= ~CCIE; //CCR1 disable interrupt
        }
        if(BLTILIDIE){ //Places the car in a frenzy to track and navigate Jim Carlson's Maze.
            DOIT = 1; // Oh it's gon do it alright...
            TB1CCTL1 |= CCIE; //CCR0 Enable interrupt

            switch(DOIT){ //Baby switch statement to truly lock the car in (this process can be thwarted anytime by '$BL EXIT')
            case 1:
                Black_Line(); //Black line navigation function
                break;
            default:break;
            }
        }
    }
}
