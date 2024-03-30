SRC_FILES := avxmpfr_add.c expAllign.c padLimbs.c intrinsics_add.c intrinsics_add_512i.c avxmpfr_utilities.c comparison.c
EXEC_NAMES := $(SRC_FILES:.c=)

COMMON_FLAGS := -O3 -Wextra -Wall -Wpedantic
SPECIAL_FLAGS := -lmpfr -lgmp -mavx2 -mavx512f -mfma -lrt

build: $(EXEC_NAMES)
	@echo "\nUse -O3 for optimization and -O0 for debugging\n"

avxmpfr_add: avxmpfr_add.c avxmpfr_utilities.c expAllign.c padLimbs.c intrinsics_add.c intrinsics_add_512i.c
	gcc -o $@ $^ $(COMMON_FLAGS) $(SPECIAL_FLAGS)

comparison: comparison.c avxmpfr_add.c avxmpfr_utilities.c expAllign.c padLimbs.c intrinsics_add.c intrinsics_add_512i.c
	gcc -o $@ $^ $(COMMON_FLAGS) $(SPECIAL_FLAGS)

%: %.c
	gcc -o $@ $< $(COMMON_FLAGS) $(SPECIAL_FLAGS)

clean:
	rm -f $(EXEC_NAMES)	${wildcard *.o}
