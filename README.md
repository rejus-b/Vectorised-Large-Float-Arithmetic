# UGR_PROJECT_23-24
Undergraduate project at University of Leeds (23/24)

Supervisor: Mantas Mikaitis - https://mmikaitis.github.io/

Supervisee: Rejus Bulevicius - https://github.com/rejus-b/

Additional guidance : Massimiliano Fasi - https://github.com/mfasi

# Project description 
Application of compiler intrinsics in floating point arithmetic using GNU MPFR.

Aiming to use compiler intrinsics to vectorise floating point arithmetic, seeing if would be more efficient than the current implementation in MPFR.

# Documentation
There is currently no explicit documentation for provided code, most files have a short description at the start.

# Dependencies
It is built upon the GNU MPFR-4.2.1 library and GMP-6.3.0 library. 

Ensure you have these installed beforehand, other versions have not been tested for compatability. 

Since this library aims to use AVX instructions, ensure you have a compatible processor with at least AVX2, some code will require AVX512.
