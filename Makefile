.PHONY: build
build:
	mkdir -p build
	cd build && cmake .. && make

.PHONY: clean
clean: 
	rm -rf ./build

.PHONY: test
test: build
	./build/const_mapper_tests
