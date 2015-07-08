CFLAGS += 

a.out: *.cu *.cpp
	nvcc -arch=sm_20 -I$(CUDA_PATH)/samples/common/inc/ $^

build: a.out

run:
	bash run_tests.sh

clean:
	rm a.out

.PHONY: clean
