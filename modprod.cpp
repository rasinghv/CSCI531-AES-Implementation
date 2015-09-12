#include "modprod.h"

unsigned char modprod(char* poly1, char* poly2) {

	// validate input parameters
	if (strlen(poly1) != 8 || strlen(poly2) != 8) { // length is not 8
		cerr << "The poly should have length 8\n";
		return -1;
	} else if (!isHexNumber(poly1) || !isHexNumber(poly2)) { // not a hex number
		cerr << "The poly should have length 8\n";
		return -1;
	}

	unsigned char a[4], b[4];
	for (unsigned int i = 0; i < 4; i++) {
		a[i] = getDecimalValueOfHex(poly1, 2, 2 * i);
		b[i] = getDecimalValueOfHex(poly2, 2, 2 * i);
	}

	unsigned char d[4];
	d[3] = multiply(a[3], b[3]) ^ multiply(a[0], b[2]) ^ multiply(a[1], b[1])
			^ multiply(a[2], b[0]);
	d[2] = multiply(a[2], b[3]) ^ multiply(a[3], b[2]) ^ multiply(a[0], b[1])
			^ multiply(a[1], b[0]);
	d[1] = multiply(a[1], b[3]) ^ multiply(a[2], b[2]) ^ multiply(a[3], b[1])
			^ multiply(a[0], b[0]);
	d[0] = multiply(a[0], b[3]) ^ multiply(a[1], b[2]) ^ multiply(a[2], b[1])
			^ multiply(a[3], b[0]);

	printf("{%02x}{%02x}{%02x}{%02x} CIRCLEX {%02x}{%02x}{%02x}{%02x} = {%02x}{%02x}{%02x}{%02x}\n"
			, a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3], d[0], d[1], d[2], d[3]);

	// printf("%02x\n", multiply(n1, n2));
	return 1;
}

unsigned char modprod(char* poly1, char* poly2, bool print) {

	// validate input parameters
	if (strlen(poly1) != 8 || strlen(poly2) != 8) { // length is not 8
		cerr << "The poly should have length 8\n";
		return -1;
	} else if (!isHexNumber(poly1) || !isHexNumber(poly2)) { // not a hex number
		cerr << "The poly should have length 8\n";
		return -1;
	}

	unsigned char a[4], b[4];
	for (unsigned int i = 0; i < 4; i++) {
		a[i] = getDecimalValueOfHex(poly1, 2, 2 * i);
		b[i] = getDecimalValueOfHex(poly2, 2, 2 * i);
	}

	unsigned char d[4];
	d[3] = multiply(a[3], b[3]) ^ multiply(a[0], b[2]) ^ multiply(a[1], b[1])
			^ multiply(a[2], b[0]);
	d[2] = multiply(a[2], b[3]) ^ multiply(a[3], b[2]) ^ multiply(a[0], b[1])
			^ multiply(a[1], b[0]);
	d[1] = multiply(a[1], b[3]) ^ multiply(a[2], b[2]) ^ multiply(a[3], b[1])
			^ multiply(a[0], b[0]);
	d[0] = multiply(a[0], b[3]) ^ multiply(a[1], b[2]) ^ multiply(a[2], b[1])
			^ multiply(a[3], b[0]);

	if (print) printf("{%02x}{%02x}{%02x}{%02x} CIRCLEX {%02x}{%02x}{%02x}{%02x} = {%02x}{%02x}{%02x}{%02x}\n"
			, a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3], d[0], d[1], d[2], d[3]);

	// printf("%02x\n", multiply(n1, n2));
	return 1;
}
