/*
 * Stopwatch_project.c
 *
 *  Created on: Sep 11, 2024
 *      Author: Yahia
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char hour_inc_flag=0;
unsigned char hour_dec_flag=0;
unsigned char min_inc_flag=0;
unsigned char min_dec_flag=0;
unsigned char sec_inc_flag=0;
unsigned char sec_dec_flag=0;
unsigned char toggle_pressed=0;
unsigned char running_flag=1;
unsigned char count_up_flag=1;
unsigned char seconds_units=0;
unsigned char seconds_tens=0;
unsigned char minutes_units=0;
unsigned char minutes_tens=0;
unsigned char hours_units=0;
unsigned char hours_tens=0;
/*******Timer Function******/
void Timer1_ctc_Init(void){
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); //CTC MODE & PRESCALER 1024
	TCNT1=0;
	OCR1A=15625; //compare match 15625
	TIMSK |= (1<<OCIE1A); //ENABLE MODULE INTERRUPT
	SREG |= (1<<7); //ENABLE GLOBAL INTERRUPT

}
void hide_all_digits(void){
	PORTA &= 0xC0;  //This function hides digits of all 7-segments
}
void show_digit(unsigned char n){  //This function shows one 7-segment at a time
	PORTA |= (1<<n);
}
/******Reset Function*******/
void INT0_Init(void){
	MCUCR |= (1<<ISC01); //TRIGGER ON THE FALLING EDGE
	GICR |= (1<<INT0);  //INT0 ENABLE
	SREG |= (1<<7);  //GLOBAL INTERRUPT ENABLE
	DDRD &= ~(1<<PD2);
}
/******Pause Function*****/
void INT1_Init(void){
	MCUCR |= (1<<ISC11) | (1<<ISC10); //TRIGGER ON RISING EDGE
	GICR |= (1<<INT1); //INT1 ENABLE
	SREG |= (1<<7); //GLOBAL INTERRUPT ENABLE
	DDRD &= ~(1<<PD3);
}

/******Resume Function****/
void INT2_Init(void){
	MCUCSR &= ~(1<<ISC2); //WHEN THIS BIT IS ZERO, TRIGGER ON FALLING EDGE
	GICR |= (1<<INT2);  //INT2 ENABLE
	SREG |= (1<<7);  //GLOBAL INTERRUPT ENABLE
	DDRB &= ~(1<<PB2);
}

void toggle_mode(void){
	if((!(PINB & (1<<PB7)))&&(toggle_pressed==0)){
		_delay_ms(50);
		if(!(PINB & (1<<PB7))){
			toggle_pressed=1;
			if(count_up_flag==1){
				count_up_flag=0;
				DDRD |= (1<<5);
				PORTD |= (1<<5);
				PORTD &= ~(1<<4);
			}
			else{
				count_up_flag=1;
				DDRD |= (1<<4);
				PORTD |= (1<<4);
				PORTD&= ~(1<<5);
			}

		}
	}
	if(PINB & (1<<PB7)){
		toggle_pressed=0;
	}
}
int main(void){
	DDRD |= (1<<4);
	PORTD |= (1<<4);
	PORTD&= ~(1<<5);
	DDRA |= 0x3F;
	DDRC |= 0x0F;
	PORTC= (PORTC & 0xF0); //initialize PORTC as zero
	DDRB &= 0; //SET ALL PORTB PINS AS INPUT
	PORTB =0xFF; //ACTIVATE INTERNAL PULL-UP FOR ALL PORTB PINS
	Timer1_ctc_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	while(1){
		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (seconds_units & 0x0F);
		show_digit(5);
		_delay_ms(2);

		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (seconds_tens & 0x0F);
		show_digit(4);
		_delay_ms(2);

		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (minutes_units & 0x0F);
		show_digit(3);
		_delay_ms(2);

		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (minutes_tens & 0x0F);
		show_digit(2);
		_delay_ms(2);

		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (hours_units & 0x0F);
		show_digit(1);
		_delay_ms(2);

		hide_all_digits();
		PORTC = (PORTC & 0xF0) | (hours_tens & 0x0F);
		show_digit(0);
		_delay_ms(2);

		toggle_mode();
		if(running_flag == 0) {  // Ensure stopwatch is paused before increment or decrement

			/******* Hours Decrement *******/
			if((!(PINB & (1 << PB0))) && (hour_dec_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB0))) {  // Ensure button is still pressed
					hour_dec_flag = 1;
					if(!(hours_units == 0 && hours_tens == 0)) {
						if(hours_units == 0 && hours_tens != 0) {
							hours_units = 9;
							hours_tens--;
						} else {
							hours_units--;
						}
					}
				}
			}
			if((PINB & (1 << PB0))) {  // Reset flag when button is released
				hour_dec_flag = 0;
			}

			/****** Hours Increment ******/
			if((!(PINB & (1 << PB1))) && (hour_inc_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB1))) {  // Ensure button is still pressed
					hour_inc_flag = 1;
					if(!(hours_units == 9 && hours_tens == 9)) {
						hours_units++;
						if(hours_units == 10) {
							hours_units = 0;
							hours_tens++;
						}
					}
				}
			}
			if((PINB & (1 << PB1))) {  // Reset flag when button is released
				hour_inc_flag = 0;
			}

			/******* Minutes Decrement *******/
			if((!(PINB & (1 << PB3))) && (min_dec_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB3))) {  // Ensure button is still pressed
					min_dec_flag = 1;
					if(!(minutes_units == 0 && minutes_tens == 0)) {
						if(minutes_units == 0 && minutes_tens != 0) {
							minutes_units = 9;
							minutes_tens--;
						} else {
							minutes_units--;
						}
					}
					else {
						if((hours_units != 0) || (hours_tens != 0)) {
							if(hours_units == 0 && hours_tens != 0) {
								hours_units = 9;
								hours_tens--;
							} else {
								hours_units--;
							}
							minutes_units = 9;
							minutes_tens = 5;
						}
					}
				}
			}
			if((PINB & (1 << PB3))) {  // Reset flag when button is released
				min_dec_flag = 0;
			}

			/****** Minutes Increment ******/
			if((!(PINB & (1 << PB4))) && (min_inc_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB4))) {  // Ensure button is still pressed
					min_inc_flag = 1;
					if(!(minutes_units == 9 && minutes_tens == 5)) {
						minutes_units++;
						if(minutes_units == 10) {
							minutes_units = 0;
							minutes_tens++;
						}
					}
					else {
						minutes_units = 0;
						minutes_tens = 0;
						hours_units++;
						if(hours_units == 10) {
							hours_units = 0;
							hours_tens++;
						}
					}
				}
			}
			if((PINB & (1 << PB4))) {  // Reset flag when button is released
				min_inc_flag = 0;
			}

			/******* Seconds Decrement *******/
			if((!(PINB & (1 << PB5))) && (sec_dec_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB5))) {  // Ensure button is still pressed
					sec_dec_flag = 1;
					if(!(seconds_units == 0 && seconds_tens == 0)) {
						if(seconds_units == 0 && seconds_tens != 0) {
							seconds_units = 9;
							seconds_tens--;
						} else {
							seconds_units--;
						}
					}
					else {
						if((minutes_units != 0) || (minutes_tens != 0)) {
							if(minutes_units == 0 && minutes_tens != 0) {
								minutes_units = 9;
								minutes_tens--;
							} else {
								minutes_units--;
							}
							seconds_units = 9;
							seconds_tens = 5;
						}
						else {
							if((hours_units != 0) || (hours_tens != 0)) {
								if(hours_units == 0 && hours_tens != 0) {
									hours_units = 9;
									hours_tens--;
								} else {
									hours_units--;
								}
								minutes_units = 9;
								minutes_tens = 5;
								seconds_units = 9;
								seconds_tens = 5;
							}
						}
					}
				}
			}
			if((PINB & (1 << PB5))) {  // Reset flag when button is released
				sec_dec_flag = 0;
			}

			/****** Seconds Increment ******/
			if((!(PINB & (1 << PB6))) && (sec_inc_flag == 0)) {
				_delay_ms(50);
				if(!(PINB & (1 << PB6))) {  // Ensure button is still pressed
					sec_inc_flag = 1;
					if(!(seconds_units == 9 && seconds_tens == 5)) {
						seconds_units++;
						if(seconds_units == 10) {
							seconds_units = 0;
							seconds_tens++;
						}
					} else {  // If seconds are at 59
						seconds_units = 0;
						seconds_tens = 0;
						if(!(minutes_units == 9 && minutes_tens == 5)) {
							minutes_units++;
							if(minutes_units == 10) {
								minutes_units = 0;
								minutes_tens++;
							}
						} else {  // If minutes are also at 59, roll them over to 00
							minutes_units = 0;
							minutes_tens = 0;
							hours_units++;
							if(hours_units == 10) {
								hours_units = 0;
								hours_tens++;
							}
						}
					}
				}
			}
			if((PINB & (1 << PB6))) {  // Reset flag when button is released
				sec_inc_flag = 0;
			}



		}




	}
}

ISR(TIMER1_COMPA_vect){
	if((running_flag==1)&&(count_up_flag==1)){

		seconds_units++;
		if(seconds_units==10){
			seconds_units=0;
			seconds_tens++;
			if(seconds_tens==6){
				seconds_tens=0;
				minutes_units++;

				if(minutes_units==10){
					minutes_units=0;
					minutes_tens++;

					if(minutes_tens==6){
						minutes_tens=0;
						hours_units++;

						if(hours_units==10){
							hours_units=0;
							hours_tens++;

							if(hours_tens==10){
								hours_tens=0;
								hours_units=0;
							}
						}
					}
				}
			}
		}
	}
	else if((running_flag==1)&&(count_up_flag==0)){

		if(seconds_units==0){
			if(seconds_tens==0){
				if(minutes_units==0){
					if(minutes_tens==0){
						if(hours_units==0){
							if(hours_tens==0){
								DDRD |= (1<<PD0);
								PORTD |= (1<<PD0);
								running_flag=0;
							}
							else{
								hours_units=9;
								hours_tens--;
								minutes_tens=5;
								minutes_units=9;
								seconds_tens=5;
								seconds_units=9;
							}
						}
						else{
							hours_units--;
							minutes_tens=5;
							minutes_units=9;
							seconds_tens=5;
							seconds_units=9;
						}

					}
					else{
						minutes_units=9;
						minutes_tens--;
						seconds_tens=5;
						seconds_units=9;
					}
				}
				else{
					minutes_units--;
					seconds_tens=5;
					seconds_units=9;
				}
			}
			else{
				seconds_units=9;
				seconds_tens--;
			}
		}
		else{
			seconds_units--;
		}

	}
}
ISR(INT0_vect){
	seconds_units=0;  //reset the time
	seconds_tens=0;
	minutes_units=0;
	minutes_tens=0;
	hours_units=0;
	hours_tens=0;
}
ISR(INT1_vect){
	if(running_flag==1){
		running_flag=0;
	}
}
ISR(INT2_vect){
	if(running_flag==0){
		running_flag=1;
	}
}
