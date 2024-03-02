//#include <stdio.h>
//#include <immintrin.h>
#include "avxmpfr_utilities.h"

/*
    Original proposed code for doing addition in parallel with truncated rounding using AVX2.
    By Massimiliano Fasi
	
	Further updates / configurations.
	By Rejus Bulevicius
	
    Known changes include:
	Fixing carry propgation and normalisation when a carry bit is encountered.
*/

// Test if a __m256i variable has all bits set to 0.
int is_all_zeros(__m256i x)
{
    return _mm256_testz_si256(x, x);
}

// Add two __m256i_u variables with truncation if normalisation is required.
__m256i avx_add (const __m256i_u a, const __m256i_u b)
{

    const __m256i_u carry_mask = _mm256_set1_epi64x(0x8000000000000000);
    const __m256i_u result_mask = _mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF);
    __m256i_u result = a;
    __m256i_u carry = b;

    char normalise = 0;
    while (!is_all_zeros(carry))
    {
        result = _mm256_add_epi64(result, carry);     // Add 64-bit integers.
        carry = _mm256_and_si256(result, carry_mask); // Extract the carry bits.
        if (is_all_zeros(carry))                      // No carries.
            break;

	//break; // Not running any of the carry normalisation code makes the addition work on padded numbers
	printf("\nI needed to carry\n");
        // Zero out the carry bit.
        result = _mm256_and_si256(result, result_mask);
        // Shift the carry bits to least significant place.
        carry = _mm256_srl_epi64(carry, _mm_cvtsi32_si128(63));
	hexdump_m256i(carry, "mid-res0");
        // If the most significant limb has a carry, we'll need to normalise at
        // the end. Note that this can only ever happen once if a and b
        // themselves are normalised.
	printf("c0 is : %llx\n", carry[0]);
	printf("c1 is : %lld\n", carry[1]);
	printf("c2 is : %lld\n", carry[2]);
	printf("c3 is : %lld\n", carry[3]);

	hexdump_m256i(carry, "mid-res");
        normalise = (carry[0] != 0);             /// I personally believe the order here is wrong for carry should be 0 not 3a

        // This is how I do the left shift across lanes.
        carry = _mm256_set_epi64x(0x0,
                                  carry[3],	// This used to be c[0] .. c[2]
                                  carry[2],
                                  carry[1]);
	hexdump_m256i(carry, "mid-res2");
    }


    // Normalise the result with truncation.
    if (normalise)
    {
    printf("NOrmalised\n");
        // Extract bits to be shifted right across lanes.
        const __m256i_u last_bit_mask = _mm256_set1_epi64x(0x0000000000000001);
        __m256i_u last_bit = _mm256_and_si256(result, last_bit_mask);

        // Extract the least significant bit of the result.
        // Unused in my implementation, but could be used for rounding.
        int rounding_bit = last_bit[0];

        // Shift the bit right across lanes. The carry position is skipped,
        // hence the shift by 62 instead of 63.
        __m256i_u top_bit = _mm256_sll_epi64(last_bit, _mm_cvtsi32_si128(62));
        top_bit = _mm256_set_epi64x(0x4000000000000000,
                                    top_bit[0],
                                    top_bit[1],
                                    top_bit[2]);
        result = _mm256_srl_epi64(result, _mm_cvtsi32_si128(1));
        result = _mm256_or_si256(result, top_bit);
        // Perhaps rounding_bit can be used to round here. For now, the result
        // is truncated and the rounding_bit is optimised away.
    }

    return result;
}

/*
int main ()
{

    __m256i_u a = _mm256_set1_epi64x(0x0FFFFFFFFFFFFFFF);
    //__m256i_u b = _mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF);
    __m256i_u b = _mm256_set_epi64x(0x7FFFFFFFFFFFFFFF,
				    0x7FFFFFFFFFFFFFFF,
				    0x7FFFFFFFFFFFFFFF,
				    0x7000000000000000);
    printf("b3 is : %llx\n", b[3]);

    //__m256i_u b = _mm256_set1_epi64x(0x0F700000000000001);
 //   hexdump_m256i(a, "a");
 //   hexdump_m256i(b, "b");

    __m256i_u result = avx_add(a, b);
    hexdump_m256i(a, "a");
    hexdump_m256i(b, "b");
    hexdump_m256i(result, "result");
}
*/