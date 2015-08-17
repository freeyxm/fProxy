Kernel_Release = $(shell uname -r | awk -F'-amd64' '{print $$1}')
INCLUDES = -I../ -I/usr/src/linux-headers-${Kernel_Release}-common/include
#LIBS = -lpthreadGC2 -lwsock32 -lws2_32
#PLATFORM = -D__linux__ #-D__WIN32__
MACROS = $(PLATFORM) -DLOG_LEVEL=0 -D_DEBUG_
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -std=c++11 $(INCLUDES) $(MACROS)
LDFLAGS =
OBJS = FSocket.o FSocketDomain.o FSocketDomain4.o FSocketDomain6.o FSocketTcp.o FSocketUdp.o\
       FUtil.o FLogger.o FPmTest.o FString.o FPtrMgr.o\
       FThread.o FThreadPool.o FThreadTask.o\
       epoll/EpollWatcher.o epoll/EpollScheduler.o epoll/EpollTask.o
SRCS = ${OBJS:%.o=%.cpp}

TARGET = libfcore.a
OUT_DIR = ../libs

all: depend $(TARGET)

depend:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(SRCS) > .depend

-include .depend

libfcore.a: $(OBJS)
	ar -rs "$(OUT_DIR)/$(TARGET)" $(OBJS)

.PHONY: clean
clean:
	rm -rf $(OBJS) "$(OUT_DIR)/$(TARGET)"

.PHONY: clean_win32
clean_win32:
	del /F/Q $(OBJS) "$(OUT_DIR)/$(TARGET)"
