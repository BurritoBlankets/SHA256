SHA-256 Algorithm Breakdown For Dummies (with C++)
==================================================

>[!WARNING]  
> As of December 9, 2023, the current version of this code doesn't correctly return the expected output of the SHA256 algorithm.
<!---
This error can be attributed to an incorrect bug in the message schedule process in which the right shift binary operand incorrectly acts as a rotate right instead of a right shift.
--->

## Purpose
Before jumping into this project I didnt know the diffrence between a bit, byte, or word. Despite this I was determined to move forward so that I could beter understand the intricate processes of how SHA256 operates. Bellow you'll find a step by step process of the algorithm that aims further explain each process at a novice level. 

## Initialization
For the first phase of the Secure Hasing Algorithm (SHA)256 you'll want to first define your custom data types, intilize the inital hash values and constants.

### Data Types
Since SHA256 involves numerours uses of binary operations, the first step to adress would be to create a custom data-types. Since the smallest unit of memory in C/C++ is a byte; for our purposes we'll be dealing with Bytes (8 bits) and Words (2 Bytes).
```cpp
/* DATATYPES  *****************************************************************/
typedef uint8_t BYTE;
typedef uint32_t WORD;
```

### Initial Hash Value
Bellow are the initial hash values for SHA256 in a hexidecimal format which are utilized in the Hash Computation Segment. The values are obtained by "taking the first thirty-two bits of the fractional parts of the square roots of the first eight prime numbers" [(FIPS 180-4 5.3.3)](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf#page=20&zoom=auto,-19,663). However for all intents and purposes, these values can be hard-coded like so.
```cpp
WORD H[8]=
{
  0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
  0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
};
```

### Constants
Similarly to the hash values, below in hexidecimal format are the constants which "represent the first sixty-four bits of the fractional parts of the cube roots of the first eighty prime numbers" [(FIPS 180-4 4.2.2)](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf#page=16&zoom=auto,-19,315).
```cpp
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
```

## Preprocessing
In the second phase of the algorithm, you'll want to convert your input string into its binary representation. Afterwards the binary is then padded to generate the message schedule.

### User Input
To avoid scanf/scanf_s compatibility errors between linux and windows machines, my code hard-codes the input string as below.
```cpp
//PT (Plain Text):
    const char *PT = "abc";
    cout << "\nPT: \n" << PT << endl;
```

### Get Binary
After the Plain Text (PT) message is obtained, it must then be converted to binary. This process is achived by utilizing a for-loop to typecasting each char into a BYTE, which is then appended an BYTE array as seen below.
```cpp
//PT -> BINARY:
    BYTE BinaryPT[8 * strlen(PT)];
    size_t sizeOfPT = 0;

    for (size_t i = 0; i < strlen(PT); i++)
    {
        BYTE currentChar = PT[i];
        BinaryPT[sizeOfPT++] = currentChar;
    }
```

# Make Padding
To ensure that SHA256 is  collision resistant; no two diffrent inputs returns the same value, the algorithm incorperates a padding function. The padding process for SHA256 is broken down into four parts. The first part consists of the binary message. Following this, a bit of value '1' is then appended to the padded message. After, a Zeros Padding of length 𝓀 is then appended. Lastly, the final 64 bits are reserved for the message bit length (𝓁). 

The equation provided by FIPS 180-4 5.1.1 equation can be re-written to determine the value of 𝓀.

$$𝓁 + 1 + 𝓀 \equiv 448 \bmod{512}$$

$$𝓀 \equiv 447 - 𝓁 \bmod{512}$$

Taking our PT from earlier (abc), our padded binaryPT should look like...
```math
\underbrace{01100001}_{\text{`a'}} \quad \underbrace{01100010}_{\text{`b'}} \quad \underbrace{01100011}_{\text{`c'}} \quad 1 \quad \overbrace{00...00}^{\text{423 bits}} \quad \overbrace{00...\underbrace{011000}_{\text{`𝓁=24'}}}^{\text{64 bits}}
```

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG3.png)
Fig 4.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG4.png)

Fig 5.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG5.png)

The next step in the SHA256 process would be to initialize the initial hash values and (k) constants. The initial hash values can be obtained by "taking the first thirty-two bits of the fractional parts of the square roots of the first eight prime numbers" (5.3.3). Similarly, the (K)onstants for SHA256 are gathered by "the first thirty-two bits of the fractional parts of the cube roots of the first sixty-four prime numbers" (4.2.2). Fortunately, these values are referenced in hexadecimal format in the Publication, which were conveniently set as global variables in Figure 6 below.

Fig 6.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG6.png)

After the message is appropriately padded, the initial hash values and (K)onstants need to be initialized; this step was personally the most challenging. While the first 16 double words of the message schedule are straightforward to compute, as they consist of an untouched padded binary, the 17-64th iteration of the message schedule was more complex. As per the [FIPS PUB 180-4 (2015) [6.4.2] Secure Hash Standard](http://dx.doi.org/10.6028/NIST.FIPS.180-4), those iterations were to be computed by:

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIGEQ1.png)

After reviewing 4.1.2 of the Publication, the values of σ1 and σ2 were discovered to be:

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIGEQ2.png)

Due to my elementary understanding of binary operands, my research in deciphering the above led me to stumble upon a video by [RedBlockBlue (2022) SHA-256 | COMPLETE Step-By-Step Explanation, ](https://www.youtube.com/watch?v=orIgy2MjqrA)which explained the from a non-mathematical approach. From the references, it was discovered that both sigma functions consisted of a right rotation of a specified value, exclusive or-ed (XOR), by another right rotation of some other value, XOR by a shift right of some additional value.

Not knowing how to implement the above bitwise rotations and right shifts into code, I again referenced the crypto-algorithms repository, where I stumbled upon [Conte, B. (2015) crypto-algorithms [sha256.c]](https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c). Here, I noticed a simplistic utilization of macros to implement all of the used bitwise operands. Not seeing any reason to convolute the process with lengthy functions, I implemented said macros into my code (Figure 7).

Fig 7.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG7.png)

With these macros defined, the message schedule was quickly implemented by nesting the above FIPS equation into a for loop, which iterated for every double word in the message schedule (Fig 4). Although not included in the FIPS PUB, the "& 0xFFFFFFFF" takes our equation and adds it to the max value of the unsigned 32-bit integer; this ensures that the resulting computation is 32-bits.

Fig 8.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG8.png)

With the initial hashing values defined and the message schedule set, the SHA-256 hash can is now ready to be computed. To compute the hash, it's first necessary to link the hash values to a working hash variable, which can be viewed in Figure 9.

Fig 9.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG9.png)

           Using the working variables just defined, the next step in computing the hash requires a "chunk loop." The "chunk loop" iterates for every double word in the message schedule and operates by utilizing two temporary values, _T1 & T2_, whose respective values can be seen in Figure 10. After the two temporary values are set for each iteration of the loop, their outputs influence the values of 'e' and 'a' of the working variables, slowly manipulating the other working variables. The entire process implemented in C++ can be seen in Figure 11.

Fig 10.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG10.png)

Fig 11.

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG11.png)

Following the "chunk loop," the final working hash values are then added to their initial hash values, leaving the last step of the hash computation process to combine all final hash values into a single composite, which can be visualized in Figure 12 below.

Fig 12

![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG12-1.png)
![](https://github.com/BurritoBlankets/SHA256/blob/main/screenshots/FIG12-2.png)


**In Closing:**

Overall, this was an enjoyable and challenging process. Going into this project, I didn't know the difference between a bit, byte, and word; however, now, I am familiar with binary terminology and bitwise operations. My primary source for this project, as listed numerously throughout this report, was the FIPS PUB 180-4; I wanted to rely primarily on this source throughout my project so I could get comfortable with referencing government publications, which I believe this assignment did an excellent job of achieving. Moving forward, I'd like to allocate my winter break to resolving the current bug so that I can comfortably link this assignment to my GitHub to share my step-by-step breakdown of the SHA256 intricacies with others.
