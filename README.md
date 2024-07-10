# Vectorised-Large-Float-Arithmetic / UGR_PROJECT_23-24
Undergraduate project at University of Leeds (23/24)

Supervisor: Mantas Mikaitis - https://mmikaitis.github.io/

Assessor: Nishant Ravikumar - https://www.researchgate.net/profile/Nishant-Ravikumar

Supervisee: Rejus Bulevicius - https://github.com/rejus-b/

Additional guidance: Massimiliano Fasi - https://github.com/mfasi

# Project description 
Application of compiler intrinsics in floating point arithmetic using GNU MPFR. A detailed report of the investigation and creaton of said prototype can be found [here](UGR_Project.pdf).

The report and project aimed to use compiler intrinsics to vectorise floating point arithmetic, seeing if would be more efficient than the current implementation in MPFR.

The final implementation found that it was not inherintely more efficient, but it had the potential for gain. Given more time there is an incentive to improve the project, but instead bypassing the GNU MPFR library to create an independent library of highly optimised arbitrary float arithmetic that could then be directly injected or customised for any arbitrary precision library that wishes to utilise it to effect. 

There is future potential for work including different arithmetic operations and further rounding modes. 

# Documentation
Refer to the [report](UGR_Project.pdf) as the main documentation for the choices and style of the project. Code is commented as much as possible to be made clear in use, however, where time was limited some comments may be missing or otherwise incoherent. 

The prototype requires some user editting in [comparison.c](src/comparison.c) depending on what features you want to compare. Otherwise follow the following make instructions.

```
make comparison
./comparison
make clean
```

# Dependencies
It is built upon the GNU MPFR-4.2.1 library and GMP-6.3.0 library. 

Ensure you have these installed beforehand, other versions have not been tested for compatability. 

Since this library aims to use AVX instructions, ensure you have a compatible processor with at least AVX2, some code will require AVX512, the code is also built with the assumption that you have a 64-bit processor. 
