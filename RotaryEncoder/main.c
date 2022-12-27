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
#define DEBOUNCE    _delay_ms(50)

void init_LED(void)
{
    DDRB = (1<<PB7) | (1<<PB6) | (1<<PB5);
    PORTB = (1<<PB7) | (1<<PB6) | (1<<PB5);
    DDRH |= (1<<PH5);       // D pin 8  //  relay
    PORTH |= (1<<PH5);

    // ---- bestuurings ledjes ---- //
    DDRG = (1<<PG1) | (1<<PG0);                         // D pin 40 41 //
    DDRL = 0xff;                                        // D pin 42 43 44 45 46 47 48 49 //
    DDRB = (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);   // D pin 50 51 52 53 //
    PORTG = (1<<PG1) | (1<<PG0);
    PORTL = 0xff;
    PORTB = (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
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

int init(void)
{
    init_h_bridge();        // h brug 1
    init_h_bridge2();       // h brug 2
    init_LED();             // LEDjes
    init_knoppen();         // knoppen
}

int main(void)
{
    int besturingX[] = {};
    int besturingY[] = {};

    init();                                         // pinnen initialiseren //
    static int tellerx = 0;                         // invoer x-as //
    static int tellery = 0;                         // invoer y-as //
    static int tellerz = 0;                         // invoer z-as //
    static int rotatiex = 0;                        // rotaties van de motor x-as //
    static int rotatiey = 0;                        // rotaties van de motor y-as //
    static int rotariez = 0;                        // rotaties van de motor z-as //
    int knop_ingedruktx = 1;                        // limit switch niet eerder ingedrukt x-as //
    int knop_ingedrukty = 1;                        // limit switch niet eerder ingedrukt y-as //
    int knop_ingedruktz = 1;                        // limit switch niet eerder ingedrukt z-as //
    int stand = 1;                                  // stand voor de switch case statement //

    while(1)
    {
        if ((PINF & (1<<PF3)) != 0)                 // noodstop //
        {
            DEBOUNCE;
            stand = 6;                              // stand 6 is de nood toestand //
        }
        if ((PINF & (1<<PF2)) == 0)                 // reset knop //
        {
            DEBOUNCE;
            h_bridge_set_percentage(-80);           // h brug x-as terug naar begin //
            h_bridge_set_percentage2(-80);          // h brug y-as terug naar begin //
            stand = 5;                              // door naar case 5 //
        }
        switch(stand)
        {
        case 1:
            h_bridge_set_percentage(0);
            h_bridge_set_percentage2(0);
            if ((PINF & (1<<PF4)) != 0)             // rotary encoder naar rechts draaien voor de x-as //
            {
                DEBOUNCE;
                tellerx++;                          // rotaties naar rechts tellen //
                ///PORTB ^= (1<<PB7);
            }
            if ((PINF & (1<<PF5)) != 0)             // rotary encoder naar links draaien voor de x-as //
            {
                DEBOUNCE;
                tellerx--;                          // rotaties naar links tellen //
                ///PORTB ^= (1<<PB6);
            }
            if ((PINF & (1<<PF1)) != 0)             // knop voor activeren x-as //
            {
                DEBOUNCE;
                stand = 2;                          // door naar case 2 //
            }
            break;
        case 2:
            if ((PINF & (1<<PF4)) != 0)             // rotary encoder naar rechts draaien voor de y-as //
            {
                DEBOUNCE;
                tellery++;                          // rotaties naar rechts tellen //
                ///PORTB ^= (1<<PB7);
            }
            if ((PINF & (1<<PF5)) != 0)             // rotary encoder naar links draaien voor de y-as //
            {
                DEBOUNCE;
                tellery--;                          // rotaties naar links tellen //
                ///PORTB ^= (1<<PB6);
            }
            if (!(PINF & (1<<PF1)) == 0)            // knop voor activeren van beide assen //
            {
                DEBOUNCE;
                h_bridge_set_percentage(60);        // start h brug 1 //
                h_bridge_set_percentage2(60);       // start h brug 2 //
                stand = 3;                          // door naar case 3 //
            }
            break;

        case 3:
            if ((PINK & (1<<PK3)) == 0)             // limit switch rotaties tellen x-as //
            {
                DEBOUNCE;
                if ((PINK & (1<<PK3)) != 0)
                {
                    DEBOUNCE;
                    rotatiex++;                     // optellen van rotaties x-as //
                    ///PORTB ^= (1<<PB5);
                }
            }
            if ((PINK & (1<<PK4)) == 0)             // limit switch rotaties tellen y-as //
            {
                DEBOUNCE;
                if ((PINK & (1<<PK4)) != 0)
                {
                    DEBOUNCE;
                    rotatiey++;                     // optellen van rotaties y-as //
                    ///PORTB ^= (1<<PB5);
                }
            }
            if (tellerx == rotatiex)                // rotaties met coordinaten vergelijken van de x-as //
            {
                h_bridge_set_percentage(0);         // h brug van de x-as uit //
            }
            if (tellery == rotatiey)                // rotaties met coordinaten vergalijken van de y-as //
            {
                h_bridge_set_percentage2(0);        // h brug van de y-as uit //
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
            if ((PINF & (1<<PF1)) != 0)             // reset knop //
            {
                DEBOUNCE;
                stand = 5;                          // door naar case 4 //
            }
            break;
        case 4:
            // z-as omlaag
            break;
        case 5:
            if ((PINK & (1<<PK0)) != 0)             // limit switch zet de h brug op de x-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage(0);         // h brug uit na limit switch voor de x-as //
                knop_ingedruktx = 1;
            }
            if ((PINK & (1<<PK1)) != 0)             // limit switch zet de h brug op de y-as uit //
            {
                DEBOUNCE;
                h_bridge_set_percentage2(0);        // h brug uit na limit switch voor de y-as //
                knop_ingedrukty = 1;
            }
            if (knop_ingedruktx == knop_ingedrukty) // als beide assen zijn gestopt //
            {
                stand = 1;                          // door naar case 1 //
            }
            break;
        case 6: // nood toestand
            DDRH |= (1<<PH5);                       // relay gaat aan //
            if ((PINF & (1<<PF3)) == 0)             // noodstop wordt los gelaten //
            {
                DEBOUNCE;
                DDRH &= ~(1<<PH5);                  // relay gaat uit //
                stand = 1;                          // terug naar stand 1 //
            }
            break;
        }
    }
    return 0;
}



