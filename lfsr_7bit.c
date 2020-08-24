/*
 *  LFSR using a TAP lots of terrible hard coding
 *  this was used to make sure I knew what I was doing for 
 *  the ISA design for a RISC processor that I need to make. 
 * 
 * 
 * 
 * 
 * 
 * 8-bit values comprising bit[7] = ^bit[6:0], and bit[6:0] = (original data - 0x20)^LFSR(i).
*/


#include<stdio.h>
#include <stdbool.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


//unsigned int TAP[] = {7, 5, 4, 3};
unsigned int TAP[] = {7, 6, 5, 4, 2, 1};
char inputMsg[] = "             Mr. Watson, come here. I want to see you.";
char msb_mask = 127;  //01111111
char ASCIISPACE = 32;

char runLFSR(char lfsr) {

    bool xorResult = false;  // holds the LSB of our final result

    // Run the tap sequence to get desired xor value
    for (unsigned int j = 0; j < 6; j++) {
        // shift the one to the bit jth location
        // AND current ASCII with shifted bit
        // move result back to start position
        bool currentBit = (lfsr & (1 << (TAP[j]-1))) >> TAP[j]-1;

        xorResult = xorResult^currentBit;  // XOR with rest of results

    }

    lfsr = lfsr << 1;  // shift everything left to get next state
    lfsr = lfsr | xorResult; //set lsb to result of XOR
    
    lfsr = lfsr & msb_mask;  //clear 8th bit -> this is only supposed to be a 7 bit lfsr
    return lfsr;
}

char addParityBit(char current) {

    bool parityBit = false;

    // reduction XOR method - only want to run 7 times
    for (int j = 0; j < 7; j++) {
        bool r = (current & (1 << j)) >> j;
        parityBit = parityBit^r;
    }


    // get the one at the MSB of a char
    char temp = 0;
    temp = temp | parityBit;
    temp = temp << 7;

    //printf("parity = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(temp));

    current = current & msb_mask;  // 0's out the front most bit
    current = current | temp;  // sets the front most bit to result of parity

    return current;

}

int main() {

    printf("input msg = %s \n", inputMsg);


    char tapArray[4];
    sprintf(tapArray, "%ld", TAP);


    char resultArray[sizeof(inputMsg)];
    char lfsr = ASCIISPACE;

    for (unsigned int i = 0; i < sizeof(inputMsg); i++) {

        char current = inputMsg[i];  // easier to read

        printf("\n");
        printf("%d. ", i);
        printf("Start with |%c| ", current);
        printf("move down to |%c| ", current-32);
        printf("with binary value of "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(current-32));
        printf(" with LFSR = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(lfsr));

        // result should be ^bit[6:0], and bit[6:0] 
        // where bit[6:0] = (original data - 0x20)^LFSR(i).

        // PART 1 - og data - 0x20 - Subtract 32 from character
        current = current - ASCIISPACE;

        // Part 2 - XOR with lfsr state
        current = current^lfsr;

        // Part 3 - use reduction XOR to get parity bit for 8th
        current = addParityBit(current); // set parity bit --> PART 6

        resultArray[i] = current;     // set result

        // doing this to calc the next state of lfsr
        lfsr = runLFSR(lfsr);  // run the LFSR --> Part 3
        printf("---- store "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(current));



    }

    // print out full result
    printf("\nEncrypted String\n");
    for (int i = 0; i < sizeof(resultArray); i++) {
        printf("%c", resultArray[i]);
    }
    return 0;
}  //end main