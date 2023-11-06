/*
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Description: Source file for the AVR LCD driver
 *
 * Author: Mahmoud Khalafallah
 */

#include "lcd.h"
#include "../common_macros.h"
#include <util/delay.h>
#include "../GPIO/gpio.h"

/*
 * Description:
 * LCD initialization
 * 1. Set RS and RW and Enable pin to be output
 * 2. Set Data port to be output For 8 bit mode
 * 3. set D4-D7 pins to be output for 4 bit mode
 * 4. function set for 8 or 4 bit mode
 * 5. Display control
 * 6. Clear display
 * 7. Entry mode set
 */
void LCD_Init() {

	GPIO_setupPinDirection(LCD_CTRL_PORT, LCD_RS, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_CTRL_PORT, LCD_E, PIN_OUTPUT);

	_delay_ms(20);
#if (LCD_Mode == 8)
	GPIO_setupPortDirection(LCD_DATA_PORT, PORT_OUTPUT);
#elif (LCD_Mode == 4)
	GPIO_setupPinDirection(LCD_DATA_PORT, LCD_D4_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT, LCD_D5_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT, LCD_D6_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT, LCD_D7_PIN, PIN_OUTPUT);
#endif

	_delay_ms(30);
#if (LCD_Mode == 8)
	LCD_sendCommand(0x38);
	_delay_ms(1);
	LCD_sendCommand(0x0C);
	_delay_ms(1);
	LCD_sendCommand(0x01);
	_delay_ms(2);
	LCD_sendCommand(0x06);
#elif (LCD_Mode == 4)
	LCD_sendCommand(0x33);
	_delay_ms(1);
	LCD_sendCommand(0x32);
	_delay_ms(1);
	LCD_sendCommand(0x28);
	_delay_ms(1);
	LCD_sendCommand(0x0C);
	_delay_ms(1);
	LCD_sendCommand(0x01);
	_delay_ms(2);
	LCD_sendCommand(0x06);
#endif
}
/*
 * Description:
 * Sending command to LCD in 4 or 8 bit mode
 * Clear RS pin
 * Set enable pin
 * send command
 * clear enable pin
 */
void LCD_sendCommand(uint8 Instruction) {

	GPIO_writePin(LCD_CTRL_PORT, LCD_RS, LOGIC_LOW);
	_delay_ms(1);

#if(LCD_Mode == 8)
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePort(LCD_DATA_PORT, Instruction);
	_delay_ms(1);

	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW);
	_delay_ms(1);
#elif (LCD_Mode==4)
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT(Instruction, 4));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT(Instruction, 5));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT(Instruction, 6));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT(Instruction, 7));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT(Instruction, 0));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT(Instruction, 1));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT(Instruction, 2));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT(Instruction, 3));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW); /* Disable LCD E=0 */
#endif
}

/*
 * Description:
 * print char on LCD
 * 1. Set RS pin
 * 2. Set enable pin
 * 3. send data
 * 4. clear enable pin
 */
void LCD_displayCharacter(uint8 data) {

	GPIO_writePin(LCD_CTRL_PORT, LCD_RS, LOGIC_HIGH);
	_delay_ms(1);

#if(LCD_Mode == 8)
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePort(LCD_DATA_PORT, data);
	_delay_ms(1);

	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW);
	_delay_ms(1);
#elif (LCD_Mode==4)
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT(data, 4));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT(data, 5));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT(data, 6));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT(data, 7));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT(data, 0));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT(data, 1));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT(data, 2));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT(data, 3));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	GPIO_writePin(LCD_CTRL_PORT, LCD_E, LOGIC_LOW); /* Disable LCD E=0 */
#endif
}

/*
 * Description:
 * Printing string on the LCD
 * Loop on the string
 * print each character with diplayCharacter function
 */
void LCD_displayString(uint8 *data) {
	uint8 i;
	for (i = 0; *(data + i) != '\0'; i++) {
		LCD_displayCharacter(*(data + i));
	}
}

void LCD_ClearDisplay() {
	LCD_sendCommand(0x01);
}

void LCD_SetCursor(uint8 row, uint8 col) {

	uint8 lcd_memory_address;

	/* Calculate the required address in the LCD DDRAM */
	switch (row) {
	case 0:
		lcd_memory_address = col;
		break;
	case 1:
		lcd_memory_address = col + 0x40;
		break;
	case 2:
		lcd_memory_address = col + 0x10;
		break;
	case 3:
		lcd_memory_address = col + 0x50;
		break;
	}
	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address | 0x80);
}

void LCD_PrintNum(sint32 num) {
	if (num < 0) {
		LCD_displayCharacter('-');
		num = -num;
	}
	if ((num * 100) % 100 != 0) {
		LCD_PrintNum((sint32) num);
		LCD_displayCharacter('.');
		LCD_PrintNum((num * 100) % 100);
	} else {
		sint32 num1 = num;
		uint16 L_u8Counter = 1;
		if (num < 10)
			LCD_displayCharacter(num + '0');
		else {
			while (num >= 10) {
				num /= 10;
				L_u8Counter *= 10;
			}
			while (L_u8Counter >= 1) {
				num = num1;
				num1 = num % L_u8Counter;
				num /= L_u8Counter;
				L_u8Counter /= 10;
				LCD_PrintNum(num);
			}

		}
	}
}

void LCD_PrintNumF(float32 num) {
	if (num < 0) {
		LCD_displayCharacter('-');
		num = -num;
	}
	if ((sint32)(num * 100) % 100 != 0) {
		LCD_PrintNumF((sint32) num);
		LCD_displayCharacter('.');
		LCD_PrintNumF((sint32)(num * 100) % 100);
	} else {
		float32 num1 = num;
		uint16 L_u8Counter = 1;
		if (num < 10)
			LCD_displayCharacter(num + '0');
		else {
			while (num >= 10) {
				num /= 10;
				L_u8Counter *= 10;
			}
			while (L_u8Counter >= 1) {
				num = num1;
				num1 =(sint32) num % L_u8Counter;
				num /= L_u8Counter;
				L_u8Counter /= 10;
				LCD_PrintNum(num);
			}

		}
	}
}

void LCD_createCustomCharacter(uint8 customValue[],uint8 index){
	LCD_sendCommand(0x40 + index * 8);
		for (index = 0; index < 8; index++) {
			LCD_displayCharacter(customValue[index]);
		}
		LCD_sendCommand(0x80);
}
void LCD_displayCustomCharacter(uint8 index){
	if (index < 8) {
		LCD_displayCharacter(index);
		}
}
