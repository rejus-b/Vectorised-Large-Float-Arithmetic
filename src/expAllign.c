// expAllign.c

#include "avxmpfr_utilities.h"

/*
    This code is designed to take any two numbers and allign the exponents in a way that the AVXMPFR algorithm can use.

    It always alligns exponents from lower exp to higher exp as this reduces the information loss.
    Future versions may have a setting for rounding down or round up when alligning.
*/

void avxmpfr_exp_allign(mpfr_t *firstNum, mpfr_t *secondNum) 
{
    /* 
	Take two numbers and find the one with the lower exponent to shift right until the exponents match.
	This does not guarantee numbers to be normalised after the operation.

	This also works directly with the mpfr_t variable as mpfr is able to understand the value of mpfr_t numbers that have been shifted with exponent shift aswell.
    */
    
    // Flag for testing if mpfr_t variables were swapped temporarily
    short mpfrSwap = 0;

    // Reduce some function calls by grabbing the exponents early
    mpfr_exp_t firstExp = (*firstNum)->_mpfr_exp;
    mpfr_exp_t secondExp = (*secondNum)->_mpfr_exp;

    // Check if exponents are already alligned
    if (firstExp == secondExp)
	return;
    
    // Make firstNum the mpfr_t with the bigger exponent 
    // Swap back after shifitng
    // This is done to avoid a different function for shifting
    // Could also be optimised to only swap the ptrs of the mantissa
    if (firstExp > secondExp)
    {
	mpfr_swap(*firstNum, *secondNum);
	mpfrSwap = 1;
    }

    /* Now to move onto actually shifting */

    // Check first if shift is equal to or less than 64 bits
    mpfr_exp_t expDifference;    
    expDifference = (*secondNum)->_mpfr_exp - (*firstNum)->_mpfr_exp;

    // If a difference of 64 or less, shift directly
    if (expDifference <= 64)
    {
	printf("\n exp diff: %ld", expDifference);
	mpn_rshift((*firstNum)->_mpfr_d, (*firstNum)->_mpfr_d, (PRECISION_256 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, expDifference);

	// Now set the exponent to the shifted value
	(*firstNum)->_mpfr_exp += expDifference;
    }

    // If a difference of greater than 64
    else if (expDifference > 64) 
    {
	// Keep track of total times a whole limb has shifted
	int limbShiftCount = 0;

	while (expDifference > 64)
	{
	    mpn_rshift((*firstNum)->_mpfr_d, (*firstNum)->_mpfr_d, (PRECISION_256 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 64);
	    expDifference -= 64;
	    limbShiftCount++;	
	}

	// The difference should now be less than or equal to 64
	mpn_rshift((*firstNum)->_mpfr_d, (*firstNum)->_mpfr_d, (PRECISION_256 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, expDifference);

	// Now set the exponent to the shifted value
	(*firstNum)->_mpfr_exp += GMP_NUMB_BITS * limbShiftCount + expDifference;
    }	
    
    /* Allignment complete */

    // If the mpfr_t variables were swapped, swap them back
    if (mpfrSwap == 1)
	mpfr_swap(*firstNum, *secondNum);

    return;
}

int main() {

    mpfr_t number1, number2;

    // Set the numbers
    mpfr_inits2(PRECISION_256, number1, number2, NULL);
      
    // Assign the numbers 
    mpfr_set_str(number1, "1.25", 10, MPFR_RNDN);
    mpfr_set_str(number2, "2.25", 10, MPFR_RNDN);
 
    // Get a copy of the limbs prior and exp 
    printf("Exponents and limbs prior: \n");

    printf("Exp: %ld \n", (number1)->_mpfr_exp);
    mp_limb_t *limbs = (mp_limb_t *)number1->_mpfr_d; 
    print_binary(limbs, PRECISION_256);

    printf("\n");
    printf("Exp: %ld \n", (number2)->_mpfr_exp);
    limbs = (mp_limb_t *)number2->_mpfr_d; 
    print_binary(limbs, PRECISION_256);
   

    // Test that different exponents are found and alligned when less than or equal to 64
    avxmpfr_exp_allign(&number1, &number2);

    // Get a copy of the limbs after and exp
    printf("\n"); 
    printf("Exponents and limbs after: \n");

    printf("Exp: %ld \n", (number1)->_mpfr_exp);
    limbs = (mp_limb_t *)number1->_mpfr_d; 
    print_binary(limbs, PRECISION_256);

    printf("\n");
    printf("Exp: %ld \n", (number2)->_mpfr_exp);
    limbs = (mp_limb_t *)number2->_mpfr_d; 
    print_binary(limbs, PRECISION_256);


    // Now test with exponents arbitrarily inflated beyond 64
    mpfr_set_exp(number1, 126);
    avxmpfr_exp_allign(&number1, &number2);
    
    // Get a copy of the limbs after and exp
    printf("\n"); 
    printf("Exponents and limbs after: \n");

    printf("Exp: %ld \n", (number1)->_mpfr_exp);
    limbs = (mp_limb_t *)number1->_mpfr_d; 
    print_binary(limbs, PRECISION_256);

    printf("\n");
    printf("Exp: %ld \n", (number2)->_mpfr_exp);
    limbs = (mp_limb_t *)number2->_mpfr_d; 
    print_binary(limbs, PRECISION_256);
    
    return 0;
}
