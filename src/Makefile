build: intrinsics_test.c
	gcc -o intrinsics intrinsics_test.c -mavx2 -mfma -O3 -Wextra -Wall -Wpedantic 
	@echo "Use -O0 during bugtest and -O3 for final"

run: intrinsics
	./intrinsics
