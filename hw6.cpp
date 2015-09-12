#include <iostream>
#include <fstream>
using namespace std;

#include "tablecheck.h"
#include "modprod.h"
#include "encrypt.h"

unsigned char* eKey = new unsigned char[4 * 4 * (10 + 1)];
unsigned char* Rcon = new unsigned char[255];
unsigned char* S = new unsigned char[256];
unsigned char* SInvert = new unsigned char[256];
unsigned char* poly = new unsigned char[4];
unsigned char* polyInvert = new unsigned char[4];

// in[1024], out[1024], state[4][Nb];
unsigned char* in = new unsigned char[16];
unsigned char* out = new unsigned char[16];
unsigned char** state = NULL;

void usage(const char *type = "none") {
	if (strcmp(type, "none") == 0) {
		cerr << "ERROR: invalid parameters. Should be one of the following:\n";
		cerr << "\thw6 tablecheck -t=tablefile\n";
		cerr << "\thw6 modprod -p1=poly1 -p2=poly2\n";
		cerr << "\thw6 keyexpand -k=key -t=tablefile\n";
		cerr << "\thw6 encrypt -k=key -t=tablefile [file]\n";
		cerr << "\thw6 decrypt -k=key -t=tablefile [file]\n";
		cerr << "\thw6 inverse -p=poly\n";
	} else {
		cerr << "ERROR: invalid parameters. Should be the following:\n";
		if (strcmp(type, "tablecheck") == 0) {
			cerr << "\thw6 tablecheck -t=tablefile\n";
		} else if (strcmp(type, "modprod") == 0) {
			cerr << "\thw6 modprod -p1=poly1 -p2=poly2\n";
		} else if (strcmp(type, "keyexpand") == 0) {
			cerr << "\thw6 keyexpand -k=key -t=tablefile\n";
		} else if (strcmp(type, "encrypt") == 0) {
			cerr << "\thw6 encrypt -k=key -t=tablefile [file]\n";
		} else if (strcmp(type, "decrypt") == 0) {
			cerr << "\thw6 decrypt -k=key -t=tablefile [file]\n";
		} else if (strcmp(type, "inverse") == 0) {
			cerr << "\thw6 inverse -p=poly\n";
		}
	}
	return;
}

// Main method of HW6, mostly taking user input and running the correct prog
int main(int argc, char *argv[]) {
// Look for correct command line arguments
	if (argc <= 1) {
		usage();
		return 0;
	} else {
		if (strcmp(argv[1], "tablecheck") == 0) { // Tablecheck prog should be run

			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-t=", 3) == 0) {
					std::ifstream infile(argv[i] + 3);

					if (infile) {
						tableCheck(infile);
					} else {
						usage("tablecheck");
					}
				}
			}
		} else if (strcmp(argv[1], "modprod") == 0) {
			// Modprod prog should be run
			char *poly1 = NULL;
			char *poly2 = NULL;
			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-p1=", 4) == 0) {
					poly1 = argv[i] + 4;
				} else if (strncmp(argv[i], "-p2=", 4) == 0) {
					poly2 = argv[i] + 4;
				}
			}
			if (poly1 && poly2) {
				modprod(poly1, poly2);
			} else {
				usage("modprod");
			}
		} else if (strcmp(argv[1], "keyexpand") == 0) {
			// Keyexpand prog should be run
			char *key = NULL;
			std::ifstream infile;

			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-k=", 3) == 0) {
					key = strdup(argv[i] + 3);
				} else if (strncmp(argv[i], "-t=", 3) == 0) {
					infile.open(argv[i] + 3);
				}
			}

			if (key && infile) {
				keyexpand(key, infile);
				
				for (int i = 0; i < 44; i++) {
					printf("w[%2d]: %02x%02x%02x%02x\n", i, eKey[4 * i], eKey[4 * i + 1], eKey[4 * i + 2], eKey[4 * i + 3]);
				}

			} else {
				usage("keyexpand");
			}
		} else if (strcmp(argv[1], "encrypt") == 0) {
			char* key = NULL;
			std::ifstream tablefile;

			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-k=", 3) == 0) {
					key = strdup(argv[i] + 3);
				} else if (strncmp(argv[i], "-t=", 3) == 0) {
					tablefile.open(argv[i] + 3);
					if (!tablefile) {
						cerr << "ERROR: invalid file '" << argv[i] + 3
								<< "' for reading.\n";
						return 0;
					}
				}
			}

			if (key && tablefile) {
				if (argv[argc - 1][0] != '-') {
					
					FILE *file = fopen(argv[argc - 1], "r");
					if (file == NULL) {
						cerr << "ERROR: invalid file '" << argv[argc - 1] << "' for reading.\n";
						return 0;
					}
					
					// Take input from file
					encrypt(key, tablefile, file);
				} else {
					encrypt(key, tablefile, stdin);
				}
			} else {
				usage("encrypt");
			}
		} else if (strcmp(argv[1], "decrypt") == 0) {
			char* key = NULL;
			std::ifstream tablefile;

			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-k=", 3) == 0) {
					key = strdup(argv[i] + 3);
				} else if (strncmp(argv[i], "-t=", 3) == 0) {
					tablefile.open(argv[i] + 3);
					if (!tablefile) {
						cerr << "ERROR: invalid file '" << argv[i] + 3
								<< "' for reading.\n";
						return 0;
					}
				}
			}

			if (key && tablefile) {
				if (argv[argc - 1][0] != '-') {
					
					FILE *file = fopen(argv[argc - 1], "r");
					if (file == NULL) {
						cerr << "ERROR: invalid file '" << argv[argc - 1] << "' for reading.\n";
						return 0;
					}
					
					// Take input from file
					decrypt(key, tablefile, file);
				} else {
					decrypt(key, tablefile, stdin);
				}
			} else {
				usage("decrypt");
			}
		} else if (strcmp(argv[1], "inverse") == 0) {
			// Inverse prog should be run
			char *poly = NULL;
			for (int i = 2; i < argc; i++) {
				if (strncmp(argv[i], "-p=", 3) == 0) {
					poly = strdup(argv[i] + 3);
				}
			}
			if (poly) {
				inverse(poly);
			} else {
				usage("inverse");
			}
		} else {
			// Failed input
			usage();
		}
	}
	return 0;
}
