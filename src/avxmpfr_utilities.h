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
#define PRECISION_512 504 
#define PRECISION_256 252


// Now to define all the functions
void print_binary(const mp_limb_t *limbs, mpfr_prec_t precision);
void hexdump_m256i(const __m256i values, const char* name);
void hexdump_m512i(const __m512i values, const char* name);

mpfr_exp_t avxmpfr_exp_allign(mpfr_t firstNum, mpfr_t secondNum, const uint16_t PRECISION);

int is_all_zeros(__m256i x);
__m256i avx_add (const __m256i_u a, const __m256i_u b, mpfr_exp_t* exponent);

int is_all_zeroes_512i(__m512i x);
__m512i avx_add_512i (const __m512i_u a, const __m512i_u b, mpfr_exp_t* exponent);

mp_limb_t* avxmpfr_pad252(mpfr_t mpfrNumber);
mp_limb_t* avxmpfr_unpad252(mpfr_t mpfrNumber);
mp_limb_t* avxmpfr_pad504(mpfr_t mpfrNumber);
mp_limb_t* avxmpfr_unpad504(mpfr_t mpfrNumber);

void avxmpfr_add(mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd, const uint16_t PRECISION);

#endif // AVXMPFR_UTILITIES_H
