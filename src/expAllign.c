// expAllign.c

#include "avxmpfr_utilities.h"

/*
    This code is designed to take any two numbers and allign the exponents in a way that the AVXMPFR algorithm can use.
    It does not do any checking of exponents beforehand, this is down to the user to determine if it is required prior.

    It always alligns exponents from lower exp to higher exp as this reduces the information loss.
    Future versions may have a setting for rounding down or round up when alligning.
*/

/*

LIST OF OBSERVATIONS / NOTES:
    - Using mpfr_mul_2ui mpfr_div_2ui changes the exponent but does not change the the mantissa limb representation 
    - Due to how mpfr stores its exponent values we cannot assume that the mantissa will change after using mpfr_mul_2ui, so we have to manually shift the mantissa over and change the exponent 
    - The reason we do this manually is because in the AVX registers there will be no concept of exponent when trying to add the blocks, so we shift up the lower exp block to lose less precision if possible ( Currently without rounding )
    - The above does not lend to make itself work with the actual mpfr_t numbers, often experiencing exponent mismatch
    - This may be solved by having a clone of the original mpfr_t before shift and then transfer the new mantissa back into this value
    

    - So overall the process will be to find the number with lower exponent and to transform it to be alligned.	
*/


void avxmpfr_exp_allign(mpfr_t *firstNum, mpfr_t *secondNum) 
{
    /* 
	Take two numbers and find the one with the lower exponent to shift right until the exponents match.
	This does not guarantee numbers to be normalised after the operation.
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

#if 0
	    Old Main for testing before the creation of avxmfr_exp_allign 

int main() {
    mpfr_t myNumber;
    mpfr_t num2;

    mpfr_init2(myNumber, 256);  // Set precision to 256 bits
    mpfr_init2(num2, 256);

    // Initialize the variable with a known value
    mpfr_set_str(myNumber, "1.25", 10, MPFR_RNDN);
    mpfr_set_str(num2, "1.25", 10, MPFR_RNDN);

    // Print initial values
    printf("Before transform:\n");
    mpfr_exp_t exponent_before = mpfr_get_exp(myNumber);
    printf("Exponent before transform: %ld\n", exponent_before);

    // Get mantissa as a string
    char mantissa_before[512];
    mpfr_sprintf(mantissa_before, "%Re", myNumber);
    printf("Mantissa before transform: %s\n", mantissa_before);

    // Mantissa as a limb 
    mpfr_prec_t precision = mpfr_get_prec(myNumber);
    mp_limb_t *limbs = (mp_limb_t *)myNumber->_mpfr_d;

    printf("Limb ");
    print_binary(limbs, PRECISION_256);

    
    // Lets try right shift the number thus reduce the mantissa and increase the exponent 
    mpn_rshift(limbs, limbs, (precision + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 2);
    mpfr_set_exp( myNumber, 3); // Changing or not changing the exponent does not matter here as we will still have to normalise ourselves after as MPFR is not a fan of messing with the normalisation of limbs between function calls
				// But we say that we have changed it for personal clarity that it has been modified, however this test number is still normalised at the exponent of 1 
				// We would normally also bring its exponent up to the number of the mpfr_t with a higher exponent


    // Print values after transform
    printf("\n");
    printf("\nAfter transform:\n");
    mpfr_exp_t exponent_after = mpfr_get_exp(myNumber);
    printf("Exponent after transform: %ld\n", exponent_after);

    // Get mantissa as a string
    char mantissa_after[512];
    mpfr_sprintf(mantissa_after, "%Re", myNumber);
    printf("Mantissa after transform: %s\n", mantissa_after);

    //precision = mpfr_get_prec(myNumber);
    //mp_limb_t *limbs3 = (mp_limb_t *)myNumber->_mpfr_d;

    printf("Limb ");
    print_binary(limbs, PRECISION_256);
    // This part of the code is custom test of mpn_rshift();
    // You can assume now that the exponent has been increased, so the mantissa should decrease    

    // You should rshift the number with the higher exponent and decreaese the exponent, because as float point goes right, exp lowers 
    // Is the above true? The floating point should remain in the same location, if u are shifting right the exp shold increase as the float is effectively left
    // But am I lowering the exponent to line up the floating point at the same location between mantissas
    
    // As mantissa increases, exponent decreases

    //mpn_rshift(limbs, limbs, (precision + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 2);

    //mpfr_set_exp( myNumber, 1);


    /*

    // Testing myNumber being divided by 2 using mpfr_div_2ui

    mpfr_div_2ui(myNumber, myNumber, 2, MPFR_RNDN);
    exponent_after = mpfr_get_exp(myNumber);
    printf("\n");
    printf("\nAfter division: \n");
    printf("Exponent after divison: %ld\n", exponent_after);
    mpfr_sprintf(mantissa_after, "%Re", myNumber);
    printf("Mantissa after division: %s\n", mantissa_after);
    // You would have to probably renormalise the result after this because MPFR doesnt exactly appreciate how stuff gets added with mixed exps and mantissas

    printf("Limb");
    for (mpfr_prec_t i = 0; i < (precision + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS; ++i)
    {
	print_binary(limbs[i]);
	printf(" ");
    }

    
    */

    printf("\n");
    printf("\nAdd together both the transformed number and original number:\n");
    mpfr_add(num2, num2, myNumber, MPFR_RNDN);
    char mant_add[512];
    mpfr_sprintf(mant_add, "%Re", num2);
    exponent_after = mpfr_get_exp(num2);
    printf("Exponent after multiplication: %ld\n", exponent_after);
    printf("Added together mantissa is : %s\n", mant_add);
 
    mp_limb_t *limbs2 = (mp_limb_t *)num2->_mpfr_d;

    printf("Limb ");
    print_binary(limbs, PRECISION_256);
    mpfr_sprintf(mant_add, "%Re", num2);
    exponent_after = mpfr_get_exp(num2);

    /*
	Could also test having a manual limb[3] binary add and compare the results to simulate what the AVX instruction would behave like
    */

    // Cleanup
    mpfr_clear(myNumber);
    mpfr_clear(num2);

    return 0;
} */

#endif
