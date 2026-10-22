/*
 * CMD.c
 *
 *  Created on: Apr 9, 2025
 *      Author: Mopel Kitele
 */
#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "macros.h"
#include "CerealPort.h"
#include "timerz.h"

//Global Variables
char Protocol0[16] = "AT+SYSSTORE=0\r\n"; //Send command to IoT to save configuration settings
char Protocol1[14] = "AT+CIPMUX=1\r\n"; //Send command to IoT to enable multiple devices to connect to the vehicle
char Protocol2[22] = "AT+CIPSERVER=1,5324\r\n"; //Send command to IoT to configure port number for vehicle UART connection
char SSID_Catcher[12] = "AT+CWJAP?\r\n"; //Send command to IoT to capture current SSID
char IP_Catcher[11] = "AT+CIFSR\r\n"; //Send command to IoT to capture current IP
char R_U_THERE[5] = "AT\r\n"; //Send Ping to IoT
char Password[5] = "Zoro"; //Password so nobody hijacks my vehicle
unsigned int Hi; //Ping timer
unsigned int itstym; //Ping Flag that waits for confirmed internet connection before it sets itself to true
char padnum; //Pad number the car has arrived @
unsigned int Boot_Up = TRUE; //Boot up timer that immediately starts off as true to begin connecting the car to the internet
unsigned int Boot_Kick; //This gets set to 0 the first time the Startup_commands() function gets called, ascertaining that the IoT boots itself up once and only once
unsigned int Command_Complete = TRUE; //After a command has been executed in its entirety, this will indicate that.
extern unsigned int Cfront; //Front pointer for Command Queue
extern unsigned int Cback; //Back pointer for Command Queue
char CMD_Buf[5][15]; //Command Buffer
extern unsigned int P_index; //Row indicating index, for Passthrough Buffer
extern unsigned int C_index; //Row indicating index, for Command Buffer
extern unsigned int B_index; //Row indicating index, for Process Buffer
extern char Pass_Buf[10][80]; //Pass-through Buffer
extern char Process_Buf[10][80]; //Process Buffer
extern unsigned int serial_flags; //CAP'N CRUNCH PRESENTS
extern char disp[4][11]; //Display Arrays
extern volatile char display_changed; //Set this to true to update the display
char display_line[4][11]; //Array that assists in the display of text
unsigned int BLTILIDIE = 0; //Places the car in a frenzy to track and navigate Jim Carlson's Maze.
unsigned int XIT; //Alleviates the car from said frenzy...
extern unsigned int LYN_DETECT_TMR; //This propagates the procession between stages of the state machine dedicated to traversing any black line it comes into contact with

void Command_Parser(void){ //Password to ANY command is $Zoro , ALL commands must be appended with a ';'
    unsigned int i; //who doesn't love a good 'i' to iterate a for loop ;)
    if(Cfront == Cback ) return; //If the command queue is clear, do not waste time running this function... we love saving electrons ;)

    if(CMD_Buf[Cfront][4] == 'S'){
        Cfront = Cback; //Empty command queue
        Command_Complete = TRUE; //Command interpretation complete, allow new command
        TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
        TB2CTL &= ~MC; //Prevent clock from ticking...
        TB2R = RESET; //reset clock value to 0
        wheels_off();
        return;
    }

    if(Command_Complete == TRUE){
        itstym = TRUE; //Now that the car is recieving commands from a TCP terminal, it will NEVER lose connection to the terminal
        Command_Complete = RESET; //Allow current command to run
        char * Curr_command; //Pointer to the currently received command
        Curr_command = (CMD_Buf[CMD_dequeue()]); //Dump the command into the queue
        strcpy(display_line[3], Curr_command + 4); //Splash the currently executing command on the display
        strcat(display_line[3], " ");
        display_changed = TRUE;
        for(i = 0;i < 4;i++){
            if(Curr_command[i] != Password[i]) { //Disregard current command if password is incorrect
                Command_Complete = TRUE; //Command interpretation complete, allow new command
                return;
            }
        }

        unsigned int time; //variable that allows commands to be passed with a precise time duration
        time = (Curr_command[8]-48)*1;
        time += (Curr_command[7]-48)*10;
        time += (Curr_command[6]- 48)*100;
        time += (Curr_command[5]-48)*1000;

        TB2CCR0 = time; //links internal interrupt timer with specified command runtime

        switch (Curr_command[4]){ //Based on what letter lies in the fifth index, the car will behave a c
        case 'f':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            forward(19000,30050);
            break;

        case 'E':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            Clear_Display();
            strcpy(disp[0], "BL Exit:");
            strcpy(disp[1], "");
            strcpy(disp[2], "");
            strcpy(disp[3], "");
            Change_Display();
            BLTILIDIE = RESET;
            XIT = TRUE;
            break;

        case 'o':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            LeftRev(30050);
            break;

        case 'p':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            RightRev(30050);
            break;

        case 'b':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            backward(19000,23550);
            break;
        case 'r':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            FrontSide180(20000);
            break;

        case 'l':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            BackSide180(20000);
            break;

        case 's':
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL |= MC__CONTINUOUS; //allow clock to tick
            wheels_off();
            break;

        case 'a':
            Command_Complete = TRUE; //Command interpretation complete, allow new command
            padnum = Curr_command[5];
            Arrived(padnum);
            break;

        case 'S':
            Command_Complete = TRUE; //Command interpretation complete, allow new command
            TB2CCTL0 |= CCIE; //CCR0 Enable interrupt
            TB2CTL &= ~MC; //Prevent clock from ticking...
            TB2R = RESET; //Reset clock value to 0
            wheels_off();
            break;

        case 'F':
            Command_Complete = TRUE; //Command interpretation complete, allow new command
            forward(19000,30050);
            break;

        case 'T':
            Command_Complete = TRUE; //Command interpretation complete, allow new command
            BLTILIDIE = TRUE; //Places the car in a frenzy to track and navigate Jim Carlson's Maze.
            LYN_DETECT_TMR = RESET; //Primes the state machine associated with this navigation
            break;

        case 'B':
            Command_Complete = TRUE; //Command interpretation complete, allow new command
            backward(19000,30050);
            break;
        default:break;
        }
    }
}

void Startup_Commands(void){
    if (Boot_Kick == RESET) return; //If the startup has already occurred do not waste time running this function... we love saving electrons ;)
    Boot_Kick = RESET;
    switch (Boot_Up){

    case 60:
        strcpy(Pass_Buf[P_index],Protocol0);
        Pass_enqueue(0); //enqueue the protocol text
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        break;

    case 70:
        strcpy(Pass_Buf[P_index],Protocol1);
        Pass_enqueue(0); //enqueue the protocol text
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        break;

    case 80:
        strcpy(Pass_Buf[P_index],Protocol2);
        Pass_enqueue(0); //enqueue the protocol text
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        break;

    case 100:
        strcpy(Pass_Buf[P_index],SSID_Catcher);
        Pass_enqueue(0); //enqueue the SSID catcher
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        break;

    case 150:
        strcpy(Pass_Buf[P_index],IP_Catcher);
        Pass_enqueue(0); //enqueue the IP catcher
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        break;
    default:break;
    }
}

void PING(void){
    if(Boot_Up == 0 && itstym){
        strcpy(Pass_Buf[P_index],R_U_THERE);
        Pass_enqueue(0); ////enqueue the ping command
        if (++P_index >= 10) P_index = BEGINNING; //RING BUFFFF EFFECCC
        serial_flags |= UCA0TX;
        Hi = 0;
        display_timer(3,6);
    }
}

void SSID_Display(void){
    char *SSID;
    if (strncmp(Process_Buf[B_index], "+CWJAP:", 7) == 0) {
        char buf_copy[64];
        strcpy(buf_copy, Process_Buf[B_index]);
        strtok(buf_copy, "\""); // Skip to first quote
        SSID = strtok(NULL, "\""); // Get SSID
        if (SSID != NULL) {
            strcpy(disp[0], SSID); // Display SSID that vehicle has connected itself to
            Change_Display();
        }
    }
}

void IP_Display(void){ //Masterful manipulation of the strtok function to capture and display ANY IP address the vehicle has connected to, ensuring easy configuration no matter the location
    char *IP;
    char *IP1;
    char *IP2;
    char *IP3;
    char *IP4;
    if (strncmp(Process_Buf[B_index], "+CIFSR:STAIP,", 13) == 0) { //Seek out the particular line in the responses stored in the 'history' of my beautiful proccess buffer that contains the IP address. (It is a known fact that "+CIFSR:STAIP," precedes the IP)
        char buf_copy[64];
        strcpy(buf_copy, Process_Buf[B_index]); //Dump that line into this buf_copy
        strtok(buf_copy, "\""); // Skip to first quote because that's where the IP begins
        IP = strtok(NULL, "\"");
        if (IP != NULL) { //Parse through and store the IP address into the 4 pointers
            IP1 = strtok(IP, ".");
            IP2 = strtok(NULL, ".");
            IP3 = strtok(NULL, ".");
            IP4 = strtok(NULL, ".");

            if (IP1 && IP2 && IP3 && IP4) { //Display the full IP in its entirety with '.' characters appended for readability
                strcpy(disp[1], IP1);
                strcat(disp[1], ".");
                strcat(disp[1], IP2);
                strcat(disp[1], ".");
                strcpy(disp[2], IP3);
                strcat(disp[2], ".");
                strcat(disp[2], IP4);
                strcat(disp[2], ".");
                Change_Display();
            }
        }
    }
}
