// Newhaven LCD Module Demo 01.c
// Drives a Newhaven NHD-0216K3Z-FS(RGB)-FBW-V3	display
//
// Created: 12/9/2017 3:30:15 PM
// Author : swulf

// I/O configuration
//
// XCK – pin 6 – Port D, PD2
// TXD – pin 3 – Port D, PD1
// SLAVE_SEL - pin 7 - Port D, PD3

// Assumes an 8 MHz system clock

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

// Variable declarations

void InitPort(void)
{
	// Initialize I/O ports
	// PD1: Output
	// PD2: Output
	// PD3: Output
	
	DDRD |= ((1 << PD1) | (1 << PD2) | (1 << PD3));
	PORTD |= (1 << PD3);	// Set Slave Select high
}

void SPIInit(uint8_t spi_mode)
{
	// Initialize the SPI system
	
	// Enable Master SPI Compliant mode
	
	UBRRL = 0;
	UBRRH = 0;

	// Set Master SPI Mode (MSPIM)
	
	UCSRC |= (1 << UMSEL1) | (1 << UMSEL0);
	
	// Set SPI Mode
	
	// SPI Modes
	//
	// SPI Mode		UCPOL		UCPHA
	// --------		-----		-----
	// 0			0			0
	// 1			0			1
	// 2			1			0
	// 3			1			1

	switch(spi_mode)
	{
		case 0:	// Mode 0
		UCSRC &= ~(1 << UCPOL);
		UCSRC &= ~(1 << UCPHA);
		break;
		
		case 1:	// Mode 1
		UCSRC &= ~(1 << UCPOL);
		UCSRC |= (1 << UCPHA);
		break;
		
		case 2:	// Mode 2
		UCSRC &= ~(1 << UCPHA);
		UCSRC |= (1 << UCPOL);
		break;
		
		case 3:	// Mode 3
		UCSRC |= (1 << UCPOL);
		UCSRC |= (1 << UCPHA);
		break;
	}
	
	// Transmit Data Order
	
	UCSRC &= ~(1 << UDORD);		// Zero: MSB of data is transmitted first
	
	// Enable transmitter
	
	UCSRB = (1 << TXEN);
	
	// Set SPI baud rate.
	// IMPORTANT: The baud rate must be set after the transmitter is enabled.
	// Sets 76.8K bps data rate with an 8 MHz clock.
	
	UBRRL = 25;		// Corresponds to a 19.2Kbps baud rate with an 8 MHz system clock
}

void SPIWrite(uint8_t data)
{
	// Write a value out over SPI
	
	loop_until_bit_is_set(UCSRA, UDRE);	// Wait until Data Register is empty
	PORTD &= ~(1 << PD3);				// Set Slave Select low
	UDR = data;							// Transmit data out over SPI
	loop_until_bit_is_set(UCSRA, TXC);	// Wait until all data sent out
	UCSRA |= (1 << TXC);				// Clear TXC bit (write a '1')
	PORTD |= (1 << PD3);				// Set Slave Select high
}

void LCDDisplayOn(void)
{
	// Turn display on
	SPIWrite(0xFE);
	SPIWrite(0x41);
	_delay_us(100);
}

void LCDDisplayOff(void)
{
	// Turn display off
	SPIWrite(0xFE);
	SPIWrite(0x42);
	_delay_us(100);
}

void LCDSetCursor(uint8_t cursor_position)
{
	// Set cursor location
	SPIWrite(0xFE);
	SPIWrite(0x45);
	SPIWrite(cursor_position);
	_delay_us(100);
}

void LCDCursorHome(void)
{
	// Bring cursor home
	SPIWrite(0xFE);
	SPIWrite(0x46);
	_delay_ms(2);
}

void LCDUnderlineCursorOn(void)
{
	// Turn Underline Cursor on
	SPIWrite(0xFE);
	SPIWrite(0x47);
	_delay_ms(2);
}

void LCDUnderlineCursorOff(void)
{
	// Turn Underline Cursor on
	SPIWrite(0xFE);
	SPIWrite(0x48);
	_delay_ms(2);
}

void LCDMoveCursorLeft(void)
{
	// Move cursor left one place
	SPIWrite(0xFE);
	SPIWrite(0x49);
	_delay_us(100);
}

void LCDMoveCursorRight(void)
{
	// Move cursor right one place
	SPIWrite(0xFE);
	SPIWrite(0x4A);
	_delay_us(100);
}

void LCDBlinkingCursorOn(void)
{
	// Turn blinking cursor on
	SPIWrite(0xFE);
	SPIWrite(0x4B);
	_delay_us(100);
}

void LCDBlinkingCursorOff(void)
{
	// Turn blinking cursor off
	SPIWrite(0xFE);
	SPIWrite(0x4C);
	_delay_us(100);
}

void LCDBackspace(void)
{
	// Backspace the cursor
	SPIWrite(0xFE);
	SPIWrite(0x4E);
	_delay_us(100);
}

void LCDClearScreen(void)
{
	// Clear LCD display
	SPIWrite(0xFE);
	SPIWrite(0x51);
	_delay_ms(2);
}

void LCDSetContrast(uint8_t display_contrast)
{
	// Set display contrast
	SPIWrite(0xFE);
	SPIWrite(0x52);
	SPIWrite(display_contrast);
	_delay_us(500);
}

void LCDMoveDisplayLeft(void)
{
	// Move display one place to the left
	SPIWrite(0xFE);
	SPIWrite(0x55);
	_delay_us(100);
}

void LCDMoveDisplayRight(void)
{
	// Move display one place to the right
	SPIWrite(0xFE);
	SPIWrite(0x56);
	_delay_us(100);
}

void LCDShowFirmwareRev(void)
{
	// Display the firmware revision number on the display
	SPIWrite(0xFE);
	SPIWrite(0x70);
	_delay_ms(4);
}

int main(void)
{
	_delay_ms(100);		// Delay to allow the display controller to initialize
	
	InitPort();			// Initialize port D I/O
	SPIInit(3);			// Initialize SPI into Mode 3
	
	// Send some commands to the display

	LCDClearScreen();
	LCDShowFirmwareRev();
	
	_delay_ms(200);
	
	LCDClearScreen();

	SPIWrite('H');
	SPIWrite('e');
	SPIWrite('l');
	SPIWrite('l');
	SPIWrite('o');
	SPIWrite(' ');
	SPIWrite('W');
	SPIWrite('o');
	SPIWrite('r');
	SPIWrite('l');
	SPIWrite('d');
	SPIWrite('!');
	
	_delay_ms(200);
	
	while(1)
	{
		LCDMoveDisplayLeft();
		_delay_ms(20);
	}

	while (1);			// Loop forever
}

