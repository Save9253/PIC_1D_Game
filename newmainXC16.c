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

unsigned char playerPosition[] = {0,0};
unsigned char playerPassThrough = 0xFF;
unsigned char doorPosition[] = {4,4};
unsigned char doorPassThrough = 0xFF;

unsigned char level = 0;

unsigned char mase[3][8] ={
{
    0b00000000,
    0b00111100,
    0b00000000,
    0b01000010,
    0b01000010,
    0b00000000,
    0b00111100,
    0b00000000,
},
{
    0b00000010,
    0b01111110,
    0b01000000,
    0b01011110,
    0b01000010,
    0b01001010,
    0b01111010,
    0b00000010,
},
{
    0b00000000,
    0b01011011,
    0b01001000,
    0b01111110,
    0b00010000,
    0b11010010,
    0b01110110,
    0b00000100,
},
};

void updateDisplay(void){
    unsigned char player = (1<<playerPosition[0]) & playerPassThrough;
    unsigned char walls = mase[level][playerPosition[1]];
    unsigned char door = (playerPosition[1] == doorPosition[1]) ? ((1<<doorPosition[0]) & doorPassThrough) : 0;
    LATB = ~(player | walls | door); 
}

void movePlayerLeft (void) {
    
    if(playerPosition[0] > 0){//unless the player is already on the most right position move him to the right
        unsigned char nextPosition = ((mase[level][playerPosition[1]]>>(playerPosition[0]-1))&(1<<0));
        if(nextPosition != 1){
            playerPosition[0]--;     
        }
    }
}

void movePlayerRight (void) {
    if(playerPosition[0] < 7){//unless the player is already on the most right position move him to the right
        unsigned char nextPosition = ((mase[level][playerPosition[1]]>>(playerPosition[0]+1))&(1<<0));
        if(nextPosition != 1){
            playerPosition[0]++;     
        }
    }
}

void movePlayerForward (void) {
    if(playerPosition[1] > 0){
        unsigned char nextY = playerPosition[1]-1;
        unsigned char nextPosition = ((mase[level][nextY]>>playerPosition[0])&(1<<0));
        if(nextPosition != 1){//if the next cell in the mase is not a wall go there
            playerPosition[1]--;     
        }else{// otherwise show that wall for a moment
            LATB = ~(mase[level][nextY]);
            __delay_ms(250);
        }  
    }else{//if you ran into the end of the mase blink all LEDs
        LATB = 0;
        __delay_ms(250);
    }
}

void movePlayerBackward (void) {
    if(playerPosition[1] < 7){//unless the player is already on the most right position move him to the right
        unsigned char nextY = playerPosition[1]+1;
        unsigned char nextPosition = ((mase[level][nextY]>>playerPosition[0])&(1<<0)); 
        if(nextPosition != 1){//if the next cell in the mase is not a wall go there
            playerPosition[1]++;    
        }else{// otherwise show that wall for a moment
            LATB = ~(mase[level][nextY]);
            __delay_ms(250);
        }  
    }else{//if you ran into the end of the mase blink all LEDs
        LATB = 0;
        __delay_ms(250);
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
                case 1:
                    movePlayerBackward();
                    break;
                case 2:
                    movePlayerForward();
                    break;
                case 3:
                    movePlayerLeft();
                    break;
            }
            
            __delay_ms(300);
            debounceTimeout = 0;
        }
    }
}

void victoryAnimation(void) {
    int speed = 200;  
    LATB = ~(0b00011000);
    __delay_ms(speed);
    LATB = ~(0b00100100);
    __delay_ms(speed);
    for(char i=0;i<5;i++){
        LATB = ~(0b01011010);
        __delay_ms(speed);
        LATB = ~(0b10100101);
        __delay_ms(speed);
    }
    LATB = ~(0b01000010);
    __delay_ms(speed);
    LATB = ~(0b10000001);
    __delay_ms(speed);
    LATB = ~(0b00000000);
    __delay_ms(speed*3);
}

void victoryCheck(void) {
    if((playerPosition[0] == doorPosition[0]) && (playerPosition[1] == doorPosition[1])){
        victoryAnimation();
        playerPosition[0] = 0;
        playerPosition[1] = 0;
        switch(level){
            case 0:
                doorPosition[0] = 2;
                doorPosition[1] = 0;
                level = 1;
                break;
            case 1:
                doorPosition[0] = 7;
                doorPosition[1] = 7;
                level = 2;
                break;
            case 2:
                doorPosition[0] = 4;
                doorPosition[1] = 4;
                level = 0;
                break;
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
        victoryCheck();
        updateDisplay();
    }
    return 0;
}
