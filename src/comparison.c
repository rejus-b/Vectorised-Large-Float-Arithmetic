/*
    Test file to compare the speedup of avxmpfr if any, in regards to mpfr.
    It will also test the correctness of the binary representation.

    mpfr_add() will only be called with MPFR_RNDN as this is all avxmpfr implements.
*/

#include "avxmpfr_utilities.h"
#include <time.h>
#include <stdlib.h>

void assign_binary(char* binNum)
{
    // Initialise array size 
    const int size = 254;

    for (int i = 0; i < size - 2; i++)
    {
	binNum[i] = '0' + (rand() % 2); // Clamp to char 0 or 1 
    }

    // Set a random value to a decimal point 
    binNum[rand() % 252] = '.';

    // Set the null terminator
    binNum[size - 1] = '\0';
    
    return;
}

int main()
{
    // Set a random seed for number generation
    srand(time(NULL));
   
     // Set up an array long enough to act as a 252 string 
    char first_bin[254]; // 252 bits of precision + 1 for '\0' and +1 for a '.'   
    char second_bin[254];

    assign_binary(first_bin);
    assign_binary(second_bin);
    

    // Initialise the mpfr_t types
    mpfr_t number1, number2, mpfr_result, avxmpfr_result;
    mpfr_inits2(PRECISION_256, number1, number2, mpfr_result, avxmpfr_result, NULL);

    // Assign the mpfr_t numbers
    mpfr_set_str(number1, first_bin, 2, MPFR_RNDN);
    mpfr_set_str(number2, second_bin, 2, MPFR_RNDN);

// For testing force normalisation
//number1->_mpfr_d[3] |= 0xF100000000000000;
//number2->_mpfr_d[3] |= 0xF100000000000000;


    // Use mpfr_add() and print the result
    printf("\n\t\t mpfr_add()\n\n");
    mpfr_add(mpfr_result, number1, number2, MPFR_RNDF); // Setting it to faithful rounding makes it no longer fail
    mpfr_printf("\n%.252Rf\n", mpfr_result);

    // Print the binary limbs aswell
    printf("\nEXP: %ld\n", (mpfr_result)->_mpfr_exp);
    mp_limb_t* mpfr_limbs = (mp_limb_t *) mpfr_result->_mpfr_d;
    print_binary(mpfr_limbs, PRECISION_256);
    printf("\n"); 

    
    // Use avxmpfr_add() and print the result
    printf("\n\t\t avxmpfr_add()\n\n");
    avxmpfr_add(avxmpfr_result, number1, number2, MPFR_RNDN, PRECISION_256);
    mpfr_printf("\n%.252Rf\n", avxmpfr_result);

    // Print the binary limbs again
    printf("\nEXP: %ld\n", (avxmpfr_result)->_mpfr_exp);
    mp_limb_t* avxmpfr_limbs = (mp_limb_t *) avxmpfr_result->_mpfr_d;
    print_binary(avxmpfr_limbs, PRECISION_256);
    printf("\n");

    int cmp_result = mpfr_equal_p(mpfr_result, avxmpfr_result);
    printf("\n\nComparison return: %i\n\n", cmp_result);  

printf("Precision of mpfr_result: %ld\n", mpfr_get_prec(mpfr_result));
printf("Precision of avxmpfr_result: %ld\n", mpfr_get_prec(avxmpfr_result));

    mpfr_clear(mpfr_result);
    mpfr_clear(avxmpfr_result);
    mpfr_clear(number1);
    mpfr_clear(number2); 


    // Use a XOR comparison on both limbs to prove they are the same/different
    // Should be a perfect 0 if all limbs are equal
    mp_limb_t limb_compare[4];
    char matches = 0;
    
    for (int i = 3; i >= 0; i--)
    {
	limb_compare[i] = mpfr_limbs[i] ^ avxmpfr_limbs[i];	
	printf("Limb %d: mpfr=%lx, avxmpfr=%lx, XOR=%lx\n", i, mpfr_limbs[i], avxmpfr_limbs[i], limb_compare[i]);
	if (limb_compare[i] == 0)
	    matches++;
    } 

    if (matches == 4)
	printf("\n\x1b[32mLimbs are equal\x1b[0m\n\n");
    else
	printf("\n\x1b[31mLimbs are unequal\x1b[0m\n\n");
    
    return 0;
}
