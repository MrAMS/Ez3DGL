init:
	-rm -rf build
	mkdir build/ && cd build/ && cmake ..

build:
	cmake --build build -j`nproc`

run: build
	./build/main

.PHONY: init build run

