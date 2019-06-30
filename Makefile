CC=gcc
CXX=g++
PROG = ytsm
CXXFLAGS := -std=c++1y -g -Wall
LDFLAGS = -L/opt/local/lib -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LIBS= -lboost_system -lboost_filesystem -lncurses -pthread -lboost_thread-mt

SRCS=  src/Channel.cpp src/Video.cpp src/subParser.cpp src/ui.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all: ytsm

ytsm: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ytsm $(OBJS) $(LIBS) $(LDFLAGS)

ui.o: ui.cpp

Channel.o: Channel.cpp Channel.h

Video.o: Video.cpp Video.h

subParser.o: subParser.cpp subParser.h

clean:
	    rm -f $(binaries) *.o src/*.o
