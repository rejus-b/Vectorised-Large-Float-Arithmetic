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
#include <immintrin.h>

// Lets define some macros 
#define PRECISION_512 512 
#define PRECISION_256 256


// Now to define all the functions
void print_binary(const mp_limb_t *limbs, mpfr_prec_t precision);
void hexdump_m256i(const __m256i values, const char* name);

void avxmpfr_exp_allign(mpfr_t *firstNum, mpfr_t *secondNum);

int is_all_zeros(__m256i x);
__m256i avx_add (const __m256i_u a, const __m256i_u b);

mp_limb_t* avxmpfr_pad252(mpfr_t mpfrNumber);
mp_limb_t* avxmpfr_unpad252(mpfr_t mpfrNumber);

#endif // AVXMPFR_UTILITIES_H
