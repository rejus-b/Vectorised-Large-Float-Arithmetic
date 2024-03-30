#include "avxmpfr_utilities.h"

/*
	Truncated rounding add using AVX512 instructions.
*/

// Test if a __m512i variable has all bits set to 0.
inline int is_all_zeros_512i(__m512i x)
{
	// Compares 64 bit masks against the value of 0, returns 1 if all zero
    return _mm512_test_epi64_mask(x, x) == 0;
}

// Add two __m512i_u variables with truncation if normalisation is required.
__m512i avx_add_512i (const __m512i_u a, const __m512i_u b, mpfr_exp_t* exponent)
{

	const __m512i carry_mask = _mm512_set1_epi64(0x8000000000000000);
	const __m512i result_mask = _mm512_set1_epi64(0x7FFFFFFFFFFFFFFF);
	__m512i result = a;
	__m512i carry = b;

    char normalise = 0;
    while (!is_all_zeros_512i(carry))
    {
	// Add code here to test if MSB of most significant lane is currently 1
	// If it is, you know there has been a carry over in previous iterations
	// You can run the normalisation if this is the case		

        result = _mm512_add_epi64(result, carry);     // Add 64-bit integers.
//	hexdump_m512i(result, "res rn");
        carry = _mm512_and_si512(result, carry_mask); // Extract the carry bits.
        if (is_all_zeros_512i(carry))                      // No carries.
            break;

        // Zero out the carry bit.
        result = _mm512_and_si512(result, result_mask);
//	hexdump_m512i(result, "res post zero");
        // Shift the carry bits to least significant place.
        carry = _mm512_srlv_epi64(carry, _mm512_set1_epi64(63));
//	hexdump_m512i(carry, "mid-res0");

        // If the most significant limb has a carry, we'll need to normalise at
        // the end. Note that this can only ever happen once if a and b
        // themselves are normalised.
		
//	printf("c0 is : %llx\n", carry[0]);
//	printf("c1 is : %lld\n", carry[1]);
//	printf("c2 is : %lld\n", carry[2]);
//	printf("c3 is : %lld\n", carry[3]);

//	hexdump_m512i(carry, "mid-res");
        normalise = (carry[0] != 0);             // I personally believe the order here is wrong for carry should be 0 not 3

        // This is how I do the left shift across lanes.
        carry = _mm512_set_epi64(0x0,
								carry[7],
								carry[6],
								carry[5],
								carry[4],
								carry[3],
								carry[2],
								carry[1]);
//	hexdump_m512i(carry, "mid-res2");
//	hexdump_m512i(result, "res end");
    }


    // Normalise the result with truncation.
    if (normalise)
    {
//    printf("NOrmalised\n");
        // Extract bits to be shifted right across lanes.
        const __m512i_u last_bit_mask = _mm512_set1_epi64(0x0000000000000001);
//	hexdump_m512i(result, "mask end");
        __m512i_u last_bit = _mm512_and_si512(result, last_bit_mask);
//	hexdump_m256i(last_bit, "lasbit");


        // Extract the least significant bit of the result.
        int rounding_bit = last_bit[7]; // Used to be 0
//	printf("\n%lld b", rounding_bit);

        // Shift the bit right across lanes. The carry position is skipped,
        // hence the shift by 62 instead of 63.
        __m512i_u top_bit = _mm512_sllv_epi64(last_bit, _mm512_set1_epi64(62));
        top_bit = _mm512_set_epi64(top_bit[6],
									top_bit[5],
									top_bit[4],
									top_bit[3],
									top_bit[2], // 0x4000000000000000 , 0, 1, 2 - Original values
                                    top_bit[1],
                                    top_bit[0],
                                    0x4000000000000000); // This value was also too short as it gets right shifted instantly after
//	hexdump_m512i(top_bit, "top bit");
        result = _mm512_srl_epi64(result, _mm_cvtsi32_si128(1));
//	hexdump_m512i(result, "normal1");
        result = _mm512_or_si512(result, top_bit);
//	hexdump_m256i(result, "normal2");

	// Increase the exponent to compensate for the right shift in for storing the overflow
//	printf("\n\n exp is: %ld \n\n", *exponent);
	(*exponent) ++;
//	printf("\n\n exp now: %ld \n\n", *exponent);

        // Perhaps rounding_bit can be used to round here. For now, the result
        // is truncated and the rounding_bit is optimised away.

	// Using roundToNearest and faithful rounding (You can remove the rounding entirely for faithful)
//	if (rounding_bit)
//	    result = _mm256_set_epi64x(result[0] & 0xFFFFFFFFFFFFFFFE,
//					result[1],
//					result[2],
//					result[3]);
   } // Normalisation

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
