/*
 * File:   newmainXC16.c
 * Author: savel
 *
 * Created on February 21, 2023, 8:06 PM
 */

#include "xc.h"
#define FCY 3700000UL
#include <libpic30.h>

int main(void) {
    AD1PCFGL = 0xFFFF;
    TRISA = 0xFFFF;
    TRISB = 0x0000;
    LATB = 0xFFFF;// all are off
    
    unsigned char handlePress() {
        //turn on the last LED for half a second then turn off
        LATB = 0xFFFE;
        __delay_ms(500);
        LATB = 0xFFFF;
    }
    
    while(1){
        unsigned char newBtnState = PORTA & (1<<0);// select the 0th bit of port A
        unsigned char oldBtnState = 0;
        unsigned char debounceTimeout = 0;
        if(debounceTimeout == 0){//if there has been enough time since the last press
            if(newBtnState > oldBtnState){//if button pressed disable registering further presses for 100ms
                debounceTimeout = 1;
                handlePress();// run the function
                __delay_ms(100);
                debounceTimeout = 0;
            }   
        }
        
    }
    return 0;
}
