/*
 * main.c
 *
 *  Created on: Oct 2, 2023
 *      Author: Mahmoud Khalafallah
 */

#include "LCD/lcd.h"
#include "KEYPAD/keypad.h"
#include "common_macros.h"
#include "GPIO/gpio.h"
#include <util/delay.h>

/*
 * Description:
 * function to make a basic operation "/,*,+,-" on two numbers and returns the result
 */
static float32 calculate(float32 firstNumber, uint8 operand,
		float32 secondNumber) {
	float32 result;
	switch (operand) {
	case '+':
		result = firstNumber + secondNumber;
		break;
	case '*':
		result = firstNumber * secondNumber;
		break;
	case '-':
		result = firstNumber - secondNumber;
		break;
	case '/':
		result = firstNumber / secondNumber;
		break;
	default:
		result = 'E';
	}
	return result;
}

static void errorBuzzer() {
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_HIGH);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_HIGH);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_HIGH);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_HIGH);
	_delay_ms(50);
	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
}

static void printMyName() {
	LCD_displayCustomCharacter(4);
	LCD_displayCustomCharacter(0);
	LCD_displayCustomCharacter(1);
	LCD_displayCustomCharacter(0);
	LCD_displayString(" ");
	LCD_displayCustomCharacter(4);
	LCD_displayCustomCharacter(3);
	LCD_displayCustomCharacter(0);
	LCD_displayCustomCharacter(2);
	LCD_displayCustomCharacter(1);
	LCD_displayCustomCharacter(0);
}
static void loading() {
	uint8 i = 5;
	while (i--) {
		LCD_SetCursor(1, 4);
		LCD_displayCharacter('.');
		_delay_ms(100);
		LCD_SetCursor(1, 3);
		LCD_displayCharacter('.');
		_delay_ms(100);
		LCD_SetCursor(1, 2);
		LCD_displayCharacter('.');
		_delay_ms(100);
		LCD_SetCursor(1, 2);
		LCD_displayCharacter(' ');
		_delay_ms(100);
		LCD_SetCursor(1, 3);
		LCD_displayCharacter(' ');
		_delay_ms(100);
		LCD_SetCursor(1, 4);
		LCD_displayCharacter(' ');
		_delay_ms(100);
	}
}
int main(void) {
	LCD_Init();
	GPIO_setupPinDirection(PORTC_ID, PIN5_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORTC_ID, PIN0_ID, PIN_OUTPUT);

	GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
	GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_LOW);

	uint8 char1m[] = { 31, 17, 21, 1, 31, 31, 31 };
	uint8 char27[] = { 31, 17, 29, 1, 31, 31, 31 };
	uint8 char3[] = { 31, 31, 31, 0, 31, 31, 31 };
	uint8 char4w[] = { 31, 17, 21, 16, 29, 17, 31 };
	uint8 char5d[] = { 31, 29, 29, 17, 31, 31, 31 };

	LCD_createCustomCharacter(char1m, 0);
	LCD_createCustomCharacter(char27, 1);
	LCD_createCustomCharacter(char3, 2);
	LCD_createCustomCharacter(char4w, 3);
	LCD_createCustomCharacter(char5d, 4);

	LCD_SetCursor(0, 2);
	LCD_displayString("Not a Simple");
	LCD_SetCursor(1, 3);
	LCD_displayString("calculator");
	_delay_ms(1000);
	LCD_ClearDisplay();
	LCD_SetCursor(0, 3);
	LCD_displayString("Made By :");
	LCD_SetCursor(1, 5);
	printMyName();
	loading();
	LCD_ClearDisplay();
	/*array to store input numbers and operations sequentially*/
	float32 equationArray[20];
	/*variable for input of keypad*/
	uint8 keypadInput;
	/*variable to store the result*/
	float32 result = 0;
	/*variable for index of operations and numbers stored in the array*/
	uint8 indexCounter = 0;
	/* flags to control errors*/
	uint8 validFlag = TRUE, syntaxErrorFlag = FALSE,
			numberOfConsequativeOperators = 0, opertorsErrorFlag = FALSE;

	while (1) {
		/*takes the input from the keypad*/
		keypadInput = KEYPAD_getPressedKey();
		/*depending on the input a certain process is done*/
		switch (keypadInput) {
		case '+':
		case '-':
		case '*':
		case '/':
			/*if the input is an operator:
			 * Sets the syntax error flag to true as if it is the last input the controller outputs an error message
			 * increments the operators count to detect if there are any consecutive operators
			 * if there are any consecutive operators it sets operators flag to true to outputs an error message
			 * increases the array index counter and stores the operator in the equation array
			 * displays the input on the LCD
			 * increases the array index counter again to be ready for the number input
			 */
			if (result != 0) {
				LCD_ClearDisplay();
				LCD_PrintNumF(result);
				result = 0;
			}
			syntaxErrorFlag = TRUE;
			numberOfConsequativeOperators++;
			if (numberOfConsequativeOperators > 1) {
				opertorsErrorFlag = TRUE;
				GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_HIGH);
			}
			if (equationArray[indexCounter] == 0
					&& equationArray[indexCounter - 1] == '/')
				GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_HIGH);
			indexCounter++;
			LCD_displayCharacter(keypadInput);
			equationArray[indexCounter] = keypadInput;
			_delay_ms(10);
			indexCounter++;
			break;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			/*
			 * if the input is a number:
			 * resets the operators counter and syntax error flag as the equation doesn't end with an operator
			 * to make a multi-digit number with single digits:
			 * multiplies the value in the array index by 10 and adds the input digit to it
			 * equationArray[2] = 1 input = 2 multiplies 1*10 = 10 and adds 1 10+1 = 11
			 * displays the input on the LCD
			 */
			if(result!=0){
				LCD_ClearDisplay();
				result = 0;
				equationArray[0]=0;
				indexCounter =0 ;

			}
			numberOfConsequativeOperators = 0;
			syntaxErrorFlag = FALSE;
			equationArray[indexCounter] *= 10;
			equationArray[indexCounter] += keypadInput;
			LCD_PrintNum(keypadInput);
			_delay_ms(10);
			break;
		case 13:
			/*
			 * if the user presses clear :
			 * clears the display
			 * resets all flags and values
			 * resets the equationArray
			 */
			LCD_ClearDisplay();
			indexCounter = 0;
			result = 0;
			validFlag = 1, syntaxErrorFlag = FALSE, numberOfConsequativeOperators =
					0, opertorsErrorFlag = FALSE;
			for (uint8 i = 0; i < 20; i++) {
				equationArray[i] = 0;
			}
			GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
			GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_LOW);
			break;
		case '=':
			/*
			 * if the user presses equal :
			 * checks for for syntax errors and if there is any outputs error message
			 * checks if there is any division by 0 and if there is any outputs error message
			 * if there are no errors:
			 * loops on the equation array execute all / and * operations and stores the + - rest equation in the array  :
			 * searches for any * or / operations and executes this single operation and stores the result at the previous index
			 * and shifts all the next element two indices to the left as we have removed the two numbers and operator and replaced them with there result
			 * sets the loop counter to be on the result element
			 * loops again and executes the + - operations to calculate the final result
			 * outputs the result and resets all values
			 */
			if (syntaxErrorFlag == TRUE || opertorsErrorFlag == TRUE) {
				LCD_ClearDisplay();
				LCD_SetCursor(1, 1);
				LCD_displayString("Syntax Error");
				errorBuzzer();
				indexCounter = 0;
				for (uint8 i = 0; i < 20; i++) {
					equationArray[i] = 0;
				}
				validFlag = FALSE, syntaxErrorFlag = FALSE;
				numberOfConsequativeOperators = 0;
				opertorsErrorFlag = FALSE;
				break;
			}
			for (uint8 i = 0; i <= indexCounter; i++) {
				if (equationArray[i] == '/' && equationArray[i + 1] == 0) {
					LCD_ClearDisplay();
					LCD_SetCursor(0, 0);
					LCD_displayString("Error");
					LCD_SetCursor(1, 0);
					LCD_displayString("divide by zero");
					errorBuzzer();
					indexCounter = 0;
					for (uint8 i = 0; i < 20; i++) {
						equationArray[i] = 0;
					}
					validFlag = FALSE, syntaxErrorFlag = FALSE;
					break;
				}
				/* loops on the equation array execute all / and * operations and stores the + - rest equation in the array  :*/
				if (equationArray[i] != '*' && equationArray[i] != '/')
					continue;
				else {
					equationArray[i - 1] = calculate(equationArray[i - 1],
							equationArray[i], equationArray[i + 1]);
					for (uint8 var = i; var <= indexCounter; ++var) {
						equationArray[var] = equationArray[var + 2];
					}
					indexCounter -= 2;
					i--;
				}
			}
			result = equationArray[0];
			for (uint8 i = 1; i <= indexCounter; i += 2) {
				result = calculate(result, equationArray[i],
						equationArray[i + 1]);
			}
			if (validFlag == 1) {
				LCD_SetCursor(1, 0);
				LCD_displayString("result: ");
				LCD_PrintNumF(result);
				indexCounter = 0;
				for (uint8 i = 0; i < 10; i++) {
					equationArray[i] = 0;
				}
				equationArray[0] = result;
				validFlag = TRUE, syntaxErrorFlag = FALSE, opertorsErrorFlag =
				FALSE, numberOfConsequativeOperators = 0;
				GPIO_writePin(PORTC_ID, PIN5_ID, LOGIC_LOW);
				GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_LOW);
			}
			break;
		}
	}
}

