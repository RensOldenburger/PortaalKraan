/*
 * h_bridge.c - XvR 2020
 *
 * Use 8-bit timer. Uses interrupts in order to be able
 * to use the pins on the multifunction shield
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "h_bridge3.h"

ISR(TIMER1_OVF_vect)
{
	if (OCR1A == 0 && OCR1B == 0)
	{
		PORT_RPWM3 &= ~(1<<PIN_RPWM3);
		PORT_LPWM3 &= ~(1<<PIN_LPWM3);
	}
	else if (OCR1A != 0)
	{
		PORT_LPWM3 &= ~(1<<PIN_LPWM3);
		PORT_RPWM3 |= (1<<PIN_RPWM3);
	}
	else if (OCR1B != 0)
	{
		PORT_RPWM3 &= ~(1<<PIN_RPWM3);
		PORT_LPWM3 |= (1<<PIN_LPWM3);
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (OCR1A != 65535)
	{
		PORT_RPWM3 &= ~(1<<PIN_RPWM3);
	}
}

ISR(TIMER1_COMPB_vect)
{
	if (OCR1B != 65535)
	{
		PORT_LPWM3 &= ~(1<<PIN_LPWM3);
	}
}

void init_h_bridge3(void)
{
	// Config pins as output
	DDR_RPWM3 |= (1<<PIN_RPWM3);
	DDR_LPWM3 |= (1<<PIN_LPWM3);

	// Output low
	PORT_RPWM3 &= ~(1<<PIN_RPWM3);
	PORT_LPWM3 &= ~(1<<PIN_LPWM3);

	// Use mode 0, clkdiv = 64
	TCCR1A = 0;
	TCCR1B = (0<<CS12) | (0<<CS11) | (1<<CS10);

	// Disable PWM output
	OCR1A = 0;
	OCR1B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK1 = (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1);

	sei();
}

void h_bridge_set_percentage3(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR1B = 0;
			OCR1A = (65535*percentage)/100;   // 65535
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR1A = 0;
			OCR1B = (65535*percentage)/-100;  // 65535
		}
	}
}
