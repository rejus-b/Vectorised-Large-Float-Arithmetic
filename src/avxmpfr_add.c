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
    avxmpfr_exp_allign(&op1, &op2);

    mp_limb_t *limbs = (mp_limb_t *)op1->_mpfr_d; 
    print_binary(limbs, PRECISION_256);
    // Now pad the limbs of these numbers
    op1->_mpfr_d = avxmpfr_pad252(op1);
    op2->_mpfr_d = avxmpfr_pad252(op2);

    // Now you can add these numbers and assign to rop
}

int main()
{
return 0;
}
