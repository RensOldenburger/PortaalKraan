/*
 * h_bridge.c - XvR 2020
 *
 * Use 8-bit timer. Uses interrupts in order to be able
 * to use the pins on the multifunction shield
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "h_bridge.h"

ISR(TIMER0_OVF_vect)
{
	if (OCR0A == 0 && OCR0B == 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
	else if (OCR0A != 0)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
		PORT_RPWM |= (1<<PIN_RPWM);
	}
	else if (OCR0B != 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM |= (1<<PIN_LPWM);
	}
}

ISR(TIMER0_COMPA_vect)
{
	if (OCR0A != 255)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
	}
}

ISR(TIMER0_COMPB_vect)
{
	if (OCR0B != 255)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
}

void init_h_bridge(void)
{
	// Config pins as output
	DDR_RPWM |= (1<<PIN_RPWM);
	DDR_LPWM |= (1<<PIN_LPWM);

	// Output low
	PORT_RPWM &= ~(1<<PIN_RPWM);
	PORT_LPWM &= ~(1<<PIN_LPWM);

	// Use mode 0, clkdiv = 64
	TCCR0A = 0;
	TCCR0B = (0<<CS02) | (1<<CS01) | (1<<CS00);

	// Disable PWM output
	OCR0A = 0;
	OCR0B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK0 = (1<<OCIE0B) | (1<<OCIE0A) | (1<<TOIE0);

	sei();
}

void h_bridge_set_percentage(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR0B = 0;
			OCR0A = (255*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR0A = 0;
			OCR0B = (255*percentage)/-100;
		}
	}
}
