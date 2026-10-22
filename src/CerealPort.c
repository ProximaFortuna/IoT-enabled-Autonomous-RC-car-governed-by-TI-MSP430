/*
 * CerealPort.c
 *
 *  Created on: Mar 6, 2025
 *      Author: Mopel Kitele
 */

#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "macros.h"
#include "CerealPort.h"

//Global Variables
unsigned int pc_enabled = 0;// Flag indicating PC is ready to communicate
unsigned int pb_index = 0; //Column indicating index, for Process Bufffer
unsigned int T_indexA1 = 0; //Column indicating index, for Transmit Buffer A1
unsigned int T_indexA0 = 0; //Column indicating index, for Transmit Buffer A0
unsigned int B_index; //Row indicating index, for Process Buffer
unsigned int P_index; //Row indicating index, for Passthrough Buffer
unsigned int C_index; //Row indicating index, for Command Buffer
unsigned int Rx_WtrA0; //A0 Write Pointer
unsigned int Rx_RdrA0; //A0 Read Pointer
unsigned int Rx_WtrA1; //A1 Write Pointer
unsigned int Rx_RdrA1; //A1 Write Pointer
char Tx_BufA0[30]; // Transmit Buffer
char Rx_BufA0[30]; //Receive Buffer
char Process_Buf[10][80]; //Process Buffer
char Pass_Buf[10][80]; //Pass-through Buffer
char CMD_Buf[5][15]; //Command Buffer
char Tx_BufA1[80]; // Transmit Buffer
char Rx_BufA1[80]; //Receive Buffer
unsigned int TxA1_Finished = 1; //Flag to indicate completion of A1 Transmission
unsigned int TxA0_Finished = 1; //Flag to indicate completion of A0 Transmission
unsigned int RxA1_Finished; //Flag to indicate completion of A1 Reception
unsigned int RxA0_Finished; //Flag to indicate completion of A0 Reception
unsigned int serial_flags; //CAP'N CRUNCH PRESENTS
char Pass_Queue[2][10]; //Pass-through two dimensional queue for efficiency
char Cmd_Queue[5]; //Command queue
unsigned int Cfront; //Front pointer for Command Queue
unsigned int Cback; //Back pointer for Command Queue
unsigned int front[2]; //Front pointer for Pass-through Queue
unsigned int back[2]; //Back pointer for Pass-through Queue

void Init_Serial(void){ //Initialization of Serial Ports
    Init_Serial_UCA1();
    Init_Serial_UCA0();
}

void Init_Serial_UCA0(void){
    //------------------------------------------------------------------------------
    // TX error (%) RX error (%)
    // TX error (%) RX error (%)
    // Baudrate UCOS16 UCBRx UCFx UCSx   neg  pos   neg  pos
    // 4800     1      104     2   0xD6 -0.08 0.04 -0.10 0.14
    // 9600     1      52      1   0x49 -0.08 0.04 -0.10 0.14
    // 19200    1      26      0   0xB6 -0.08 0.16 -0.28 0.20
    // 57600    1      8       10  0xF7 -0.32 0.32 -1.00 0.36
    // 115200   1      4       5   0x55 -0.80 0.64 -1.12 1.76
    // 460800   0      17      0   0x4A -2.72 2.56 -3.76 7.28
    //------------------------------------------------------------------------------
    // Configure eUSCI_A0 for UART mode
    UCA0CTLW0 = 0;
    UCA0CTLW0 |= UCSWRST; // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
    UCA0CTLW0 &= ~UCMSB; // MSB, LSB select
    UCA0CTLW0 &= ~UCSPB; // UCSPB = 0(1 stop bit) OR 1(2 stop bits)
    UCA0CTLW0 &= ~UCPEN; // No Parity
    UCA0CTLW0 &= ~UCSYNC;
    UCA0CTLW0 &= ~UC7BIT;
    UCA0CTLW0 |= UCMODE_0;
    // BRCLK Baudrate UCOS16 UCBRx UCFx UCSx neg pos neg pos
    // 8000000 115200 1 4 5 0x55 -0.80 0.64 -1.12 1.76
    // UCA?MCTLW = UCSx + UCFx + UCOS16
    UCA0BRW = 4; // 115,200 baud
    UCA0MCTLW = 0x5551;
    UCA0CTLW0 &= ~UCSWRST; // release from reset
    //    UCA0TXBUF = 0x00; // Prime the Pump
    UCA0IE |= UCRXIE; // Enable RX interrupt
    //------------------------------------------------------------------------------
}

void Init_Serial_UCA1(void){ //115,2000
    //------------------------------------------------------------------------------
    // TX error (%) RX error (%)
    // Baudrate UCOS16 UCBRx UCFx UCSx   neg  pos   neg  pos
    // 4800     1      104     2   0xD6 -0.08 0.04 -0.10 0.14
    // 9600     1      52      1   0x49 -0.08 0.04 -0.10 0.14
    // 19200    1      26      0   0xB6 -0.08 0.16 -0.28 0.20
    // 57600    1      8       10  0xF7 -0.32 0.32 -1.00 0.36
    // 115200   1      4       5   0x55 -0.80 0.64 -1.12 1.76
    // 460800   0      17      0   0x4A -2.72 2.56 -3.76 7.28
    //------------------------------------------------------------------------------
    // Configure eUSCI_A0 for UART mode
    UCA1CTLW0 = 0;
    UCA1CTLW0 |= UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
    UCA1CTLW0 &= ~UCMSB; // MSB, LSB select
    UCA1CTLW0 &= ~UCSPB; // UCSPB = 0(1 stop bit) OR 1(2 stop bits)
    UCA1CTLW0 &= ~UCPEN; // No Parity
    UCA1CTLW0 &= ~UCSYNC;
    UCA1CTLW0 &= ~UC7BIT;
    UCA1CTLW0 |= UCMODE_0;
    // BRCLK Baudrate UCOS16 UCBRx UCFx UCSx neg pos neg pos
    // 8000000 115200 1 4 5 0x55 -0.80 0.64 -1.12 1.76
    // UCA?MCTLW = UCSx + UCFx + UCOS16
    UCA1BRW = 4; // STARTS AT 115,200 baud
    UCA1MCTLW = 0x5551;
    UCA1CTLW0 &= ~UCSWRST; // release from reset
    //    UCA1TXBUF = 0x00; // Prime the Pump
    UCA1IE |= UCRXIE; // Enable RX interrupt
    //------------------------------------------------------------------------------
}

//IOT to FRAM!
#pragma vector=EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void){
    //------------------------------------------------------------------------------
    // Echo back RXed character, confirm TX buffer is ready first
    switch(__even_in_range(UCA0IV,0x08)){
    case 0: // Vector 0 - no interrupt
        break;
    case 2: // Vector 2 - RXIFG
        Rx_BufA0[Rx_WtrA0] = UCA0RXBUF; // Rx -> Rx_Buf character array
        if (++Rx_WtrA0 >= (sizeof(Rx_BufA0))) Rx_WtrA0 = RESET_STATE; //RING BUFFFF EFFECCC
        serial_flags |= UCA0RX;
        break;
    case 4: // Vector 4 - TXIFG
        UCA0TXBUF = Tx_BufA0[T_indexA0]; // Transmit Current Indexed value
        if(Tx_BufA0[T_indexA0] == '\r') TxA0_Finished = CR;
        if(Tx_BufA0[T_indexA0] == '\n' && TxA0_Finished == CR || Tx_BufA0[T_indexA0] == '\0'){ // Is the next T_index location NULL - End of Command
            UCA0IE &= ~UCTXIE; // Disable Tx interrupt
            UCA0IFG |= UCTXIFG;
        }
        Tx_BufA0[T_indexA0] = NULL;
        if (++T_indexA0 >= sizeof(Tx_BufA0)) T_indexA0 = BEGINNING; //RING BUFFFF EFFECCC
        break;
    default: break;
    }
    //------------------------------------------------------------------------------
}

// USB TRANSMISSION AND RECEPTION INTERRUPT CONFIG! (PC TALKS 2 FRAM)
#pragma vector=EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void){
    //------------------------------------------------------------------------------
    // Echo back RXed character, confirm TX buffer is ready first
    switch(__even_in_range(UCA1IV,0x08)){
    case 0: // Vector 0 - no interrupt
        break;
    case 2: // Vector 2 - RXIFG
        Rx_BufA1[Rx_WtrA1] = UCA1RXBUF; // Rx -> Rx_Buf character array
        if (++Rx_WtrA1 >= (sizeof(Rx_BufA1))) Rx_WtrA1 = RESET_STATE; // Circular buffer back to beginning (USE A BEGINNING MACRO)
        serial_flags |= UCA1RX;
        pc_enabled = TRUE;
        break;
    case 4: // Vector 4 - TXIFG
        UCA1TXBUF = Tx_BufA1[T_indexA1]; // Transmit Current Indexed value
        if(Tx_BufA1[T_indexA1] == '\r') TxA1_Finished = CR;
        if(Tx_BufA1[T_indexA1] == '\n' && TxA1_Finished == CR || Tx_BufA1[T_indexA1] == '\0'){ // Is the next T_index location NULL - End of Command
            UCA1IE &= ~UCTXIE; // Disable Tx interrupt
            UCA1IFG |= UCTXIFG;
        }
        Tx_BufA1[T_indexA1] = NULL;
        if (++T_indexA1 >= sizeof(Tx_BufA1)) T_indexA1 = BEGINNING;
        break;
    default: break;
    }
    //------------------------------------------------------------------------------
}

void Pass_enqueue (unsigned int port){ //Adding to Passthrough Queue
    Pass_Queue[port][back[port]] = P_index;
    if(++back[port] >= 10) back[port] = 0;
}

void CMD_enqueue (void){ //Adding to Command Queue
    Cmd_Queue[Cback] = C_index;
    if(++Cback >= 5) Cback = 0;
}

unsigned int Pass_dequeue(unsigned int port){ //Removing from Passthrough Queue
    unsigned int desired;
    desired = Pass_Queue[port][front[port]];
    if(++front[port] >= 10) front[port] = 0;
    return desired;
}

unsigned int CMD_dequeue(void){ //Removing from Command Queue
    unsigned int desired;
    desired = Cmd_Queue[Cfront];
    if(++Cfront >= 5) Cfront = 0;
    return desired;
}

void Parse (unsigned int port){ //Sift and sort ALL information that enters the process buffer into relevant information (COMMANDS) and trash (PASSTHROUGH)
    char * New, * Old;
    Old = Process_Buf[B_index];
    New = strchr(Old,'$');
    if(New != NULL){
        strncpy(Pass_Buf[P_index], Old, New - Old);
        Pass_Buf[P_index][New - Old] = NULL;
    }
    else strcpy(Pass_Buf[P_index],Old);
    Old = New + 1;
    while (New != NULL){
        New = strchr(Old,';');
        if(New != NULL){
            strncpy(CMD_Buf[C_index], Old, New - Old);
            CMD_Buf[C_index][New - Old] = NULL;
            CMD_enqueue();
            if(++C_index >= 5) C_index = BEGINNING;
            Old = New + 1;
        }
        New = strchr(Old,'$');
        if(New != NULL){
            strncat(Pass_Buf[P_index], Old, New - Old);
            Old = New +1;
        }
        else strcat(Pass_Buf[P_index], Old);
    }
    if(Pass_Buf[P_index][0] != 0x0D ){
        Pass_enqueue(port);
        if (++P_index >= 10) P_index = BEGINNING;
    }
}

void Cereal_Process(void){ //The ears and mouth of this vehicle.
    if(serial_flags & UCA0RX){
        char RX0Copy;
        while (Rx_WtrA0 != Rx_RdrA0){ //If Read is Not Equal to Write (If there is a received character that hasn't been processed)
            RX0Copy = Rx_BufA0[Rx_RdrA0];
            Process_Buf[B_index][pb_index] = RX0Copy;
            Rx_BufA0[Rx_RdrA0] = NULL;
            if (++pb_index >= sizeof(Process_Buf[0])) pb_index = BEGINNING; //RING BUFFFF EFFECCC
            if (++Rx_RdrA0 >= sizeof(Rx_BufA0)) Rx_RdrA0 = BEGINNING; //RING BUFFFF EFFECCC
            if (RX0Copy == '\0') pb_index = BEGINNING;
            if (RX0Copy == '\r') RxA0_Finished = CR;
            if (RX0Copy == '\n' && RxA0_Finished == CR){
                // NEVER CHANGE!!
                RxA0_Finished = RESET;
                Process_Buf[B_index][pb_index] = NULL;
                // Line complete function
                SSID_Display();
                IP_Display();
                Parse(1);
                serial_flags |= UCA1TX;
                //Prepare next buffer line
                pb_index = BEGINNING;
                if (++B_index >= 10) B_index = BEGINNING;
            }
        }
        serial_flags &= ~UCA0RX;
    }
    if (serial_flags & UCA0TX) {
        if (front[0] != back[0] && TxA0_Finished){
            strcpy(Tx_BufA0, Pass_Buf[Pass_dequeue(0)]);
            T_indexA0 = BEGINNING;
            UCA0IE |= UCTXIE;
            TxA0_Finished = RESET;
        }
        else if (front[0] == back[0])serial_flags &= ~UCA0TX;
    }
    if(serial_flags & UCA1RX){
        char RX1Copy;
        while (Rx_WtrA1 != Rx_RdrA1){ //If Read is Not Equal to Write (If there is a received character that hasn't been processed)
            RX1Copy = Rx_BufA1[Rx_RdrA1];
            Process_Buf[B_index][pb_index] = RX1Copy;
            Rx_BufA1[Rx_RdrA1] = NULL;
            if (++pb_index >= sizeof(Process_Buf[0])) pb_index = BEGINNING; //RING BUFFFF EFFECCC
            if (++Rx_RdrA1 >= sizeof(Rx_BufA1)) Rx_RdrA1 = BEGINNING; //RING BUFFFF EFFECCC
            if (RX1Copy == '\0') pb_index = BEGINNING;
            if (RX1Copy == '\r') RxA1_Finished = CR;
            if (RX1Copy == '\n' && RxA1_Finished == CR){
                // NEVER CHANGE!!
                RxA1_Finished = RESET;
                Process_Buf[B_index][pb_index] = NULL;
                // Line complete function
                Parse(0);
                serial_flags |= UCA0TX;
                //Prepare next buffer line
                pb_index = BEGINNING;
                if (++B_index >= 10) B_index = BEGINNING;
            }
        }
        serial_flags &= ~UCA1RX;
    }
    if(serial_flags & UCA1TX){
        if(front[1] != back[1] && TxA1_Finished){
            strcpy(Tx_BufA1, Pass_Buf[Pass_dequeue(1)]);
            T_indexA1 = BEGINNING;
            UCA1IE |= UCTXIE;
            TxA1_Finished = RESET;
        }
        else if (front[1] == back[1]) serial_flags &= ~UCA1TX;
    }
}
