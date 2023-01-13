/*
 */
#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "h_bridge.h"
#include "h_bridge2.h"
#include "h_bridge3.h"

#define DEBOUNCE    _delay_ms(30)

void init_LED(void)
{
    DDRB = (1<<PB7) | (1<<PB6) | (1<<PB5);
    PORTB = (1<<PB7) | (1<<PB6) | (1<<PB5);
    DDRH |= (1<<PH5);       // D pin 8  //  relay
    PORTH &= ~(1<<PH5);

    // ---- bestuurings ledjes ---- //
    DDRB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);                                                  // D pin 50 51 52 53 //
    DDRL |= (1<<PL0) | (1<<PL1) | (1<<PL2) | (1<<PL3) | (1<<PL4) | (1<<PL5) | (1<<PL6) | (1<<PL7);      // D pin 42 43 44 45 46 47 48 49 //
    DDRG |= (1<<PG1) | (1<<PG0);

    PORTB &= ~(1<<PB0) & ~(1<<PB1) & ~(1<<PB2) & ~(1<<PB3);
    PORTL &= ~(1<<PL0) & ~(1<<PL1) & ~(1<<PL2) & ~(1<<PL3) & ~(1<<PL4) & ~(1<<PL5) & ~(1<<PL6) & ~(1<<PL7);
    PORTG &= ~(1<<PG0) & ~(1<<PG1);
}

void init_knoppen(void)
{
    /**< PH3 t/m PH6, PG5 en PE3 bezet voor de H bruggen */
    DDRF &= ~(1<<PF1);      // A pin 1  //  start knop
    DDRF &= ~(1<<PF2);      // A pin 2  //  reset
    DDRF &= ~(1<<PF3);      // A pin 3  //  noodstop
    DDRF &= ~(1<<PF4);      // A pin 4  //  rotarie encoder rechts
    DDRF &= ~(1<<PF5);      // A pin 5  //  rotarie encoder links
    DDRK &= ~(1<<PK0);      // A pin 8  //  limit switches x-as
    DDRK &= ~(1<<PK1);      // A pin 9  //  limit switches y-as
    DDRK &= ~(1<<PK2);      // A pin 10 //  limit switches z-as
    DDRK &= ~(1<<PK3);      // A pin 11 //  rotaties tellen x-as
    DDRK &= ~(1<<PK4);      // A pin 12 //  rotaties tellen y-as
    DDRK &= ~(1<<PK5);      // A pin 13 //  rotaties tellen z-as
}

void init(void)
{
    init_h_bridge();        // h brug 1
    init_h_bridge2();       // h brug 2
    init_h_bridge3();       // h brug 3
    init_LED();             // LEDjes
    init_knoppen();         // knoppen
}

int main(void)
{
    init();                                         // pinnen initialiseren //
    int besturingx[7] = {PB0, PB1, PB2, PB3, PL0, PL1, PL2};        // array voor de x-as ledjes
    int besturingy[7] = {PG1, PG0, PL7, PL6, PL5, PL4, PL3};        // array voor de y-as ledjes
    int rotatiex = 0;                        // rotaties van de motor x-as //
    int rotatiey = 0;                        // rotaties van de motor y-as //
    int rotariez = 0;                        // rotaties van de motor z-as //
    int x = 0;                               // int voor besturing x-as //
    int y = 0;                               // int voor besturing y-as //
    int knop_ingedruktx = 1;                        // limit switch niet eerder ingedrukt x-as //
    int knop_ingedrukty = 1;                        // limit switch niet eerder ingedrukt y-as //
    int knop_ingedruktz = 1;                        // limit switch niet eerder ingedrukt z-as //
    int xKlaar = 0;                                 // x-as is op de juiste plek //
    int yKlaar = 0;                                 // y-as is op de juiste plek //
    int stand = 1;                                  // stand voor de switch case statement //

    while(1)
    {
        if ((PINF & (1<<PF3)) != 0)                 // noodstop //
        {
            DEBOUNCE;
            stand = 7;                              // stand 7 is de nood toestand //
        }

        switch(stand)
        {
        case 1: // besturing x-as //
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            h_bridge_set_percentage3(0);

            if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF4)) == 0)
                {
                    DEBOUNCE;
                    if (x <= 3)
                    {
                        PORTB &= ~(1<<besturingx[x]);
                    }
                    else if (x > 3)
                    {
                        PORTL &= ~(1<<besturingx[x]);
                    }
                    x++;
                }
            }
            if ((PINF & (1<<PF5)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF5)) == 0)
                {
                    DEBOUNCE;
                    if (x <= 3)
                    {
                        PORTB &= ~(1<<besturingx[x]);
                    }
                    else if (x > 3)
                    {
                        PORTL &= ~(1<<besturingx[x]);
                    }
                    x--;
                }
            }
            if (x <= 3)                                                 // eerste 3 ledjes x-as
            {
                PORTB |= (1<<besturingx[x]);
            }
            if (x < 0)                                                  // laatste 4 ledjes weer uit x-as
            {
                PORTB &= ~(1<<besturingx[x]);
                x = 6;                                                  // x weer op 6
            }
            if (x > 3)                                                  // laatste 4 ledjes x-as
            {
                PORTL |= (1<<besturingx[x]);
            }
            if (x > 6)                                                  // eerste 3 ledjes weer uit x-as
            {
                PORTL &= ~(1<<besturingx[x]);
                x = 0;
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
        case 2: // besturing y-as //
            if ((PINF & (1<<PF4)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF4)) == 0)
                {
                    DEBOUNCE;
                    if (y <= 1)
                    {
                        PORTG &= ~(1<<besturingy[y]);
                    }
                    else if (y > 1)
                    {
                        PORTL &= ~(1<<besturingy[y]);
                    }
                    y++;
                }
            }
            if ((PINF & (1<<PF5)) != 0)
            {
                DEBOUNCE;
                if ((PINF & (1<<PF5)) == 0)
                {
                    DEBOUNCE;
                    if (y <= 1)
                    {
                        PORTG &= ~(1<<besturingy[y]);
                    }
                    else if (y > 1)
                    {
                        PORTL &= ~(1<<besturingy[y]);
                    }
                    y--;
                }
            }
            if (y <= 1)                                                  // eerste 2 ledjes y-as
            {
                PORTG |= (1<<besturingy[y]);
            }
            if (y < 0)                                                  // laatste 5 ledjes weer uit y-as
            {
                PORTG &= ~(1<<besturingy[y]);
                y = 6;                                                  // y weer op 6
            }
            if (y > 1)                                                 // laatste 5 ledjes y-as
            {
                PORTL |= (1<<besturingy[y]);
            }
            if (y > 6)                                                  // eerste 2 ledjes weer uit y-as
            {
                PORTL &= ~(1<<besturingy[y]);
                y = 0;                                                  // y weer op 0
            }
            if ((PINF & (1<<PF1)) != 0)            // knop voor activeren van beide assen //
            {
                DEBOUNCE;
                if ((PINF & (1<<PF1)) == 0)
                {
                    DEBOUNCE;
                    h_bridge_set_percentage(30);    // start h brug 1 //
                    h_bridge_set_percentage2(30);   // start h brug 2 //
                    stand = 3;                      // door naar case 3 //
                }
            }
            if ((PINF & (1<<PF2)) != 0)             // reset knop //
            {
                DEBOUNCE;
                if ((PINF & (1<<PF2)) == 0)
                {
                    DEBOUNCE;
                    stand = 1;
                }
            }
            break;
        case 3: // naar x en y coordinaten //
            if ((PINK & (1<<PK3)) != 0)             // limit switch rotaties tellen x-as //
            {
                DEBOUNCE;
                if ((PINK & (1<<PK3)) == 0)
                {
                    DEBOUNCE;
                    rotatiex++;                     // optellen van rotaties x-as //
                }
            }
            if ((PINK & (1<<PK4)) != 0)             // limit switch rotaties tellen y-as //
            {
                DEBOUNCE;
                if ((PINK & (1<<PK4)) == 0)
                {
                    DEBOUNCE;
                    rotatiey++;                     // optellen van rotaties y-as //
                }
            }
            if (x == rotatiex)                // rotaties met coordinaten vergelijken van de x-as //
            {
                h_bridge_set_percentage(0);         // h brug van de x-as uit //
                xKlaar = 1;
            }
            if (y == rotatiey)                // rotaties met coordinaten vergalijken van de y-as //
            {
                h_bridge_set_percentage2(0);        // h brug van de y-as uit //
                yKlaar = 1;
            }
            if (rotatiex == 2)                      // limit switch wordt niet geregistreerd wanneer die ingedrukt wordt gehouden x-as //
            {
                knop_ingedruktx = 0;
            }
            if (rotatiey == 2)                      // limit switch wordt niet geregistreerd wanneer die ingedrukt wordt gehouden y-as //
            {
                knop_ingedrukty = 0;
            }
            if ((PINK & (1<<PK0)) && knop_ingedruktx == 0)   // limit switch zet de h brug op de x-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage(0);         // h brug uit na limit switch x-as //
                knop_ingedruktx = 1;
            }
            if ((PINK & (1<<PK1)) && knop_ingedrukty == 0)   // limit switch zet de h brug op de y-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage2(0);        // h brug uit na limit switch y-as //
                knop_ingedrukty = 1;
            }

            if ((PINF & (1<<PF2)) != 0)                 // reset knop //
            {
                DEBOUNCE;
                if ((PINF & (1<<PF2)) == 0)
                {
                    DEBOUNCE;
                    h_bridge_set_percentage(-60);           // h brug x-as terug naar begin //
                    h_bridge_set_percentage2(-60);          // h brug y-as terug naar begin //
                    stand = 5;                              // door naar case 5 //
                }
            }
            if (xKlaar == 1 && yKlaar == 1)
            {
                h_bridge_set_percentage3(60);
                stand = 4;
            }

            break;
        case 4: // z-as omlaag //
            if ((PINK & (1<<PK5)) != 0)                // limit switch z-as //
            {
                DEBOUNCE;
                knop_ingedruktz = 0;
                h_bridge_set_percentage3(-60);
            }
            if (knop_ingedruktz == 0)
            {
                if ((PINK & (1<<PK2)) != 0)
                {
                    DEBOUNCE;
                    h_bridge_set_percentage3(0);
                    h_bridge_set_percentage(-60);
                    h_bridge_set_percentage2(-60);
                    knop_ingedruktz = 1;
                    stand = 5;
                }
            }
            break;
        case 5:
            if ((PINK & (1<<PK0)) != 0)             // limit switch zet de h brug op de x-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage(0);         // h brug uit na limit switch voor de x-as //
                knop_ingedruktx = 0;
            }
            if ((PINK & (1<<PK1)) != 0)             // limit switch zet de h brug op de y-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage2(0);        // h brug uit na limit switch voor de y-as //
                knop_ingedrukty = 0;
            }
            if (knop_ingedruktx == 0 && knop_ingedrukty == 0) // als beide assen zijn gestopt //
            {
                knop_ingedruktx = 1;
                knop_ingedrukty = 1;
                h_bridge_set_percentage3(60);
                stand = 6;                          // door naar case 6 //
            }
            break;
        case 6:
            if ((PINK & (1<<PK5)) != 0)
            {
                DEBOUNCE;
                PORTH |= (1<<PH5);                       // relay gaat aan //
                knop_ingedruktz = 0;
                h_bridge_set_percentage3(-60);
            }
            if (knop_ingedruktz == 0)
            {
                if ((PINK & (1<<PK2)) != 0)
                {
                    DEBOUNCE;
                    h_bridge_set_percentage3(0);
                    knop_ingedruktz = 1;
                    stand = 1;
                }
            }
            break;
        case 7: // nood toestand
            PORTH |= (1<<PH5);                       // relay gaat aan //
            if ((PINF & (1<<PF3)) == 0)             // noodstop wordt los gelaten //
            {
                DEBOUNCE;
                PORTH &= ~(1<<PH5);                  // relay gaat uit //
                stand = 1;                          // terug naar stand 1 //
            }
            break;
        }
    }
    return 0;
}



