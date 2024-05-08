// expAllign.c

#include "avxmpfr_utilities.h"

/*
    This code is designed to take any two numbers and allign the exponents in a way that the AVXMPFR algorithm can use.

    It always alligns exponents from lower exp to higher exp as this reduces the information loss.
    Future versions may have a setting for rounding down or round up when alligning.
*/

mpfr_exp_t avxmpfr_exp_allign(mpfr_t firstNum, mpfr_t secondNum, const uint16_t PRECISION) 
{
    /* 
	Take two numbers and find the one with the lower exponent to shift right until the exponents match.
	This does not guarantee numbers to be normalised after the operation.

	This also works directly with the mpfr_t variable as mpfr is able to understand the value of mpfr_t numbers that have been shifted with exponent shift aswell.

	Returns the largest exponent
    */
    
    // Flag for testing if mpfr_t variables were swapped temporarily
    char mpfrSwap = 0;

    // Reduce some function calls by grabbing the exponents early
    mpfr_exp_t firstExp = (firstNum)->_mpfr_exp;
    mpfr_exp_t secondExp = (secondNum)->_mpfr_exp;

    // Check if exponents are already alligned
    if (firstExp == secondExp)
		return secondExp;
    
    // Make firstNum the mpfr_t with the bigger exponent 
    // Swap back after shifitng
    // This is done to avoid a different function for shifting
    // Could also be optimised to only swap the ptrs of the mantissa
    if (firstExp > secondExp)
    {
		mpfr_swap(firstNum, secondNum);
		mpfrSwap = 1;
    }
	

    /* Now to move onto actually shifting */
	

//#define AVX_SHIFT 1

#ifdef AVX_SHIFT
    // Check first if shift is equal to or less than 64 bits
    mpfr_exp_t expDifference;    
    expDifference = (secondNum)->_mpfr_exp - (firstNum)->_mpfr_exp;


	// Now that you have the MPFR number, extract it into AVX2 for vectorising the shift
	__m256i_u avx_first_num = _mm256_set_epi64x(firstNum->_mpfr_d[0],  // The least significant AVX lane / MPFR limb
					firstNum->_mpfr_d[1],
					firstNum->_mpfr_d[2],
					firstNum->_mpfr_d[3]); 
					
	
					
    if (expDifference <= 64)
    {
		
		//avx_first_num = _mm256_or_si256 ( _mm256_slli_epi64 ( a, n ), _mm256_blend_epi32 ( _mm256_setzero_si256 ( ), _mm256_permute4x64_epi64 ( _mm256_srli_epi64 ( a, 64 - n ), _MM_SHUFFLE ( 2, 1, 0, 0 ) ), _MM_SHUFFLE ( 3, 3, 3, 0 ) ) );
		
		
		// First mask out the low bits
		__m256i_u last_bit_mask = _mm256_set1_epi64x(expDifference);
		__m256i_u last_bit = _mm256_and_si256(avx_first_num, last_bit_mask);
		
			//			// Shift the top lane for right shifting it
			//			__m256i top_lane_mask = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0, 0x0, 0x0);

		
		// Shift the bit right across lanes.
		__m256i_u top_bit = _mm256_sll_epi64(last_bit, _mm_cvtsi32_si128(64-expDifference - 2)); // -1 for carry bit? wait no this is pre nail bit
		
			//			// Shift the first lane of top bits by expDif bits
			//			__m256i shifted_lane = _mm256_srli_epi64(_mm256_and_si256(data, top_lane_mask), expDifference);
		
		top_bit = _mm256_set_epi64x(top_bit[2],
							top_bit[1],
							top_bit[0],
							top_bit[0] - expDifference);	// First lane is right shifted by expDifference
							
		avx_first_num = _mm256_srl_epi64(avx_first_num, _mm_cvtsi32_si128(expDifference - 2));	// Shift each lane right by expDiff
		avx_first_num = _mm256_or_si256(avx_first_num, top_bit);
		
		firstNum->_mpfr_d[0] = avx_first_num[3];
		firstNum->_mpfr_d[1] = avx_first_num[2];
		firstNum->_mpfr_d[2] = avx_first_num[1];
		firstNum->_mpfr_d[3] = avx_first_num[0];
		
		// Now set the exponent to the shifted value
		(firstNum)->_mpfr_exp += expDifference;
		
		printf("I am here and working nerd");
	}
    // If a difference of greater than 64
    else if (expDifference > 64) 
    {
		printf("NOT HERE NERD\n");
		// Keep track of total times a whole limb has shifted
		int limbShiftCount = 0;

		while (expDifference > 64)
		{
			printf("TRIPLE SHIFTERD FATALITY\n");
			// First mask out the low bits
			__m256i_u last_bit_mask = _mm256_set1_epi64x(expDifference);
			__m256i_u last_bit = _mm256_and_si256(avx_first_num, last_bit_mask);
			
			// Shift the bit right across lanes.
			__m256i_u top_bit = _mm256_sll_epi64(last_bit, _mm_cvtsi32_si128(64-expDifference));
			top_bit = _mm256_set_epi64x(top_bit[2],
								top_bit[1],
								top_bit[0],
								top_bit[0] - expDifference);	// First lane is right shifted by expDifference
								
			avx_first_num = _mm256_srl_epi64(avx_first_num, _mm_cvtsi32_si128(expDifference));	// Shift each lane right by expDiff
			avx_first_num = _mm256_or_si256(avx_first_num, top_bit);
			expDifference -= 64;
			limbShiftCount++;	
		}

		// The difference should now be less than or equal to 64
		// First mask out the low bits
		__m256i_u last_bit_mask = _mm256_set1_epi64x(expDifference);
		__m256i_u last_bit = _mm256_and_si256(avx_first_num, last_bit_mask);
		
		// Shift the bit right across lanes.
		__m256i_u top_bit = _mm256_sll_epi64(last_bit, _mm_cvtsi32_si128(64-expDifference));
		top_bit = _mm256_set_epi64x(top_bit[2],
							top_bit[1],
							top_bit[0],
							top_bit[0] - expDifference);	// First lane is right shifted by expDifference
							
		avx_first_num = _mm256_srl_epi64(avx_first_num, _mm_cvtsi32_si128(expDifference));	// Shift each lane right by expDiff
		avx_first_num = _mm256_or_si256(avx_first_num, top_bit);

		// Now set the exponent to the shifted value
		printf("Final exp is %ld\n", (firstNum)->_mpfr_exp + GMP_NUMB_BITS * limbShiftCount + expDifference);
		(firstNum)->_mpfr_exp += GMP_NUMB_BITS * limbShiftCount + expDifference;
		
		printf("\tIran I swear\n");
    }

#endif




#ifndef AVX_SHIFT
    // Check first if shift is equal to or less than 64 bits
    mpfr_exp_t expDifference;    
    expDifference = (secondNum)->_mpfr_exp - (firstNum)->_mpfr_exp;

    // If a difference of 64 or less, shift directly
    if (expDifference <= 64)
    {
		mpn_rshift((firstNum)->_mpfr_d, (firstNum)->_mpfr_d, (PRECISION + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, expDifference);

		// Now set the exponent to the shifted value
		(firstNum)->_mpfr_exp += expDifference;
    }

    // If a difference of greater than 64
    else if (expDifference > 64) 
    {
		// Keep track of total times a whole limb has shifted
		int limbShiftCount = 0;

		while (expDifference > 64)
		{
			mpn_rshift((firstNum)->_mpfr_d, (firstNum)->_mpfr_d, (PRECISION + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 64);
			expDifference -= 64;
			limbShiftCount++;	
		}

		// The difference should now be less than or equal to 64
		mpn_rshift((firstNum)->_mpfr_d, (firstNum)->_mpfr_d, (PRECISION + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, expDifference);

		// Now set the exponent to the shifted value
		(firstNum)->_mpfr_exp += GMP_NUMB_BITS * limbShiftCount + expDifference;
    }	
    #endif
	
	/* Code runs for both AVX and linear */
    /* Allignment complete */

    // If the mpfr_t variables were swapped, swap them back
    if (mpfrSwap == 1)
    {
		mpfr_swap(firstNum, secondNum);
		return firstExp; 
    }

    return secondExp;
}


/*
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
*/
