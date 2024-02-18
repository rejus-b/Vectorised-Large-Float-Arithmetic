/* avxmpfr_utilities.h 

    Code developed to investigate the effectiveness of AVX instructions in MPFR.
    Undertaken as a project for the bachelors of Computer Science at the University of Leeds year 23/24.
    Developed by Rejus Bulevicius with supervision from Mantas Mikaitis and additional supervision from Massimiliano Fasi.

    Code is held under the GNU Lesser General Public Lisence.
    It is built upon the GNU MPFR-4.2.1 library and GMP-6.3.0 library. 

*/

#ifndef AVXMPFR_UTILITIES_H
#define AVXMPFR_UTILITIES_H 

// Required includes
#include <stdio.h>
#include <mpfr.h>
#include <stdint.h>

// Lets define some macros 
#define PRECISION_512 512 
#define PRECISION_256 256

// Note that all limbs are printed from the least significant limb (LSL) first to the most significant limb (MSL)
void print_binary(const mp_limb_t *limbs, mpfr_prec_t precision)
{
    for (mpfr_prec_t i = 0; i < (precision + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS; ++i)
    {
        for (int j = GMP_NUMB_BITS - 1; j >= 0; --j)
        {
            printf("%ld", (limbs[i] >> j) & 1);
        }
        printf(" ");
    }
}

#endif // AVXMPFR_UTILITIES_H
