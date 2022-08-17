MAINSOURCE := Test/testThread.cc Test/testLatch.cc \
Test/testEventLoop1.cc Test/testReactor.cc \
 Test/testRuninloop.cc Test/testAccept.cc Test/testServer.cc Test/testSimAll.cc


SOURCE := $(wildcard *.cc Base/*.cc Test/*.cc Net/*.cc)

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
TARGET1 := ThreadTest 			#线程类的测试
TARGET2 := LatchTest			#latch同步类的测试
TARGET3 := EventLoopTest1		#eventloop的测试， version1
TARGET4 := ReactorTest			#Reactor的测试， version1
TARGET5 := RunInLoopTest		#进行Runinloop的测试，eventloopthread
TARGET6 := AcceptTest			#进行accpet的简单测试
TARGET7 := ServerTest			#进行Server的简单的测试
TARGET8 := SimAllTest			#进行单线程的所有功能的测试


.PHONY  : objs clean all tests 

objs    : $(OBJS)
clean   : 
			find ./Base/ -name "*.o" | xargs rm -f
			find ./Net/ -name "*.o" | xargs rm -f
			find ./Test/ -name "*.o" | xargs rm -f	



$(TARGET1) : $(OBJS) Test/testThread.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)


$(TARGET2) : $(OBJS) Test/testLatch.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)


$(TARGET3) : $(OBJS) Test/testEventLoop1.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)

$(TARGET4) : $(OBJS) Test/testReactor.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)

$(TARGET5) : $(OBJS) Test/testRuninloop.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)

$(TARGET6) : $(OBJS) Test/testAccept.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)

$(TARGET7) : $(OBJS) Test/testServer.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)

$(TARGET8) : $(OBJS) Test/testSimAll.o
	$(CC) $(CXXFLAGS) -o   $@ $^  $(LDFLAGS) $(LIBS)


