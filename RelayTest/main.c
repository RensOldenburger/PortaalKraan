/*
 */
#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#define DEBOUNCE _delay_ms(10)


void init_knoppen(void)
{
    DDRF &= ~(1<<PF4);      // A pin 4  //  rotarie encoder rechts
    DDRF &= ~(1<<PF5);      // A pin 5  //  rotarie encoder links
    DDRF &= ~(1<<PF1);      // A pin 1  //  start knop
}
void init(void)
{
    DDRB |= (1<<PB7);
    PORTB |= (1<<PB7);

    // ---- ledjes 53 tot 47 ---- //
    DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3);
    DDRL |= (1<<PL0) | (1<<PL1) | (1<<PL2) | (1<<PL3) | (1<<PL4) | (1<<PL5) | (1<<PL6) | (1<<PL7);
    DDRG |= (1<<PG0) | (1<<PG1);

    PORTB &= ~(1<<PB0) & ~(1<<PB1) & ~(1<<PB2) & ~(1<<PB3);
    PORTL &= ~(1<<PL0) & ~(1<<PL1) & ~(1<<PL2) & ~(1<<PL3) & ~(1<<PL4) & ~(1<<PL5) & ~(1<<PL6) & ~(1<<PL7);
    PORTG &= ~(1<<PG0) & ~(1<<PG1);
}

int main(void)
{
    init();
    int besturingx[7] = {PB0, PB1, PB2, PB3, PL0, PL1, PL2};
    int besturingy[7] = {PG1, PG0, PL7, PL6, PL5, PL4, PL3};
    static int x = 0;
    static int y = 0;
    int stand = 1;

    while (1)
    {
        if (x <= 3)
        {
            PORTB |= (1<<besturingx[x]);
        }
        if (x > 3)
        {
            PORTL |= (1<<besturingx[x]);
        }
        if (x > 6)
        {
            PORTL &= ~(1<<besturingx[x]);
            x = 0;
        }
        if (x < 0)
        {
            PORTB &= ~(1<<besturingx[x]);
            x = 6;
        }
        if (y < 2)
        {
            PORTG |= (1<<besturingy[y]);
        }
        if (y >= 2)
        {
            PORTL |= (1<<besturingy[y]);
        }
        if (y > 6)
        {
            PORTL &= ~(1<<besturingy[y]);
            y = 0;
        }
        if (y < 0)
        {
            PORTG &= ~(1<<besturingy[y]);
            y = 6;
        }
        switch (stand)
        {
        case 1:
            if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                if (x <= 3)
                {
                    //PORTB |= (1<<besturingx[x]);
                    PORTB &= ~(1<<besturingx[x]);
                }
                else if (x > 3)
                {
                    //PORTL |= (1<<besturingx[x]);
                    PORTL &= ~(1<<besturingx[x]);
                }
                if ((PINF & (1<<PF4)) == 0)
                {
                    DEBOUNCE;
                    x++;
                }
            }
            if ((PINF & (1<<PF5)) != 0)
            {
                DEBOUNCE;
                if (x <= 3)
                {
                    //PORTB |= (1<<besturingx[x]);
                    PORTB &= ~(1<<besturingx[x]);
                }
                else if (x > 3)
                {
                    //PORTL |= (1<<besturingx[x]);
                    PORTL &= ~(1<<besturingx[x]);
                }
                if ((PINF & (1<<PF5)) == 0)
                {
                    DEBOUNCE;
                    x--;
                }
            }
            if ((PINF & (1<<PF1)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF1)) == 0)
                {
                    DEBOUNCE;
                    stand = 2;
                }
            }
            break;
        case 2:
             if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                if (y <= 1)
                {
                    //PORTG |= (1<<besturingy[y]);
                    PORTG &= ~(1<<besturingy[y]);

                }
                else if (y > 1)
                {
                    //PORTL |= (1<<besturingy[y]);
                    PORTL &= ~(1<<besturingy[y]);
                }
                //PORTG &= ~(1<<besturingy[y]);
                //PORTL &= ~(1<<besturingy[y]);
                if ((PINF & (1<<PF4)) == 0)
                {
                    DEBOUNCE;
                    y++;
                }
            }
            if ((PINF & (1<<PF5)) != 0)
            {
                DEBOUNCE;
                if (y <= 1)
                {
                    //PORTG |= (1<<besturingy[y]);
                    PORTG &= ~(1<<besturingy[y]);
                }
                else if (y >= 2)
                {
                    //PORTL |= (1<<besturingy[y]);
                    PORTL &= ~(1<<besturingy[y]);
                }
                //PORTG &= ~(1<<besturingy[y]);
                //PORTL &= ~(1<<besturingy[y]);
                if ((PINF & (1<<PF5)) == 0)
                {
                    DEBOUNCE;
                    y--;
                }
            }
            if ((PINF & (1<<PF1)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF1)) == 0)
                {
                    DEBOUNCE;
                    stand = 1;
                }
            }
            break;
        }

    }
    return 0;
}
