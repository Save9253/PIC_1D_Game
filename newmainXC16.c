/*
 * File:   newmainXC16.c
 * Author: Saveliy Saunin
 *
 * Created on February 21, 2023, 8:06 PM
 */

#include "xc.h"
#define FCY 3700000UL
#include <libpic30.h>

int main(void)
{
    AD1PCFGL = 0xFFFF;
    TRISA = 0xFFFF;
    TRISB = 0x0000;
    LATB = 0xFFFF; // all are off

    void processInput(unsigned char inputNumber, void handlePress)
    {
        unsigned char newInState = PORTA & (1 << inputNumber); // select the 0th bit of port A
        unsigned char oldInState = 0;
        unsigned char debounceTimeout = 0;
        if (debounceTimeout == 0)
        { // if there has been enough time since the last press
            if (newBtnState > oldBtnState)
            { // if button pressed disable registering further presses for 100ms
                debounceTimeout = 1;
                handlePress(); // run the function
                __delay_ms(100);
                debounceTimeout = 0;
            }
        }
    }

    void blink()
    {
        // turn on the last LED for half a second then turn off
        LATB = 0xFFFE;
        __delay_ms(500);
        LATB = 0xFFFF;
    }

    while (1)
    {
        processInput(0, blink());
    }
    return 0;
}
