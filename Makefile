MAINSOURCE := Test/testThread.cc Test/testLatch.cc


SOURCE := $(wildcard *.cc Base/*.cc Test/*.cc)

#反过滤，过滤不是main函数的文件
override SOURCE := $(filter-out $(MAINSOURCE), $(SOURCE))	

#patsubst: 进行模式的替换
OBJS   := $(patsubst %.cc, %.o, $(SOURCE))		#将.cc -> .o


TARGET  := MyMuduo 
CC 	:= g++
LIBS 	:= -lpthread
INCLUDE := -I /root/
CFLAGS  := -std=c++11 -g -Wall -O3 -D_PTHREADS -I /root		#下面就是c\cpp编译汇编的过程
CXXFLAGS := $(CFLAGS)


#Test target 
TARGET1 := ThreadTest 
TARGET2 := LatchTest

.PHONY  : objs clean all tests 

objs    : $(OBJS)



$(TARGET1) : $(OBJS) Test/testThread.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)


$(TARGET2) : $(OBJS) Test/testLatch.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)






