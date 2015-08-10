export PLATFORM = -D__linux__ #-D__WIN32__

all: 
	cd fcore && make
	cd fprotocol && make
	cd demo && make

.PHONY: clean
clean:
	cd fcore && make clean
	cd fprotocol && make clean
	cd demo && make clean

.PHONY: clean_win32
clean_win32:
	cd fcore && make clean_win32
	cd fprotocol && make clean_win32
	cd demo && make clean_win32
