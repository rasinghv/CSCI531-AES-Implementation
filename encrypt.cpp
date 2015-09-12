#include <fstream>
#include <vector>
#include <iterator>
#include "encrypt.h"
#include "tablecheck.h"
#include "modprod.h"

using namespace std;

extern unsigned char* eKey;
extern unsigned char* Rcon;
extern unsigned char* S;
extern unsigned char* SInvert;
extern unsigned char* in;
extern unsigned char* out;
extern unsigned char** state;
extern unsigned char* poly;
extern unsigned char* polyInvert;

// Initialize round Rijndael constant and store in array rcon
void initRcon() {
	Rcon[0] = 0x8d;
	for (int i = 1; i < 255; i++) {
		Rcon[i] = multiply(Rcon[i - 1], 0x02);
	}
}

unsigned int getInvertTable(int index) {
// Hardcoded table of multiplicative inverses
	char* table_list; // = (char*)calloc(1, 256);
	table_list =
			(char*)
				"00018df6cb527bd1e84f29c0b0e1e5c7\
74b4aa4b992b605f583ffdccff40eeb2\
3a6e5af1554da8c9c10a98153044a2c2\
2c45926cf3396642f235206f77bb5919\
1dfe37672d31f569a764ab135425e909\
ed5c05ca4c2487bf183e22f051ec6117\
165eafd349a63643f44791df3393213b\
79b7978510b5ba3cb670d006a1fa8182\
837e7f809673be569b9e95d9f702b9a4\
de6a326dd88a84722a149f88f9dc899a\
fb7c2ec38fb8654826c8124acee7d262\
0ce01fef11757871a58e763dbdbc8657\
0b282fa3dad4e40fa92753041bfcace6\
7a07ae63c5dbe2ea948bc4d59df8906b\
b10dd6ebc60ecfad084ed7e35d501eb3\
5b2338346846038cdd9c7da0cd1a411c";

	// Read each pair of hexvals, store as ASCII vals in inv_table
	char temp_val[3];
	temp_val[0] = table_list[index * 2];
	temp_val[1] = table_list[index * 2 + 1];
	temp_val[2] = ' ';

	return strtol(temp_val, NULL, 16);
}

unsigned char inverse(char* p) {
	if (strlen(p) != 8) {
		cout << "ERROR: polynomial must consist of 8 characters, all hex values.\n";
		return 0;
	}

	for (unsigned int i = 0; i < 8; i++) {
		if (!isHexChar(p[i])) {
			cout << "ERROR: polynomial must consist of 8 characters, all hex values.\n";
			return 0;
		}
	}

	unsigned int poly[4];
	for (unsigned int i = 0; i < 4; i++) {
		poly[i] = getDecimalValueOfHex(p, 2, 2*i);
	}

	// Declare default arrays for a, b, remainder, quotient, aux
	unsigned char rem[6][5] = { { 0x01, 0x00, 0x00, 0x00, 0x01 }, { 0x00,
			poly[0], poly[1], poly[2], poly[3] } };
	unsigned char quo[6][4] = { { 0 } };
	unsigned char aux[6][4] = { { 0 }, { rem[0][1], rem[0][2], rem[0][3],
			rem[0][4] } };

	// Print first two rows of output (should be pretty much the same every time)
	printf("i=1, rem[i]={%02x}{%02x}{%02x}{%02x}, quo[i]={00}{00}{00}{00}, aux[i]={00}{00}{00}{00}\n",
			rem[0][1], rem[0][2], rem[0][3], rem[0][4]);

	printf("i=2, rem[i]={%02x}{%02x}{%02x}{%02x}, quo[i]={00}{00}{00}{00}, aux[i]={%02x}{%02x}{%02x}{%02x}\n",
			poly[0], poly[1], poly[2], poly[3], aux[1][0], aux[1][1], aux[1][2],
			aux[1][3]);

	// Iterate up to 4 more times, computing rem, quo, and aux; find inv_poly if applicable
	for (int i = 2; i < 6; i++) {

		
		// First half of quotient
		unsigned char temp_math[6 - i];
		quo[i][2] = multiply(getInvertTable(rem[i - 1][i - 1]), rem[i - 2][i - 2]);
		for (int j = 0; j < (6 - i); j++) {
			temp_math[j] = rem[i - 2][j + i - 1]
					^ multiply(rem[i - 1][j + i], quo[i][2]);
		}

		// Second half of quotient
		if (i < 5) {
			temp_math[5 - i] = rem[i - 2][4];
			quo[i][3] = multiply(getInvertTable(rem[i - 1][i - 1]), temp_math[0]);
		} else {
			// Find value of end of quotient that leads to remainder of 0x01
			quo[i][3] = multiply(getInvertTable(rem[i - 1][i - 1]), temp_math[0] ^ 0x01);
			rem[i][i - 1] = 0x01;
		}
		// Compute remainder from quotient and previous remainders
		for (int j = i; j < 5; j++) {
			rem[i][j] = temp_math[j - i + 1] ^ multiply(rem[i - 1][j], quo[i][3]);
		}
		// Compute aux array values
		// aux[i] = modprod(aux[i], aux[i - 1]);

		aux[i][3] = multiply(quo[i][3], aux[i - 1][3]) ^ multiply(quo[i][0], aux[i - 1][2]) ^ multiply(quo[i][1], aux[i - 1][1])
				^ multiply(quo[i][2], aux[i - 1][0]);
		aux[i][2] = multiply(quo[i][2], aux[i - 1][3]) ^ multiply(quo[i][3], aux[i - 1][2]) ^ multiply(quo[i][0], aux[i - 1][1])
				^ multiply(quo[i][1], aux[i - 1][0]);
		aux[i][1] = multiply(quo[i][1], aux[i - 1][3]) ^ multiply(quo[i][2], aux[i - 1][2]) ^ multiply(quo[i][3], aux[i - 1][1])
				^ multiply(quo[i][0], aux[i - 1][0]);
		aux[i][0] = multiply(quo[i][0], aux[i - 1][3]) ^ multiply(quo[i][1], aux[i - 1][2]) ^ multiply(quo[i][2], aux[i - 1][1])
				^ multiply(quo[i][3], aux[i - 1][0]);

		for (int j = 0; j < 4; j++) {
			aux[i][j] ^= aux[i - 2][j];
		}
		// Output current results
		printf(
				"i=%d, rem[i]={%02x}{%02x}{%02x}{%02x}, quo[i]={%02x}{%02x}{%02x}{%02x}, aux[i]={%02x}{%02x}{%02x}{%02x}\n",
				i + 1, rem[i][1], rem[i][2], rem[i][3], rem[i][4], quo[i][0],
				quo[i][1], quo[i][2], quo[i][3], aux[i][0], aux[i][1],
				aux[i][2], aux[i][3]);
		// Invalid coefficient, no inverse
		if (i != 5 && rem[i][i] == 0x00) {
			printf("{%02x}{%02x}{%02x}{%02x} does not have a multiplicative inverse.\n",
					poly[0], poly[1], poly[2], poly[3]);
			return 0;
		}
	}

	printf("Multiplicative inverse of {%02x}{%02x}{%02x}{%02x} is {%02x}{%02x}{%02x}{%02x}\n",
			poly[0], poly[1], poly[2], poly[3], aux[5][0], aux[5][1],
			aux[5][2], aux[5][3]);

	return 0;
}

void keyexpand(char* key, std::ifstream& file) {

	std::string line;
	int count = 0;
	while (std::getline(file, line)) {
		count++;
		if (count == 1) {
			char* data = strdup(line.substr(2, strlen(line.c_str()) - 2).c_str());

			for (int i = 0; i < 256; i++) {
				S[i] = getDecimalValueOfHex(data, 2, 2 * i);
			}
			delete data;
		} else if (count == 2) {
			char* P = strdup(line.substr(2, strlen(line.c_str()) - 2).c_str());
			if (strlen(P) >= 8) {
				for (int i = 0; i < 4; i++) {
					poly[i] = getDecimalValueOfHex(P, 2, 2 * i);
				}
				
			} else {
				cerr << P << endl;
			}
			delete P;
		} else if (count == 3) {
			char* P = strdup(line.substr(5, strlen(line.c_str()) - 5).c_str());
			if (strlen(P) >= 8) {
				for (int i = 0; i < 4; i++) {
					polyInvert[i] = getDecimalValueOfHex(P, 2, 2 * i);
				}
			} else {
				cerr << P << endl;
			}
			delete P;
		}
	}
	

	file.close();
	
	if (count != 3) {
		cerr << "Malformed table file!\n";
		return;
	}

	int i, j, k;
	unsigned char tempa[4]; // Used for the column/row operations

	// The first round key is the key itself.
	for (i = 0; i < Nk; i++) {
		eKey[i * 4] = getDecimalValueOfHex(key, 2, i * 8);
		eKey[i * 4 + 1] = getDecimalValueOfHex(key, 2, i * 8 + 2);
		eKey[i * 4 + 2] = getDecimalValueOfHex(key, 2, i * 8 + 4);
		eKey[i * 4 + 3] = getDecimalValueOfHex(key, 2, i * 8 + 6);

	}

	// init rCon
	initRcon();


	// All other round keys are found from the previous round keys.
	while (i < (Nb * (Nr + 1))) {
		for (j = 0; j < 4; j++) {
			tempa[j] = eKey[(i - 1) * 4 + j];
		}
		if (i % Nk == 0) {


			// This function rotates the 4 bytes in a word to the left once.
			// [a0, a1, a2, a3] becomes [a1, a2, a3, a0]

			// Function RotWord()
			k = tempa[0];
			tempa[0] = tempa[1];
			tempa[1] = tempa[2];
			tempa[2] = tempa[3];
			tempa[3] = k;


			// SubWord() is a function that takes a four-byte input word and
			// applies the S-box to each of the four bytes to produce an output
			// word.

			// Function Subword()
			tempa[0] = S[tempa[0]];
			tempa[1] = S[tempa[1]];
			tempa[2] = S[tempa[2]];
			tempa[3] = S[tempa[3]];



			tempa[0] = tempa[0] ^ Rcon[i / Nk];


		} else if (Nk > 6 && i % Nk == 4) {

			// Function Subword()
			tempa[0] = S[tempa[0]];
			tempa[1] = S[tempa[1]];
			tempa[2] = S[tempa[2]];
			tempa[3] = S[tempa[3]];


		}

		eKey[i * 4 + 0] = eKey[(i - Nk) * 4 + 0] ^ tempa[0];
		eKey[i * 4 + 1] = eKey[(i - Nk) * 4 + 1] ^ tempa[1];
		eKey[i * 4 + 2] = eKey[(i - Nk) * 4 + 2] ^ tempa[2];
		eKey[i * 4 + 3] = eKey[(i - Nk) * 4 + 3] ^ tempa[3];
		i++;


	}
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void addRoundKey(int round) {
	int i, j;
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] ^= eKey[round * Nb * 4 + i * Nb + j];
		}
	}
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void subBytes() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[i][j] = S[state[i][j]];
		}
	}
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void shiftRows() {
	unsigned char temp;

	// Rotate first row 1 columns to left
	temp = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp;

	// Rotate second row 2 columns to left
	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;

	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;

	// Rotate third row 3 columns to left
	temp = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = temp;
}

// MixColumns function mixes the columns of the state matrix
void mixColumns() {

	unsigned char* temp_col = new unsigned char[4];
	for (int i = 0; i < 4; i++) {
		
		temp_col[0] = multiply(state[0][i], poly[3] ) ^ multiply(poly[0] , state[1][i]) 
						^ multiply(poly[1] , state[2][i]) ^ multiply(poly[2] , state[3][i]);
		temp_col[1] = multiply(state[0][i], poly[2] ) ^ multiply(poly[3] , state[1][i]) 
						^ multiply(poly[0] , state[2][i]) ^ multiply(poly[1] , state[3][i]);
		temp_col[2] = multiply(state[0][i], poly[1] ) ^ multiply(poly[2] , state[1][i]) 
						^ multiply(poly[3] , state[2][i]) ^ multiply(poly[0] , state[3][i]);
		temp_col[3] = multiply(state[0][i], poly[0] ) ^ multiply(poly[1] , state[1][i]) 
						^ multiply(poly[2] , state[2][i]) ^ multiply(poly[3] , state[3][i]);	

		// printf("state[0][%d] = %02x * %02x ^ %02x * %02x ^ %02x * %02x ^ %02x * %02x = %02x\n", i, 
		//		state[0][i], poly[3] , state[1][i], poly[0] , state[2][i], poly[1] , state[3][i], poly[2] , temp_col[0]);

		state[0][i] = temp_col[0];
		state[1][i] = temp_col[1];
		state[2][i] = temp_col[2];
		state[3][i] = temp_col[3];
	}
}

std::vector<unsigned char> readFile(const char* filename) {
	// open the file:
	std::streampos fileSize;
	std::ifstream file(filename, std::ios::binary);

	// get its size:
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// read the data:
	std::vector<unsigned char> fileData(fileSize);
	file.read((char*) &fileData[0], fileSize);
	return fileData;
}

// Cipher is the main function that encrypts the PlainText.
void encrypt(char* key, std::ifstream& tablefile, FILE *file) {

	int size = fread(in, 1, 16, file);
	if (size == 1 || *in == '\n') {
		cerr << "Input file is incorrect!\n";
		return;
	}

	int count = 0;

	printf("round[%2d].input    ", count);
	for (int i = 0; i < 16; i++) {
		printf("%02x", in[i]);
	}
	cout << endl;

	printf("round[%2d].k_sch    %s\n", count, key);

	keyexpand(key, tablefile);
	int i, j, round = 0;

	// initialize state
	state = new unsigned char*[4];
	for (int c = 0; c < 4; c++) {
		state[c] = new unsigned char[Nb];
	}

	//Copy the input PlainText to state array.
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = in[i * 4 + j];
		}
	}

	// Add the First round key to the state before starting the rounds.
	addRoundKey(0);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for (round = 1; round < Nr; round++) {

		count++;

		// print state
		printf("round[%2d].start    ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		subBytes();

		printf("round[%2d].s_box    ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		shiftRows();

		// print state
		printf("round[%2d].s_row    ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		mixColumns();
		printf("round[%2d].m_col    ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		addRoundKey(round);

		printf("round[%2d].k_sch    ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", eKey[round * Nb * 4 + i * Nb + j]);
			}
		}
		cout << endl;
	}

	count++;

	printf("round[%2d].start    ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	subBytes();

	printf("round[%2d].s_box    ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	shiftRows();
	printf("round[%2d].s_row    ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	addRoundKey(Nr);
	printf("round[%2d].k_sch    ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", eKey[round * Nb * 4 + i * Nb + j]);
		}
	}
	cout << endl;

	printf("round[%2d].output   ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}

	// The encryption process is over.
	// Copy the state array to output array.
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			out[i * 4 + j] = state[j][i];
		}
	}
	
	cout << endl;
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void invSubBytes() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[i][j] = SInvert[state[i][j]];
		}
	}
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void invShiftRows() {
	unsigned char temp;

	// Rotate first row 1 columns to right
	temp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = temp;

	// Rotate second row 2 columns to right
	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;

	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;

	// Rotate third row 3 columns to right
	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
}

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the
// inexperienced.  Please use the references to gain more information.
void invMixColumns() {
		unsigned char* temp_col = new unsigned char[4];
	for (int i = 0; i < 4; i++) {
		
		temp_col[0] = multiply(state[0][i], polyInvert[3] ) ^ multiply(polyInvert[0] , state[1][i]) 
						^ multiply(polyInvert[1] , state[2][i]) ^ multiply(polyInvert[2] , state[3][i]);
		temp_col[1] = multiply(state[0][i], polyInvert[2] ) ^ multiply(polyInvert[3] , state[1][i]) 
						^ multiply(polyInvert[0] , state[2][i]) ^ multiply(polyInvert[1] , state[3][i]);
		temp_col[2] = multiply(state[0][i], polyInvert[1] ) ^ multiply(polyInvert[2] , state[1][i]) 
						^ multiply(polyInvert[3] , state[2][i]) ^ multiply(polyInvert[0] , state[3][i]);
		temp_col[3] = multiply(state[0][i], polyInvert[0] ) ^ multiply(polyInvert[1] , state[1][i]) 
						^ multiply(polyInvert[2] , state[2][i]) ^ multiply(polyInvert[3] , state[3][i]);	


		state[0][i] = temp_col[0];
		state[1][i] = temp_col[1];
		state[2][i] = temp_col[2];
		state[3][i] = temp_col[3];
	}
}

// InvCipher is the main function that decrypts the CipherText.
void decrypt(char* key, std::ifstream& tablefile, FILE* file) {
	
	int size = fread(in, 1, 16, file);
	if (size == 1 || *in == '\n') {
		cerr << "Input file is incorrect!\n";
		return;
	}

	int count = 0;
	
	keyexpand(key, tablefile);

	printf("round[%2d].iinput   ", count);
	for (int i = 0; i < 16; i++) {
		printf("%02x", in[i]);
	}
	cout << endl;

	printf("round[%2d].ik_sch   ", count);
	for (int i = 0; i < Nb; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%02x", eKey[Nr * Nb * 4 + i * Nb + j]);
		}
	}
	cout << endl;

	// get SInvert
	for (int i = 0; i < 256; i++) {
		SInvert[S[i]] = i;
	}

	int i, j, round = 0;
	count = 0;

	// initialize state
	state = new unsigned char*[4];
	for (int c = 0; c < 4; c++) {
		state[c] = new unsigned char[Nb];
	}

	//Copy the input CipherText to state array.
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = in[i * 4 + j];
		}
	}

	// Add the First round key to the state before starting the rounds.
	addRoundKey(Nr);

	// There will be Nr rounds.
	// The first Nr-1 rounds are identical.
	// These Nr-1 rounds are executed in the loop below.
	for (round = Nr - 1; round > 0; round--) {

		count++;

		// print state
		printf("round[%2d].istart   ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		invShiftRows();

		// print state
		printf("round[%2d].is_row   ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		invSubBytes();

		// print state
		printf("round[%2d].is_box   ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		addRoundKey(round);

		printf("round[%2d].ik_sch   ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", eKey[round * Nb * 4 + i * Nb + j]);
			}
		}
		cout << endl;

		// print state
		printf("round[%2d].ik_add   ", count);
		for (i = 0; i < Nb; i++) {
			for (j = 0; j < 4; j++) {
				printf("%02x", state[j][i]);
			}
		}
		cout << endl;

		invMixColumns();
	}

	count++;

	// print state
	printf("round[%2d].istart   ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	// The last round is given below.
	// The MixColumns function is not here in the last round.
	invShiftRows();

	// print state
	printf("round[%2d].is_row   ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	invSubBytes();

	// print state
	printf("round[%2d].is_box   ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	printf("round[%2d].ik_sch   ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", eKey[round * Nb * 4 + i * Nb + j]);
		}
	}
	cout << endl;

	addRoundKey(0);

	// print state
	printf("round[%2d].ioutput  ", count);
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			printf("%02x", state[j][i]);
		}
	}
	cout << endl;

	// The decryption process is over.
	// Copy the state array to output array.
	for (i = 0; i < Nb; i++) {
		for (j = 0; j < 4; j++) {
			out[i * 4 + j] = state[j][i];
		}
	}
}
