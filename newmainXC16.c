/*
 * File:   newmainXC16.c
 * Author: Saveliy Saunin
 *
 * Created on February 21, 2023, 8:06 PM
 */

#include "xc.h"
#define FCY 3700000UL
#include <libpic30.h>

void blink(unsigned char ledNumber)
{
    // turn on the last LED for half a second then turn off
    LATB &= ~(1<<ledNumber);
    __delay_ms(500);
    LATB |= (1<<ledNumber);
}

void processInput(unsigned char inputNumber, unsigned char ledNumber)
{
    unsigned char newInState = (PORTA & (1 << inputNumber))>>inputNumber; // select the 0th bit of port A
    unsigned char oldInState = 0;
    unsigned char debounceTimeout = 0;
    if (debounceTimeout == 0)
    { // if there has been enough time since the last press
        if (newInState > oldInState)
        { // if button pressed disable registering further presses for 100ms
            debounceTimeout = 1;
            blink(ledNumber); // run the function
            __delay_ms(100);
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

    while (1)
    {
        processInput(0, 0);
        processInput(1, 1);
        processInput(2, 2);
        processInput(4, 3);
    }
    return 0;
}
