CXX = g++
OBJS = 
INCLUDES = -I"."
LIBS = -L./fprotocol -lfprotocol -L./fcore/ -lfcore -lpthreadGC2 -lwsock32 #-lws2_32
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -D__WIN32__ $(INCLUDES) -D_DEBUG_ -D_DEBUG_MUTEX_
LDFLAGS = $(LIBS) #-mwindows

TARGET = fProxy.exe fClient.exe test.exe

all: flibs $(TARGET) 

.PHONY: flibs
flibs:
	cd fcore && make
	cd fprotocol && make

%.exe: %.o $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
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
