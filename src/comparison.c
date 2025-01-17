/*
    Test file to compare the speedup of avxmpfr if any, in regards to mpfr.
    It will also test the correctness of the binary representation.

    mpfr_add() will only be called with MPFR_RNDN as this is all avxmpfr implements.
*/

#include "avxmpfr_utilities.h"
#include <time.h>
#include <stdlib.h>

void assign_binary(char* binNum)
{
    // Initialise array size 
    const int size = 254;

    // Set a random value to a decimal point 
    int pointLocation = rand() % 252;

    for (int i = 0; i < size - 1; i++)
    {
	if (i == pointLocation)
	    binNum[i] = '.';
	else
	    binNum[i] = '0'+ (rand() % 2); // Clamp to char 0 or 1 
    }

    // Set the null terminator
    binNum[size - 1] = '\0';
    
    return;
}

void assign_binary_504(char* binNum)
{
    // Initialise array size 
    const int size = 506;

    // Set a random value to a decimal point 
    int pointLocation = rand() % 504;

    for (int i = 0; i < size - 1; i++)
    {
	if (i == pointLocation)
	    binNum[i] = '.';
	else
	    binNum[i] = '0'+ (rand() % 2); // Clamp to char 0 or 1 
    }

    // Set the null terminator
    binNum[size - 1] = '\0';
    
    return;
}

int main()
{
    // Make it so that it automatically prints the numbers after the long wait
    setbuf(stdout, NULL);  // Disable buffering for stdout

    // Set a random seed for number generation
    srand(time(NULL));
   
     // Set up an array long enough to act as a 252 string 
    char first_bin[254];	// 252 bits of precision +1 for '\0' and +1 for a '.'   
    char second_bin[254];
	
    // Set up an array long enough to act as a 504 string
    char first_bin_512[506];	// 504 bits of precision +1 for '\0' and +1 for a '.'
    char second_bin_512[506];

    // Initialise some variables
    uint16_t PRECISION = PRECISION_256;	// Set the precision you want to compare
    uint64_t total = 0;			// How many values are correct against mpfr_add()
    clock_t start, end;			// Operation start and end time
    mpfr_t mpfr_time;			// How long it takes to execute mpfr_add()
    mpfr_t avxmpfr_time;		// How long it takes to execute avxmpfr_add() 
    char debug = 0;				// If debug is 1 print out the variables and limbs
    uint64_t iterations = 1<<15;// 1<<25 in actual timing cases 

    // Initialise some mpfr_t variables for storing the time
    mpfr_inits2(256, mpfr_time, avxmpfr_time, NULL);
    mpfr_set_str(mpfr_time, "0", 10, MPFR_RNDN);  
    mpfr_set_str(avxmpfr_time, "0", 10, MPFR_RNDN);  

    // Initialise the mpfr_t numbers
    mpfr_t number1, number2, number1_512, number2_512, mpfr_result, avxmpfr_result, mpfr_result_512, avxmpfr_result_512;
    mpfr_inits2(PRECISION_256, number1, number2, mpfr_result, avxmpfr_result, NULL);
    mpfr_inits2(PRECISION_512, number1_512, number2_512, mpfr_result_512, avxmpfr_result_512, NULL);

    //  Test it total of 33,554,432 iterations
    for(uint32_t i = 0; i < iterations; i++)
    {
		// Assign a 252 binary value
		assign_binary(first_bin);
		assign_binary(second_bin);
		
		// Assign a 504 binary value
		assign_binary_504(first_bin_512);
		assign_binary_504(second_bin_512);
		
		// Assign the mpfr_t numbers
		mpfr_set_str(number1, first_bin, 2, MPFR_RNDN);
		// Print the binary limbs aswell
//printf("\nEXP: %ld\n", (number1)->_mpfr_exp);
//mp_limb_t* mpfr_limbs = (mp_limb_t *) number1->_mpfr_d;
//print_binary(mpfr_limbs, PRECISION_256);
//printf("\n");
		mpfr_set_str(number2, second_bin, 2, MPFR_RNDN);
		
		mpfr_set_str(number1_512, first_bin_512, 2, MPFR_RNDN);
		// Print the binary limbs aswell
//printf("\nEXP: %ld\n", (number1_512)->_mpfr_exp);
// mpfr_limbs = (mp_limb_t *) number1_512->_mpfr_d;
//print_binary(mpfr_limbs, PRECISION_512);
//printf("\n");
		
		mpfr_set_str(number2_512, second_bin_512, 2, MPFR_RNDN);

		
		if (PRECISION == PRECISION_256)
		{
			// mpfr_add()
			if (debug)
			{
				// Use mpfr_add() and print the result
				printf("\n\t\t mpfr_add()\n\n");
			}

			start = clock();
			mpfr_add(mpfr_result, number1, number2, MPFR_RNDF); // Setting it to faithful rounding makes it no longer fail
			end = clock();
			mpfr_add_d(mpfr_time, mpfr_time, ((double) (end - start) / CLOCKS_PER_SEC), MPFR_RNDN);		// ((double) (end - start)) / CLOCKS_PER_SEC;

			if (debug)
			{ 
			   mpfr_printf("\n%.252Rf\n", mpfr_result);

				// Print the binary limbs aswell
				printf("\nEXP: %ld\n", (mpfr_result)->_mpfr_exp);
				mp_limb_t* mpfr_limbs = (mp_limb_t *) mpfr_result->_mpfr_d;
				print_binary(mpfr_limbs, PRECISION_256);
				printf("\n"); 
			}

			// avxmpfr_add()
			if (debug)
			{ 
				// Use avxmpfr_add() and print the result
				printf("\n\t\t avxmpfr_add()\n\n");
			}

			start = clock();
			avxmpfr_add(avxmpfr_result, number1, number2, MPFR_RNDF, PRECISION_256);
			end = clock();
			mpfr_add_d(avxmpfr_time, avxmpfr_time, ((double) (end - start) / CLOCKS_PER_SEC), MPFR_RNDN);		// ((double) (end - start)) / CLOCKS_PER_SEC;
			//avxmpfr_time += ((double) (end - start)) / CLOCKS_PER_SEC;

			if (debug)
			{
				mpfr_printf("\n%.252Rf\n", avxmpfr_result);

				// Print the binary limbs again
				printf("\nEXP: %ld\n", (avxmpfr_result)->_mpfr_exp);
				mp_limb_t* avxmpfr_limbs = (mp_limb_t *) avxmpfr_result->_mpfr_d;
				print_binary(avxmpfr_limbs, PRECISION_256);
				printf("\n");
			}

			int cmp_result = mpfr_equal_p(mpfr_result, avxmpfr_result);
			total += cmp_result;

			//printf("\n\nComparison return: %i\n\n", cmp_result);  
			if (cmp_result == 0)
			{
				printf("\n\x1b[31mLimbs are unequal\x1b[0m\n\n");
				break;
			}
		}
		
		
		
		// If you want to test 512 precision
		if (PRECISION == PRECISION_512)
		{
			// mpfr_add()
			if (debug)
			{
				// Use mpfr_add() and print the result
				printf("\n\t\t mpfr_add()\n\n");
			}

			start = clock();
			mpfr_add(mpfr_result_512, number1_512, number2_512, MPFR_RNDF); // Setting it to faithful rounding makes it no longer fail
			end = clock();
			mpfr_add_d(mpfr_time, mpfr_time, ((double) (end - start) / CLOCKS_PER_SEC), MPFR_RNDN);		// ((double) (end - start)) / CLOCKS_PER_SEC;

			if (debug)
			{ 
			   mpfr_printf("\n%.512Rf\n", mpfr_result_512);

				// Print the binary limbs aswell
				printf("\nEXP: %ld\n", (mpfr_result_512)->_mpfr_exp);
				mp_limb_t* mpfr_limbs = (mp_limb_t *) mpfr_result->_mpfr_d;
				print_binary(mpfr_limbs, PRECISION_512);
				printf("\n"); 
			}

			// avxmpfr_add_512()
			if (debug)
			{ 
				// Use avxmpfr_add() and print the result
				printf("\n\t\t avxmpfr_add()\n\n");
			}

			start = clock();
			avxmpfr_add_512(avxmpfr_result_512, number1_512, number2_512, MPFR_RNDF, PRECISION_512);
			end = clock();
			mpfr_add_d(avxmpfr_time, avxmpfr_time, ((double) (end - start) / CLOCKS_PER_SEC), MPFR_RNDN);		// ((double) (end - start)) / CLOCKS_PER_SEC;
			//avxmpfr_time += ((double) (end - start)) / CLOCKS_PER_SEC;

			if (debug)
			{
				mpfr_printf("\n%.512Rf\n", avxmpfr_result_512);

				// Print the binary limbs again
				printf("\nEXP: %ld\n", (avxmpfr_result_512)->_mpfr_exp);
				mp_limb_t* avxmpfr_limbs = (mp_limb_t *) avxmpfr_result->_mpfr_d;
				print_binary(avxmpfr_limbs, PRECISION_512);
				printf("\n");
			}

			int cmp_result = mpfr_equal_p(mpfr_result_512, avxmpfr_result_512);
			total += cmp_result;

			//printf("\n\nComparison return: %i\n\n", cmp_result);  
			if (cmp_result == 0)
			{
				printf("\n\x1b[31mLimbs are unequal\x1b[0m\n\n");
				break;
			}
		}
			
    }

    printf("\n\nMatch value : %ld", total / iterations); // If 1 complete match, else no
    printf("\nTotal matches : %ld\n", total);
    if (total / (1<<25))
	printf("\n\x1b[32mLimbs are equal\x1b[0m\n\n");

    // Print the time cases
    mpfr_printf("\nTime taken for mpfr_add():\t\t %.128Rf seconds\n", mpfr_time); 
    mpfr_printf("\nTime taken for avxmpfr_add():\t\t %.128Rf seconds\n", avxmpfr_time); 

    mpfr_div_ui(mpfr_time, mpfr_time, iterations, MPFR_RNDN);
    mpfr_div_ui(avxmpfr_time, avxmpfr_time, iterations, MPFR_RNDN);

    mpfr_printf("\nAverage time taken for mpfr_add():\t %.128Rf seconds\n", mpfr_time);
    mpfr_printf("Average time taken for avxmpfr_add():\t %.128Rf seconds\n", avxmpfr_time);

    return 0;
}
