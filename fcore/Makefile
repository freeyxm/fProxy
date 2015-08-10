INCLUDES = -I../
#LIBS = -lpthreadGC2 -lwsock32 -lws2_32
#PLATFORM = -D__linux__ #-D__WIN32__
MACROS = $(PLATFORM) -DLOG_LEVEL=3 -D_DEBUG_
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -std=c++11 $(INCLUDES) $(MACROS)
LDFLAGS =
OBJS = FSocket.o FSocketDomain.o FSocketDomain4.o FSocketDomain6.o FSocketTcp.o FSocketUdp.o FThread.o FUtil.o FLogger.o FPmTest.o FString.o FThreadPool.o FThreadTask.o FPtrMgr.o
SRCS = ${OBJS:%.o=%.cpp}

TARGET = libfcore.a
OUT_DIR = ../libs

all: depend $(TARGET)

depend:
	$(CXX) $(INCLUDES) -MM $(SRCS) > .depend

-include .depend

libfcore.a: $(OBJS)
	ar -rs "$(OUT_DIR)/$(TARGET)" $(OBJS)

.PHONY: clean
clean:
	rm -rf $(OBJS) "$(OUT_DIR)/$(TARGET)"

.PHONY: clean_win32
clean_win32:
	del /F/Q $(OBJS) "$(OUT_DIR)/$(TARGET)"
