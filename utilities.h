#ifndef __UTILITIES_H__
#define __UTILITIES_H__

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}
#define xtime(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b))

#include <iostream>

using namespace std;

// Check whether an input character digit
extern bool isHexDigit(char c);

// Check whether an input character symbol
extern bool isHexSymbol(char c);

// Check whether an input hex number
bool isHexNumber(char* input);

// get decimal value of hex chars
extern unsigned char getDecimalValueOfHex(char array[], int len, int start);

// Check whether an input character hex-char
extern bool isHexChar(char c);

// multiply 2 hex-char
extern unsigned char multiply(unsigned char a, unsigned char b);

// convert decimal to hex

#endif //__UTILITIES_H__
