SRC_FILES := avxmpfr_add.c expAllign.c padLimbs252.c intrinsics_add.c avxmpfr_utilities.c
EXEC_NAMES := $(SRC_FILES:.c=)

COMMON_FLAGS := -O3 -Wextra -Wall -Wpedantic
SPECIAL_FLAGS := -lmpfr -lgmp -mavx2 -mfma

build: $(EXEC_NAMES)
	@echo "\nUse -O3 for optimization and -O0 for debugging\n"

avxmpfr_add: avxmpfr_add.c avxmpfr_utilities.c expAllign.c padLimbs252.c intrinsics_add.c
	gcc -o $@ $^ $(COMMON_FLAGS) $(SPECIAL_FLAGS)

%: %.c
	gcc -o $@ $< $(COMMON_FLAGS) $(SPECIAL_FLAGS)

clean:
	rm -f $(EXEC_NAMES)	${wildcard *.o}
