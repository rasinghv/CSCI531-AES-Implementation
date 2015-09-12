#ifndef __TABLECHECK_H__
#define __TABLECHECK_H__

#include <iostream>
#include <string.h>

#include "utilities.h"

using namespace std;

// This table contains a random permutation of the values from 00 through ff, inclusive.
// (Need to compute the inverse transform in your code.)
extern bool checkS(string line);

// This table contains a hexstring of length 8 which corresponds to 4 byte of binary values.
// Each byte is a coefficient of the a(x) polynomial in the MixColumns() transformation.
// The first two hex digits correspond to the coefficient for x3, the next two hex digits
// correspond to the coefficient for x2, etc.
// This table contains a hexstring of length 8 which corresponds to 4 byte of binary values.
// Each byte is a coefficient of the a-1(x) polynomial in the InvMixColumns() transformation.
// The first two hex digits correspond to the coefficient for x3, the next two hex digits correspond
// to the coefficient for x2, etc.
extern bool checkPnINVP(string line1, string line2);

// Check the integrity of table file
extern bool tableCheck(std::ifstream& file);

#endif /* TABLECHECK_H_ */
