all:
	mkdir -p build && cd build && cmake .. && make install

clean:
	cd build && make clean && cd .. && rm -rf build && rm -rf bin
