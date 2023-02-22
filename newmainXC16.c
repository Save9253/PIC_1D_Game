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
    while(1){
        unsigned char inputs = PORTA & 0x0001;
        //if(inputs == 0xFFFE){
            if(inputs == 0x0001){
                LATB = 0xFFFD;//1st on
            }else{
                LATB = 0xFFFE;//0th on
            }
            __delay_ms(100);
        //}   
    }
    return 0;
}
