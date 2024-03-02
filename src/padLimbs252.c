// padLimbs256.c

/*
    This code takes 4 MPFR limbs and changes the spacing to be consistent formatting of having a leading 0 per limb.
    Example of output MPFR limb value using 6 bit number transformed to work with 8 bits.
    1101 01(00) -> (0)110 (0)101 (Where the original 2 LSB 0s are default MPFR type limb padding.

    This requires:
	A number less than or equal to 2^252 to work with 256 bit numbers.
	Any mpfr_t variables need to be normalised beforehand otherwise there the padding is not guaranteed to work and neither is the AVX-MPFR algorithm
*/

//include <imtrim.h>

#include "avxmpfr_utilities.h"


mp_limb_t* avxmpfr_pad252(mpfr_t mpfrNumber) // Take an input MPFR variable type  
{
    // This could be a void type and pad the original mpfr_t variable limbs directly or not if we want it to be possible to pad without doing the AVX
    // To properly padd it, we will have to mpn_right shift each limb seperatly, the bits shifted out of the right are in the MSB of the return
    // For 256 bit implementation working under 252 bits there will be 4 assumed spaces of default MPFR padding   
    // Limb 4 in mpfr_d is the most significant limb (MSL) Limb 0 is the least significant limb (LSL) a.k.a. Little Endian

    // Extract the limbs from the mpfrNumber
    mp_limb_t* limbs = (mp_limb_t *)mpfrNumber->_mpfr_d;

    /*   
    // Test setting all limbs that can be used, to be used 
    limbs[0] = 0xFFFFFFFFFFFFFFF0;
    limbs[1] = 0xFFFFFFFFFFFFFFFF;
    limbs[2] = 0xFFFFFFFFFFFFFFFF;
    limbs[3] = 0xFFFFFFFFFFFFFFFF;
    */    

    for (int i = 0; i < 4; i++)
    {
	// Shift everything 1 time per iteration, decreasing from limbs[3...0] to limbs [1...0] 

	int limbOffset = i * (sizeof(uint64_t));
	int precisionOffset = i * 64;
	
	mpn_rshift(limbs + limbOffset / GMP_NUMB_BITS, limbs + limbOffset / GMP_NUMB_BITS, (PRECISION_256 - precisionOffset + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 1); 

	// The below implementation can also be used but I did not since it only affects one limb at a time not true shifting across limbs, not that this is technically required
	// mpn_rshift(limbs + i, limbs + i , 1, 1);
    }
    
    return limbs; 

    /* 
			    Below is the non-loop code of the above for clarity

    // Shift everything 1 times right, leaves one padded 0 in MSL
    mpn_rshift(limbs, limbs, (PRECISION_256 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 1); 
     
    // Now shift limbs[2...0] right 1 time, leaves one padded 0 in limbs[2] - Make sure to reduce the precision used
    mpn_rshift(limbs + (sizeof(uint64_t) * 1)/GMP_NUMB_BITS, limbs + sizeof(uint64_t)/GMP_NUMB_BITS, (PRECISION_256 - 64 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 1);

    // Now shift limbs[1...0] right 1 time, leaves one padded 0 in limbs[1] - Make sure to reduce the precision used
    mpn_rshift(limbs + (sizeof(uint64_t) * 2)/GMP_NUMB_BITS, limbs + (sizeof(uint64_t) * 2)/GMP_NUMB_BITS, (PRECISION_256 - 128 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 1);
    
    // Now shift limbs[0...0] right 1 time, leaves one padded 0 in limbs[0] - Make sure to reduce the precision used
    mpn_rshift(limbs + (sizeof(uint64_t) * 3)/GMP_NUMB_BITS, limbs + (sizeof(uint64_t) * 3)/GMP_NUMB_BITS, (PRECISION_256 - 192 + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS, 1);
			End of example.
    */
}

mp_limb_t* avxmpfr_unpad252(mpfr_t mpfrNumber) // Take an input MPFR variable type  
{
    /*
	Conceptually the reverse of avxmpfr_pad256.
	Takes a set of limbs in padded format e.g., 0111 0101 and transforms it to an MPFR readable number -> 1111 0100

	Since leftshifting between limbs we want to retain information we cannot use the exact same algorithm as avxmpfr_pad256.
	This function uses a combination of bit masks to extract MSBs without padding and shifting to return the correct format.
     */

    // Extract the limbs from the mpfrNumber
    mp_limb_t* limbs = (mp_limb_t *)mpfrNumber->_mpfr_d;
   
    /*
    // Test setting all bits that can be used, to be used 
    limbs[0] = 0x7FFFFFFFFFFFFFFF;
    limbs[1] = 0x7FFFFFFFFFFFFFFF;
    limbs[2] = 0x7FFFFFFFFFFFFFFF;
    limbs[3] = 0x7FFFFFFFFFFFFFFF;
    */   

    // Shift limbs[3...3] once to the left leaving a single 0 LSB.
    mpn_lshift(limbs + 3, limbs + 3, 1, 1);

    // Mask the non-pad MSB of limbs[2...2] to shift into limbs [3...3] and then shift limbs[2...2] over twice leaving two 0 LSBs
    uint64_t  lsb_mask = limbs[2] | 0x8000000000000000;
    limbs[3] = limbs[3] | lsb_mask;
    mpn_lshift(limbs + 2, limbs + 2, 1, 2);

    // Mask the non-pad MSBs of limbs[1...1] to shift into limbs [2...2] and then shift limbs[1...1] over thrice leaving three 0 LSBs
    lsb_mask = limbs[1] | 0x9000000000000000;
    limbs[2] = limbs[2] | lsb_mask;
    mpn_lshift(limbs +1, limbs + 1, 1, 3);
    
    // Mask the non-pad MSBs of limbs[0...0] to shift into limbs [1...1] and then shift limbs[0...0] over four times leaving four 0 LSBs
    lsb_mask = limbs[0] | 0xA000000000000000;
    limbs[1] = limbs[1] | lsb_mask;
    mpn_lshift(limbs, limbs, 1, 4);
    
    return limbs; 
}

/*
int main()
{
    // Main function will be removed when all code is linked

    // Test having a fake mpfr number
    mpfr_t num;
    mpfr_init2(num, 252);
    
    mpfr_set_d(num, 2.75, MPFR_RNDN);

    // Print the current number value 
    mpfr_printf("Value is = %.50Rf\n", num);

   // mpfr_prec_t precision = mpfr_get_prec(num);
    printf(" EXP: %ld \n", mpfr_get_exp(num));
    printf(" LIMBS: ");

    // Test the padding code	
    mp_limb_t *limbs = avxmpfr_pad252(num);
    print_binary(limbs, PRECISION_256);


    // Test unpadding code
    printf("\n\n");
    limbs = avxmpfr_unpad252(num);
    print_binary(limbs, PRECISION_256);

}
*/
