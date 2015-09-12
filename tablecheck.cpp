#include "tablecheck.h"
#include "modprod.h"
#include <fstream>
#include <iostream>

using namespace std;

// This table contains a random permutation of the values from 00 through ff, inclusive.
// (Need to compute the inverse transform in code.)
bool checkS(string line) {
	if (line.size() != 2 + 256 * 2) {
		cerr << "S line in invalid!\n";
		return false;
	}

	char array[line.size() + 1]; //as 1 char space for null is also required
	strcpy(array, line.c_str());

	if (array[0] != 'S' || array[1] != '=') {
		cerr << "S line is in incorrect format!\n";
		return false;
	}

	int decimalArray[256];
	for (int i = 0; i < 256; i++) {
		decimalArray[i] = -1;
	}

	for (unsigned int i = 2; i < line.size(); i = i + 2) {
		int dec = getDecimalValueOfHex(array, 2, i);
		if (decimalArray[dec] != -1) {
			cerr << "S line is in incorrect format, the hex at " << i << " " << array[i] << array[i + 1] << " appears more than one time!\n";
			return false;
		} else {
			decimalArray[dec] = 1;
		}
	}

	return true;
}

// This table contains a hexstring of length 8 which corresponds to 4 byte of binary values.
// Each byte is a coefficient of the a(x) polynomial in the MixColumns() transformation.
// The first two hex digits correspond to the coefficient for x3, the next two hex digits
// correspond to the coefficient for x2, etc.
// This table contains a hexstring of length 8 which corresponds to 4 byte of binary values.
// Each byte is a coefficient of the a-1(x) polynomial in the InvMixColumns() transformation.
// The first two hex digits correspond to the coefficient for x3, the next two hex digits correspond
// to the coefficient for x2, etc.
bool checkPnINVP(string line1, string line2) {

	// cout << line1 << endl;
	// cout << line2 << endl;

	char* a = (char*) line1.c_str();
	char* b = (char*) line2.c_str();
	return modprod(a, b, false) == 1;
}

// Check the integrity of table file
bool tableCheck(std::ifstream& file) {
	std::string line;
	int count = 0;
	char *P, *INVP, *S;
	while (std::getline(file, line)) {
		count++;
		if (count == 1) {
			S = strdup(line.substr(2, strlen(line.c_str()) - 2).c_str());
			if (strlen(S) != 512) {
				cerr << "Invalid S-box, wrong number of entries!\n";
				return false;
			}
			
			bool arr[256];
			for (int i = 0; i < 256; i++) {
				arr[i] = false;
			}
			
			for (int i = 0; i < 256; i++) {
				int value = getDecimalValueOfHex(S, 2, 2 * i);
				if (arr[value] == true) {
					cerr << "Invalid S-box,!\n";
					return false;
				}
				arr[value] = true;
			}
		}
		if (count == 2) {
			P = strdup(line.substr(2, strlen(line.c_str()) - 2).c_str());
		} else if (count == 3) {
			INVP = strdup(line.substr(5, strlen(line.c_str()) - 5).c_str());
			return checkPnINVP(P, INVP);
		}
	}
	
	if (count != 3) {
		cerr << "The input file should have 3 lines!\n";
	}
	
	file.close();
	return false;
}
