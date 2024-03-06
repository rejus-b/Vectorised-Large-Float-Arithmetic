/*
    Test file to compare the speedup of avxmpfr if any, in regards to mpfr.
    It will also test the correctness of the binary representation.

    mpfr_add() will only be called with MPFR_RNDN as this is all avxmpfr implements.
*/

#include "avxmpfr_utilities.h"
#include <time.h>
#include <stdlib.h>

// Struct for storing a number in representation and its exponent
typedef struct random_number
{
    char binNum[253]; // 252 bits of precision + 1 for '\0'
    mpfr_exp_t exp;
} random_number;

void assign_binary(random_number* num)
{
    // Initialise array size 
    const int size = 253;

    // Create 252 bits of random binary as a str
    for (int i = 0; i < size - 1; i++)
    {
	num->binNum[i] = '0' + (rand() % 2); // Clamp to char 0 or 1 
    } 

    // Set the null terminator
    num->binNum[size - 1] = '\0';
 
    return;
}

int main()
{
    // Set a random seed for number generation
    srand(time(NULL));
  
    // Assign a randon number 
    random_number num;
    assign_binary(&num);
    
    printf("\nhello");
    printf("\nstr: %s\n", num.binNum);

    return 0;
}
