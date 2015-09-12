#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include <iostream>
#include "utilities.h"

using namespace std;

// The number of columns comprising a state in AES
#define Nb 4

// The number of 32 bit words in a key.
#define Nk 4

// Key length in bytes [128 bit]
#define KEYLEN 16

// The number of rounds in AES Cipher.
#define Nr 10

// initialize Rcon
extern void initRcon();

// inverse
unsigned char inverse(char* a);

extern void keyexpand(char* key, std::ifstream& file);

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
extern void addRoundKey(unsigned char round);

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
extern void subBytes();

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
extern void shiftRows();

// MixColumns function mixes the columns of the state matrix
extern void mixColumns();

// Cipher is the main function that encrypts the PlainText.
extern void encrypt(char* key, std::ifstream& tablefile, FILE *file);

extern int fillBlock(int sz, char *str, unsigned char *in);

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
extern void invSubBytes();

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
extern void invShiftRows();

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the
// inexperienced.  Please use the references to gain more information.
extern void invMixColumns();

// InvCipher is the main function that decrypts the CipherText.
extern void decrypt(char* key, std::ifstream& tablefile, FILE *file);

#endif //__ENCRYPT_H__
