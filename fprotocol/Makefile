INCLUDES = -I../
#LIBS = -lpthreadGC2 -lwsock32 -lws2_32
MACROS = -D_DEBUG_ -D_LOG_MUTEX_ -DLOG_LEVEL=0 #-D__WIN32__
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -std=c++11 $(INCLUDES) $(MACROS)
LDFLAGS =
OBJS = FProtocol.o FPSocks4.o FPSocks5.o FPHttp.o FPSocks52.o
SRCS = ${OBJS:%.o=%.cpp}

TARGET = libfprotocol.a
OUT_DIR = ../libs

all: depend $(TARGET)

depend:
	$(CXX) $(INCLUDES) -MM $(SRCS) > .depend

-include .depend

libfprotocol.a: $(OBJS)
	ar -rs "$(OUT_DIR)/$(TARGET)" $(OBJS)

.PHONY: clean
clean:
	rm -rf $(OBJS) "$(OUT_DIR)/$(TARGET)"

.PHONY: clean_win32
clean_win32:
	del /F/Q $(OBJS) "$(OUT_DIR)/$(TARGET)"
