LIB_DIR = ../libs
INCLUDES = -I"../"
#LIBS = -L./fprotocol -lfprotocol -L./fcore/ -lfcore -lpthreadGC2 -lwsock32 #-lws2_32
LIBS = -L$(LIB_DIR) -lfprotocol -lfcore -lpthread
#PLATFORM = -D__linux__ #-D__WIN32__
MACROS = $(PLATFORM) -DLOG_LEVEL=0 -D_DEBUG_
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 -std=c++11 $(INCLUDES) $(MACROS)
LDFLAGS = $(LIBS) #-mwindows
OBJS = ../util/FServer.o ../util/FServerTask.o main.o
SRCS = ${OBJS:%.o=%.cpp} main.cpp fProxy.cpp fClient.cpp test.cpp testThread.cpp

TARGET = fProxy.exe fClient.exe test.exe testThread.exe

all: depend $(TARGET)

depend:
	$(CXX) $(INCLUDES) -MM $(SRCS) > .depend

-include .depend

fProxy.exe: fProxy.o $(OBJS) $(LIB_DIR)/libfcore.a $(LIB_DIR)/libfprotocol.a
	$(CXX) -c main.cpp $(CXXFLAGS) -D TARGET_SERVER
	$(CXX) -o $@ fProxy.o $(OBJS) $(LDFLAGS)

fClient.exe: fClient.o $(OBJS) $(LIB_DIR)/libfcore.a $(LIB_DIR)/libfprotocol.a
	$(CXX) -c main.cpp $(CXXFLAGS) -D TARGET_CLIENT
	$(CXX) -o $@ fClient.o $(OBJS) $(LDFLAGS)

test.exe: test.o $(OBJS) $(LIB_DIR)/libfcore.a $(LIB_DIR)/libfprotocol.a
	$(CXX) -c main.cpp $(CXXFLAGS) -D TARGET_TEST
	$(CXX) -o $@ test.o $(OBJS) $(LDFLAGS)

testThread.exe: testThread.o $(OBJS) $(LIB_DIR)/libfcore.a
	$(CXX) -c main.cpp $(CXXFLAGS) -D TARGET_TEST_THREAD
	$(CXX) -o $@ testThread.o $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o $(OBJS) $(TARGET)

.PHONY: clean_win32
clean_win32:
	del /F/Q *.o $(OBJS) $(TARGET)
