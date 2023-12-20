/*******************************************************************************
 * Author:  Jose (Pillo) G. Duenas-Lopez
 * Version: 1.0 (DEC 06 2023)
 *
 * Purpose:
 *          SHA256 Cryptographic Hashing Function using C++
 *
 * References:
 *          http://dx.doi.org/10.6028/NIST.FIPS.180-4
 *          FIPS PUB 180-4: Secure Hash Standard (SHS)
 *
 *          https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c
 *          Stole Macros from here, came in clutch durring Hash Computation
 *
 *          https://www.youtube.com/watch?v=orIgy2MjqrA&t=3s
 *          Broke FIPS PUB 180-4 into plain English
 *
 *          https://sha256algorithm.com/
 *          Showed expected output for every step of code
 *
 ******************************************************************************/


/* LIBRARIES  *****************************************************************/
#include <iostream>     /* For in&output                                      */
#include <cstdint>
#include <cstring>
#include <iomanip>


/* DATATYPES  *****************************************************************/
typedef uint8_t BYTE;
typedef uint32_t WORD;

/* MACROS  ********************************************************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))



/* SETTING GLOBAL VARs  *******************************************************/
using namespace std;

/* FIPS PUB 180-4, 5.3.3: Setting the Initial Hash Value                      */
WORD H[8]=
{
  0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
  0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
};

/* FIPS PUB 180-4, 4.2.2: Constants                                           */
static const WORD K[64]=
{
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
  0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,

  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
  0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,

  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
  0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,

  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
  0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,

  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
  0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,

  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
  0xd192e819,0xd6990624,0xf40e3585,0x106aa070,

  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
  0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,

  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
  0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};


/* MISC FUNCTIONS *************************************************************/

void printBinary(const BYTE *binaryData, size_t length)
{
  for (size_t i = 0; i < length; ++i)
  {
    for (int j = 7; j >= 0; --j)
      cout << static_cast<int>((binaryData[i] >> j) & 0x01);

    if ((i + 1) % 4 == 0)
      cout << "\n";
  }
  cout << endl;
}


void printHex(const BYTE *binaryData, size_t length)
{
  for (size_t i = 0; i < length; ++i)
    cout << hex << setw(2) << setfill('0') << static_cast<int>(binaryData[i]);
  cout << dec << endl;
}


/* PREPROCESSING *************************************************************/

/* FIPS PUB 180-4, 5.1.1: Padding the Message                                 */
void addPadding(const BYTE *message, size_t message_len, BYTE *padded_message, size_t *padded_len)
{
  *padded_len = 64;

//Calculated from FIPS PUB page 5.1.1, determines how many zeros are needed
  size_t padded_size = 64 - ((message_len + 9) % 64);

//Checks if there is enough space
  if (message_len + padded_size > 64)
    return;

//Copies message to padding
  memcpy(padded_message, message, message_len);

//Appends the '1' bit
  padded_message[message_len] = 0x80;

//Appends the '0' bits
  memset(padded_message + message_len + 1, 0, padded_size - 9);

//Appends original message length checksum (in bits)
  for (size_t i = 0; i < 8; ++i)
    padded_message[56 + i] = static_cast<BYTE>((message_len * 8 ) >> (56 - i * 8));
}


/* HASH COMPUTATION ***********************************************************/

/* FIPS PUB 180-4, 6.2.2: Hash Computation                                    */
void messageSchedule(const BYTE *padded_message, WORD *W)
{
  for (size_t i = 0; i < 64; ++i)
  {

//Copies the first 16 words directly from the padded message
    if( i < 16 )
    {
      W[i] =
        (static_cast<WORD>(padded_message[i * 4 + 3]) << 24) |
        (static_cast<WORD>(padded_message[i * 4 + 2]) << 16) |
        (static_cast<WORD>(padded_message[i * 4 + 1]) << 8)  |
        (static_cast<WORD>(padded_message[i * 4 ]));
    }
//Performs math stuff, refer to 6.2.2 and MACRO's for more information
    else
      W[i] = ( SIG1(W[i - 2]) + W[i - 7] + SIG0(W[i - 15]) + W[i - 16] ) & 0xFFFFFFFF;
  }
}


/* FIPS PUB 180-4, 6.2.2: Hash Computation                                    */
void getHash( WORD *message_sched, BYTE *hash)
{
//Initialize working variables to initial hash value
  WORD a = H[0];
  WORD b = H[1];
  WORD c = H[2];
  WORD d = H[3];
  WORD e = H[4];
  WORD f = H[5];
  WORD g = H[6];
  WORD h = H[7];


//Update working variables using temp variables, refer to 6.2.2 and MACRO's
  WORD temp1, temp2;
  for(WORD i = 0; i < 64; ++i)
  {
    temp1 = h + EP1(e) + CH(e, f, g) + K[i] + message_sched[i];
    temp2 = EP0(a) + MAJ(a, b, c);

    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }


//Set final hash values as...
  H[0] += a;
  H[1] += b;
  H[2] += c;
  H[3] += d;
  H[4] += e;
  H[5] += f;
  H[6] += g;
  H[7] += h;

//Compile hash values into one datatype to be called in main
  for (int i = 0; i < 8; ++i)
  {
    hash[i * 4] = static_cast<BYTE>((H[i] >> 24) & 0xFF);
    hash[i * 4 + 1] = static_cast<BYTE>((H[i] >> 16) & 0xFF);
    hash[i * 4 + 2] = static_cast<BYTE>((H[i] >> 8 ) & 0xFF);
    hash[i * 4 + 3] = static_cast<BYTE>(H[i] & 0xFF);
  }
}


/* MAIN CODE (MEAT & POTATOES)  ***********************************************/

int main()
{
//PT (Plain Text):
    const char *PT = "ABC";
    cout << "\nPT: \n" << PT << endl;


//PT -> BINARY:
    BYTE BinaryPT[8 * strlen(PT)];
    size_t sizeOfPT = 0;

    for (size_t i = 0; i < strlen(PT); i++)
    {
        BYTE currentChar = PT[i];
        BinaryPT[sizeOfPT++] = currentChar;
    }

    cout << "\nBinary:" << endl;
    printBinary(BinaryPT, sizeOfPT);


//BINARY -> PADDED:
    BYTE padded_message[64];
    size_t message_len = sizeOfPT;
    size_t padded_len;

    addPadding(BinaryPT, message_len, padded_message, &padded_len);

    cout << "\nPadded Binary:" << endl;
    printBinary(padded_message, padded_len);


//PADDED -> MESSAGE SCHEDULE:
    WORD message_sched[64];
    messageSchedule(padded_message, message_sched);

    cout << "\nMessage Schedule:" << endl;
    printBinary(reinterpret_cast<BYTE*>(message_sched), 64 * sizeof(WORD));

//MESSAGE SCHEDULE -> HASH:
    BYTE hash[32];
    getHash( message_sched, hash);

    cout << "\nHash (BINARY) " << endl;
    printBinary(hash, 32);

  cout << "\nHash (HEX) " << endl;
  printHex(hash, 32);

    return 0;
}
