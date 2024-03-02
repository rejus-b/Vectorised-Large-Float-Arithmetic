/*
    The code linking all of avx / mpfr code to create an avxmpfr_add().
*/

#include "avxmpfr_utilities.h"

void avxmpfr_add(mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd, uint16_t precision)
{
    /*
	rop is resultant operand
	op1 is first operand
	op2 is second operand
	rnd is rounding mode
	precision is the precision of the avx lanes you want to use and the assumed precision of your mpfr_number (Either PRECISION_256 / PRECISION_512)

	Note precision is not currently implemented
    */

    // First allign the exponents of the numbers to be added 
    avxmpfr_exp_allign(op1, op2);

    // Now pad the limbs of these numbers
    op1->_mpfr_d = avxmpfr_pad252(op1);
    op2->_mpfr_d = avxmpfr_pad252(op2);

    // Now you can add these numbers and assign to rop
    // Note that you have to create a set of packed integers for the AVX lanes
    
    // Set the first AVX register
    __m256i_u op1_avx = _mm256_set_epi64x(op1->_mpfr_d[0],  // The least significant AVX lane / MPFR limb
					    op1->_mpfr_d[1],
					    op1->_mpfr_d[2],
					    op1->_mpfr_d[3]);
    
    __m256i_u op2_avx = _mm256_set_epi64x(op2->_mpfr_d[0],  // The least significant AVX lane / MPFR limb
					    op2->_mpfr_d[1],
					    op2->_mpfr_d[2],
					    op2->_mpfr_d[3]);

    // Now you can add these
    __m256i_u rop_avx = avx_add(op1_avx, op2_avx);
    
    // Now assign them to the actual rop
    rop->_mpfr_d[0] = rop_avx[0];
    rop->_mpfr_d[1] = rop_avx[1];
    rop->_mpfr_d[2] = rop_avx[2];
    rop->_mpfr_d[3] = rop_avx[3];
     
    // Finally unpad rop
    rop->_mpfr_d = avxmpfr_unpad252(rop);

}

int main() {

    mpfr_t number1, number2, result;

    // Set the numbers
    mpfr_inits2(PRECISION_256, number1, number2, result, NULL);
   
    // Assign the numbers 
    mpfr_set_str(number1, "1.25", 10, MPFR_RNDN);
    mpfr_set_str(number2, "2.25", 10, MPFR_RNDN);
 
    // Get a copy of the limbs prior and exp 
    printf("Exponents and limbs prior: \n");

    printf("Exp: %ld \n", (number1)->_mpfr_exp);
    mp_limb_t *limbs = (mp_limb_t *)number1->_mpfr_d; 
    print_binary(limbs, PRECISION_256);

    printf("\n");
//    printf("Exp: %ld \n", (number2)->_mpfr_exp);
 //   limbs = (mp_limb_t *)number2->_mpfr_d; 
  //  print_binary(limbs, PRECISION_256);
   

    // Test that different exponents are found and alligned when less than or equal to 64
    avxmpfr_add(result, number1, number2, MPFR_RNDN, PRECISION_256);
   
    printf("\nAfter avxmpfr_add"); 
    printf("Exp: %ld \n", (result)->_mpfr_exp);
    limbs = (mp_limb_t *)result->_mpfr_d; 
    print_binary(limbs, PRECISION_256);

    return 0;
}
