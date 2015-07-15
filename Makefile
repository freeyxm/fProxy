INCLUDES = -I"."
#LIBS = -L./fprotocol -lfprotocol -L./fcore/ -lfcore -lpthreadGC2 -lwsock32 #-lws2_32
LIBS = -L./fprotocol -lfprotocol -L./fcore/ -lfcore -lpthread
MACROS = -D_DEBUG_ -D_LOG_MUTEX_ #-D__WIN32__
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 $(INCLUDES) $(MACROS)
LDFLAGS = $(LIBS) #-mwindows
OBJS = 

TARGET = fProxy.exe fClient.exe test.exe

all: flibs $(TARGET) 

.PHONY: flibs
flibs:
	cd fcore && make
	cd fprotocol && make

fProxy.exe: fProxy.o fcore/libfcore.a fprotocol/libfprotocol.a
	$(CXX) -o $@ fProxy.o $(LDFLAGS)

fClient.exe: fClient.o fcore/libfcore.a fprotocol/libfprotocol.a
	$(CXX) -o $@ fClient.o $(LDFLAGS)

test.exe: test.o $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	cd fcore && make clean
	cd fprotocol && make clean
	rm -rf $(OBJS) $(TARGET)
	
.PHONY: cleanall
cleanall:
	cd fcore && make clean
	cd fprotocol && make clean
	make clean

.PHONY: clean_win32
clean_win32:
	del /F/Q *.o $(TARGET)

.PHONY: cleanall_win32
cleanall_win32:
	cd fcore && make clean_win32
	cd fprotocol && make clean_win32
	make clean_win32
