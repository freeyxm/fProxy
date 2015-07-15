LIB_DIR = ../libs
INCLUDES = -I"../"
#LIBS = -L./fprotocol -lfprotocol -L./fcore/ -lfcore -lpthreadGC2 -lwsock32 #-lws2_32
LIBS = -L$(LIB_DIR) -lfprotocol -lfcore -lpthread
MACROS = -D_DEBUG_ -D_LOG_MUTEX_ #-D__WIN32__
CXX = g++
CXXFLAGS = -g -O0 -Wall -fmessage-length=0 $(INCLUDES) $(MACROS)
LDFLAGS = $(LIBS) #-mwindows
OBJS = 

TARGET = fProxy.exe fClient.exe test.exe


all: $(TARGET) 

fProxy.exe: fProxy.o $(LIB_DIR)/libfcore.a $(LIB_DIR)/libfprotocol.a
	$(CXX) -o $@ fProxy.o $(LDFLAGS)

fClient.exe: fClient.o $(LIB_DIR)/libfcore.a $(LIB_DIR)/libfprotocol.a
	$(CXX) -o $@ fClient.o $(LDFLAGS)

test.exe: test.o $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)

.PHONY: clean_win32
clean_win32:
	del /F/Q *.o $(TARGET)
