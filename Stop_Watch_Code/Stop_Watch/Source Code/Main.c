/*
 *  Main.c
 *
 *  Created on: Feb 1, 2024
 *
 *  Author: Khaled Samy
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/*--------Defines--------*/
#define TRUE 1
#define SET_BIT(REG,BIT_NUM) (REG |= (1 << BIT_NUM))
#define CLEAR_BIT(REG,BIT_NUM) (REG &= ~(1 << BIT_NUM))

/*--------Functions--------*/
void InputOutput_Init(void);
void Timer1_Init(void);
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void Stop_Watch_Display(void);

/*--------Global Variables--------*/
unsigned char SEC1 = 0;
unsigned char SEC2 = 0;
unsigned char MIN1 = 0;
unsigned char MIN2 = 0;
unsigned char HOUR1 = 0;
unsigned char HOUR2 = 0;

int main(void) {
	InputOutput_Init();
	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	while (TRUE) {
		Stop_Watch_Display();
	}
	return 0;
}

void InputOutput_Init(void) {
	DDRC |= 0x0F;  //First 4 pins in PORTC are OUTPUT to the 7447 Decoder
	PORTC &= 0xF0;  //Send 4 ZEROS to the 7447 Decoder

	DDRA |= 0x3F; //First 6 pins in PORTA are OUTPUT (Enable pins for the six 7 Segments)
	PORTA |= 0x3F;  //Enable all the six 7 Segments

	CLEAR_BIT(DDRD, PD2);  //Button for INT0
	SET_BIT(PORTD, PD2); //Internal Pull Up

	CLEAR_BIT(DDRD, PD3); //Button for INT1

	CLEAR_BIT(DDRB, PB2); //Button for INT2
	SET_BIT(PORTB, PB2); //Internal Pull Up
}

ISR(TIMER1_COMPA_vect) {
	/* Increment 1 SECOND */
	SEC1++;

	if (SEC1 == 10) {
		SEC2++;
		SEC1 = 0;
	}
	if (SEC2 == 6) {
		MIN1++;
		SEC2 = 0;
	}
	if (MIN1 == 10) {
		MIN2++;
		MIN1 = 0;
	}
	if (MIN2 == 6) {
		HOUR1++;
		MIN2 = 0;
	}
	if (HOUR1 == 10) {
		HOUR2++;
		HOUR1 = 0;
	}

	// Check for day reset
	if (HOUR2 == 2 && HOUR1 == 4) // 24 hours reached
			{
		// Reset all variables at the end of the day
		SEC1 = SEC2 = MIN1 = MIN2 = HOUR1 = HOUR2 = 0;
	}
}

ISR(INT0_vect) {
	/* Reset */
	SEC1 = SEC2 = MIN1 = MIN2 = HOUR1 = HOUR2 = 0;

	// Reset Timer1 counter (TCNT1)
	TCNT1 = 0;
}

ISR(INT1_vect) {
	/* Stop Timer1 (No Clock) */
	CLEAR_BIT(TCCR1B, CS12);
	CLEAR_BIT(TCCR1B, CS11);
	CLEAR_BIT(TCCR1B, CS10);
}

ISR(INT2_vect) {
	/* Timer1 Resume (Activate The Clock) */
	SET_BIT(TCCR1B, CS11);
	SET_BIT(TCCR1B, CS10);
}

void Timer1_Init(void) {
	/* Timer1 will count 15,625 times per second with prescaler 64 */

	// Enable Module (Timer1 in CTC Mode with OCR1A) Interrupt
	SET_BIT(TIMSK, OCIE1A);

	// Disconnect OC1A and OC1B, non-PWM mode
	TCCR1A = 0;

	// Set the value for 1 second interrupt (Assuming Prescaler = 64)
	OCR1A = 15625; // (1 second / [1 / (1 MHz / 64)]) = 15625

	// Enable Global Interrupt
	SET_BIT(SREG, 7);

	// Initial Value of Timer1
	TCNT1 = 0;

	// CTC on TOP = OCR1A (Mode 4), Assume Prescaler = 64
	SET_BIT(TCCR1B, WGM12);
	SET_BIT(TCCR1B, CS11);
	SET_BIT(TCCR1B, CS10);
}

void INT0_Init(void) {
	// Enable Global Interrupt
	SET_BIT(SREG, 7);

	// Trigger INT0 with the Falling edge
	SET_BIT(MCUCR, ISC01);

	// Enable Module(External Interrupt 0) Interrupt
	SET_BIT(GICR, INT0);
}

void INT1_Init(void) {
	// Enable Global Interrupt
	SET_BIT(SREG, 7);

	// Trigger INT1 with the Raising edge
	SET_BIT(MCUCR, ISC11);
	SET_BIT(MCUCR, ISC10);

	// Enable Module(External Interrupt 1) Interrupt
	SET_BIT(GICR, INT1);
}

void INT2_Init(void) {
	// Enable Global Interrupt
	SET_BIT(SREG, 7);

	// Trigger INT2 with the Falling edge
	CLEAR_BIT(MCUCSR, ISC2);

	// Enable Module(External Interrupt 2) Interrupt
	SET_BIT(GICR, INT2);
}

void Stop_Watch_Display(void) {
	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F); //Display the units digit of seconds
	SET_BIT(PORTA, PA0); //Enable only the first 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F); //Display the tenth digit of seconds
	SET_BIT(PORTA, PA1); //Enable only the second 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F); //Display the units digit of minutes
	SET_BIT(PORTA, PA2); //Enable only the third 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F); //Display the tenth digit of minutes
	SET_BIT(PORTA, PA3); //Enable only the fourth 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (HOUR1 & 0x0F);  //Display the units digit of hours
	SET_BIT(PORTA, PA4); //Enable only the fifth 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (HOUR2 & 0x0F);  //Display the tenth digit of hours
	SET_BIT(PORTA, PA5); //Enable only the sixth 7 Segment
}
