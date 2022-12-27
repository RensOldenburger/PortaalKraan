#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define SDI         PH5
#define Shift_Clk   PH4
#define Latch_Clk   PG5

// defines alphabet letters based in the 7 segment display //
#define LA  ~0x77
#define LB  ~0x7c
#define LC  ~0x39
#define LD  ~0x5e
#define LE  ~0x79
#define LF  ~0x71
#define LG  ~0x6f
#define LH  ~0x76
#define LI  ~0x19
#define LJ  ~0x1e
#define LK  ~0x7a
#define LL  ~0x38
#define LM  ~0x37
#define LN  ~0x54
#define LO  ~0x3f
#define LP  ~0x73
#define LQ  ~0x67
#define LR  ~0x50
#define LS  ~0x6d
#define LT  ~0x78
#define LU  ~0x1c
#define LV  ~0x3e
#define LW  ~0x1d
#define LX  ~0x70
#define LY  ~0x6e
#define LZ  ~0x29
#define LSP ~0x00

#define D0  0xC0
#define D1  0xF9
#define D2  0xA4
#define D3  0xB0
#define D4  0x99
#define D5  0x92
#define D6  0x83
#define D7  0xF8
#define D8  0x80
#define D9  0x90

void init (void)
{
	// Initialiseer de pinnen voor datain, shiftclk en latchclk als output
    DDRH |= (1<<SDI);
    DDRH |= (1<<Shift_Clk);
    DDRG |= (1<<Latch_Clk);

	// Maak shiftclk en latchclk laag
	PORTH &= ~(1<<Shift_Clk);
    PORTG &= ~(1<<Latch_Clk);
}

void send_data(char data)
{
	// Herhaal voor alle bits in een char
	int i;
    for(i = 0; i < 8; i++)
	{
	    if (data & (1<< (7-i)))
        {
            PORTH |= (1<<SDI);
        }
        else
        {
            PORTH &= ~(1<<SDI);
        }
        PORTH |= (1<<Shift_Clk);
        PORTH &= ~(1<<Shift_Clk);
		// Bepaal de waarde van de bit die je naar het schuifregister
		// wil sturen
		// Maak de juiste pin hoog of laag op basis van de bepaalde waarde
		// van het bit
		// Toggle shiftclk (hoeveel tijd moest het signaal minimaal hoog zijn?)
	}
}

void display(char data, char disp)
{
	send_data(data);
	send_data(disp);

	// Toggle latchclk (hoeveel tijd moest het signaal minimaal hoog zijn?)
	PORTG |= (1<< Latch_Clk);
    PORTG &= ~(1<< Latch_Clk);
}

int main(void)
{
	init();

	while(1)
	{
	    display(LSP, 0x01);
	    display(D4, 0x02);
	    display(D2, 0x04);
	    display(D0, 0x08);
	    //display_1 0b00000001        0x01
        //display_2 0b00000010        0x02
        //display_3 0b00000100        0x04
        //display_4 0b00001000        0x0F

        //0 = C0
        //1 = F9
        //2 = A4
        //3 = B0
        //4 = 99
        //5 = 92
        //6 = 83
        //7 = F8
        //8 = 80
        //9 = 90
	}
}
