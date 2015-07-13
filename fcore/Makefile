OBJS = FSocket.o FSocketTcp.o FSocketUdp.o FServer.o FThread.o FUtil.o FPmTest.o FString.o
#LIBS = -lpthreadGC2 -lwsock32 -lws2_32
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -D__WIN32__ -D_DEBUG_ -D_DEBUG_MUTEX_ 
LDFLAGS = 

TARGET = libfcore.a

all: $(TARGET)

libfcore.a: $(OBJS)
	ar -rs $@ $^

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET)

.PHONY: clean_win32
clean_win32:
	del /F/Q $(OBJS) $(TARGET)
