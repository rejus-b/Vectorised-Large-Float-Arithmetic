# build: intrinsics_test.c
#	gcc -o intrinsics intrinsics_test.c -mavx2 -mfma -O3 -Wextra -Wall -Wpedantic 
#	@echo "Use -O0 during bugtest and -O3 for final"
#
#run: intrinsics
#	./intrinsics



SRC_FILES := $(wildcard *.c)
EXEC_NAMES := $(SRC_FILES:.c=)

COMMON_FLAGS := -O3 -Wextra -Wall -Wpedantic
SPECIAL_FLAGS := -lmpfr -lgmp -mavx2 -mfma

build: $(EXEC_NAMES)
		@echo "\nUse -O3 for optimisation and -O0 for debugging\n"

%: %.c
	gcc -o $@ $< $(COMMON_FLAGS) $(SPECIAL_FLAGS)

clean:
	rm $(EXEC_NAMES)
