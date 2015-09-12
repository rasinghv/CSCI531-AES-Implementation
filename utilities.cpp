#include "utilities.h"

// Check whether an input character digit
bool isHexDigit(char c) {
	return c >= '0' && c <= '9';
}

// Check whether an input character symbol
bool isHexSymbol(char c) {
	return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// Check whether an input character hex-char
bool isHexChar(char c) {
	return isHexDigit(c) || isHexSymbol(c);
}

// Check whether an input hex number
bool isHexNumber(char* input) {
	for (unsigned int i = 0; i < strlen(input); i++) {
		if (!isHexChar(input[i])) return false;
	}
	return true;
}

// get decimal value of hex chars
unsigned char getDecimalValueOfHex(char array[], int len, int start) {

	// initialize value
	unsigned char value = 0;

	// compute the value
	for (int i = 0; i < len; i++) {

		// get char by char
		char c = array[start + i];

		// update value
		if (isHexDigit(c)) {
			value = value * 16 + c - '0';
		} else if (isHexChar(c)) {
			value = value * 16 + c - 'a' + 10;
		} else {
			cerr << "Character " << c << " isn't hexchar!\n";
			return -1;
		}
	}

	// return
	return value;
}

// multiply 2 hex-char
unsigned char multiply(unsigned char a, unsigned char b) {

   unsigned char c = 0;
   unsigned char d = b;

   for (int i=0 ; i < 8 ; i++) {
      if (a%2 == 1) c ^= d;
      a /= 2;
      d = xtime(d);
   }
   return c;
}
