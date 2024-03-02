#include "avxmpfr_utilities.h"

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

// Note that lanes are printed from the least significant to the most significant
// Print the contents of a __m256i variable in hex.
void hexdump_m256i(const __m256i values, const char* name)
{
    __uint64_t *val = (__uint64_t*) &values;
    for (int i = 0; i < 4; i++)
	{
        printf("%016lX ", val[i]);
    }
    printf(" = %s\n", name);
}
