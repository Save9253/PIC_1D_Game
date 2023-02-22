/*
 * File:   newmainXC16.c
 * Author: Saveliy Saunin
 *
 * Created on February 21, 2023, 8:06 PM
 */

#include "xc.h"
#define FCY 3700000UL
#include <libpic30.h>
// Define the period values for the two timers
#define TIMER1_PERIOD 5000
#define TIMER2_PERIOD 2500

// Variables to keep track of timer counts
unsigned int timer1_count = 0;
unsigned int timer2_count = 0;

unsigned char playerPosition = 0;
unsigned char playerPassThrough = 0xFF;
unsigned char doorPassThrough = 0xFF;

unsigned char mase[8][8] ={
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
};

void blink(unsigned char ledNumber)
{
    // turn on the last LED for half a second then turn off
    LATB &= ~(1<<ledNumber);
    __delay_ms(500);
    LATB |= (1<<ledNumber);
}

void updateDisplay(void){
    LATB = ~((1<<playerPosition) & playerPassThrough); 
}

void movePlayerLeft (void) {
    if(playerPosition > 0){//unless the player is already on the most right position move him to the right
        playerPosition--;     
    }
}

void movePlayerRight (void) {
    if(playerPosition < 7){//unless the player is already on the most right position move him to the right
        playerPosition++;
    }
}

void processInput(unsigned char inputNumber, unsigned char comandNumber)
{
    unsigned char newInState = (PORTA & (1 << inputNumber))>>inputNumber; // select the 0th bit of port A
    unsigned char oldInState = 0;
    unsigned char debounceTimeout = 0;
    if (debounceTimeout == 0)
    { // if there has been enough time since the last press
        if (newInState > oldInState)
        { // if button pressed disable registering further presses for 100ms
            debounceTimeout = 1;
            switch(comandNumber){
                case 0:
                    movePlayerRight();
                    break;
                case 3:
                    movePlayerLeft();
                    break;
                default:
                    blink(0);
                    break;
            }
            
            __delay_ms(200);
            debounceTimeout = 0;
        }
    }
}

int main(void)
{
    AD1PCFGL = 0xFFFF; // set all to digital
    TRISA = 0xFFFF; // set all to inputs
    TRISB = 0x0000; // set all to outputs 
    LATB = 0xFFFF; // all are off
    
    // Configure Timer 1
    T1CONbits.TON = 0;          // Turn off Timer 1
    T1CONbits.TCS = 0;          // Use internal clock source
    T1CONbits.TCKPS = 0b11;     // Set prescaler to 1:64?
    TMR1 = 0;                   // Clear Timer 1 count
    PR1 = TIMER1_PERIOD;        // Set Timer 1 period
    T1CONbits.TON = 1;          // Turn on Timer 1
    
    // Configure Timer 2
    T2CONbits.TON = 0;          // Turn off Timer 2
    T2CONbits.TCS = 0;          // Use internal clock source
    T2CONbits.TCKPS = 0b11;     // Set prescaler to 1:1?
    TMR2 = 0;                   // Clear Timer 2 count
    PR2 = TIMER2_PERIOD;        // Set Timer 2 period
    T2CONbits.TON = 1;          // Turn on Timer 2
    
    while (1)
    {
        processInput(0, 0);
        processInput(1, 1);
        processInput(2, 2);
        processInput(4, 3);
        
        // Check Timer 1
        if (IFS0bits.T1IF)
        {
            IFS0bits.T1IF = 0;          // Clear Timer 1 interrupt flag
            
            // Toggle LED1
            playerPassThrough ^= 0xFF;
            
            // Increment Timer 1 count
            timer1_count++;
            
            // Check if Timer 1 count has reached the period
            if (timer1_count >= TIMER1_PERIOD)
            {
                timer1_count = 0;   // Reset Timer 1 count
            }
        }
        
        // Check Timer 2
        
        if (IFS0bits.T2IF)
        {
            IFS0bits.T2IF = 0;          // Clear Timer 2 interrupt flag
            
            // Toggle door value
            doorPassThrough ^= 0xFF;
            
            // Increment Timer 2 count
            timer2_count++;
            
            // Check if Timer 2 count has reached the period
            if (timer2_count >= TIMER2_PERIOD)
            {
                timer2_count = 0;   // Reset Timer 2 count
            }
        }
        
        updateDisplay();
    }
    return 0;
}
