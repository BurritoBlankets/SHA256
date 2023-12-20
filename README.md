# SHA256
SHA256 in CPP
Algorithm Process:
The first step of the SHA-256 CHF is to convert the plain text (PT) input string into its binary representation. Initially posing as a challenge due to an overcomplication of maters, I utilized a vector matrix to complete the above task. Upon discovering Conte, B (2015) crypto-algorithms [sha256_test.c], I decided to do away with my vector matrix and opt for a more straightforward approach, which involved creating my own data types by creating a typedef for unsigned eight-bit integers and unsinged 32-bit integers as “BYTE[s]” and “WORD[s]” (Figure 1). This change enabled an easier conceptualization for my ASCII to binary conundrum, which was quickly resolved by type-casting my string input to my newly defined BYTE data type (Figure 2).

Fig 1.
 

Fig 2.
 
      
After a binary input is obtained, the message can be padded into a 512-bit BYTE pointer. The padding process for SHA256 is broken down into four parts. The first part consists of the binary message. Following this, a bit of value ‘1’ is then appended to the padded message. After this ‘1’, a Zeros Padding of length k (Figure 3) is then appended. Lastly, the final 64 bits are reserved for the message bit length (M). See Figure 4 for reference, obtained from 5.1.1 of Publication; its implementation can be viewed in Figure 5 
Fig 3.
k = 447-M mod 512

Fig 4.
 

Fig 5.
 

The next step in the SHA256 process would be to initialize the initial hash values and (k) constants. The initial hash values can be obtained by “taking the first thirty-two bits of the fractional parts of the square roots of the first eight prime numbers” (5.3.3). Similarly, the (K)onstants for SHA256 are gathered by “the first thirty-two bits of the fractional parts of the cube roots of the first sixty-four prime numbers” (4.2.2). Fortunately, these values are referenced in hexadecimal format in the Publication, which were conveniently set as global variables in Figure 6 below.

Fig 6.
 
	
After the message is appropriately padded, the initial hash values and (K)onstants need to be initialized; this step was personally the most challenging. While the first 16 double words of the message schedule are straightforward to compute, as they consist of an untouched padded binary, the 17-64th iteration of the message schedule was more complex. As per the FIPS PUB 180-4 (2015) [6.4.2] Secure Hash Standard, those iterations were to be computed by:
 

After reviewing 4.1.2 of the Publication, the values of σ1 and σ2 were discovered to be:
 

Due to my elementary understanding of binary operands, my research in deciphering the above led me to stumble upon a video by RedBlockBlue (2022) SHA-256 | COMPLETE Step-By-Step Explanation, which explained the from a non-mathematical approach. From the references, it was discovered that both sigma functions consisted of a right rotation of a specified value, exclusive or-ed (XOR), by another right rotation of some other value, XOR by a shift right of some additional value.	

Not knowing how to implement the above bitwise rotations and right shifts into code, I again referenced the crypto-algorithms repository, where I stumbled upon Conte, B. (2015) crypto-algorithms [sha256.c]. Here, I noticed a simplistic utilization of macros to implement all of the used bitwise operands. Not seeing any reason to convolute the process with lengthy functions, I implemented said macros into my code (Figure 7).

Fig 7.
 

           With these macros defined, the message schedule was quickly implemented by nesting the above FIPS equation into a for loop, which iterated for every double word in the message schedule (Fig 4). Although not included in the FIPS PUB, the “& 0xFFFFFFFF” takes our equation and adds it to the max value of the unsigned 32-bit integer; this ensures that the resulting computation is 32-bits.

Fig 8. 

With the initial hashing values defined and the message schedule set, the SHA-256 hash can is now ready to be computed. To compute the hash, it’s first necessary to link the hash values to a working hash variable, which can be viewed in Figure 9.

Fig 9.
 

           Using the working variables just defined, the next step in computing the hash requires a “chunk loop.” The “chunk loop” iterates for every double word in the message schedule and operates by utilizing two temporary values, T1 & T2, whose respective values can be seen in Figure 10. After the two temporary values are set for each iteration of the loop, their outputs influence the values of ‘e’ and ‘a’ of the working variables, slowly manipulating the other working variables. The entire process implemented in C++ can be seen in Figure 11.

Fig 10.
 

Fig 11. 

           Following the “chunk loop,” the final working hash values are then added to their initial hash values, leaving the last step of the hash computation process to combine all final hash values into a single composite, which can be visualized in Figure 12 below.

Fig 12
  

Note:
As of December 9, 2023, the current version of this code doesn’t correctly return the expected output of the SHA256 algorithm. This error can be attributed to an incorrect bug in the message schedule process in which the right shift binary operand incorrectly acts as a rotate right instead of a right shift.

In Closing:
Overall, this was an enjoyable and challenging process. Going into this project, I didn’t know the difference between a bit, byte, and word; however, now, I am familiar with binary terminology and bitwise operations. My primary source for this project, as listed numerously throughout this report, was the FIPS PUB 180-4; I wanted to rely primarily on this source throughout my project so I could get comfortable with referencing government publications, which I believe this assignment did an excellent job of achieving. Moving forward, I’d like to allocate my winter break to resolving the current bug so that I can comfortably link this assignment to my GitHub to share my step-by-step breakdown of the SHA256 intricacies with others.
