Name: Rahul Singhvi
USC ID - 5635013503

Homework 6 - AES (Advanced Encryption Standard)
1. Use
- Compile with:
make hw6
- Check the integrity of tablefile
./hw6 tablecheck -t=tablefile
- Perform modular product (modulo {01}x^4+{01}) of two polynomials with coefficients in GF(2^8)
./hw6 modprod -p1=poly1 -p2=poly2
- Perform the key expansion algorithm for AES-128 using key as the 16-byte cipher key in hexstring format
./hw6 keyexpand -k=key -t=tablefile
- Encrypts the first 16 bytes of the input using AES-128 driven by tablefile in the ECB mode using key as the encryption key
./hw6 encrypt -k=key -t=tablefile [file]
- Decrypts the first 16 bytes of the input using AES-128 driven by tablefile in the ECB mode using key as the encryption key
./hw6 decrypt -k=key -t=tablefile [file]
- Calculate the multiplicative inverse (modulo {01}x^4+{01}) of poly with GF(2^8) coefficients using extended Euclidean algorithm
./hw6 inverse -p=poly
- Clean binary files with:
make clean

2. Design
- Tablecheck
- Modprod
- Keyexpand
- Encrypt
- Decrypt
- Inverse

3. Known Bugs
- None known

4. Deviation from the specs
- None known