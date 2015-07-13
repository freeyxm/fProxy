OBJS = FProtocol.o FPSocks4.o FPSocks5.o FPHttp.o
INCLUDES = -I../
#LIBS = -lpthreadGC2 -lwsock32 -lws2_32
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -D__WIN32__ $(INCLUDES) -D_DEBUG_ -D_DEBUG_MUTEX_
LDFLAGS = 

TARGET = libfprotocol.a

all: $(TARGET)

libfprotocol.a: $(OBJS)
	ar -rs $@ $^

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET)

.PHONY: clean_win32
clean_win32:
	del /F/Q $(OBJS) $(TARGET)
