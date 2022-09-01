# cmake_minimum_required (VERSION 2.6)

# project(WebServer)

# include_directories(
#     Base
#     Http
#     Log
#     Net
#     Timer
# )




# set(CMAKE_CXX_COMPILER "g++")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# find_package(Threads REQUIRED)

# aux_source_directory(SRC_LIST1 Base)                       # the src file under dir:base
# aux_source_directory(SRC_LIST2 Net)                         
# aux_source_directory(SRC_LIST3 Http)
# aux_source_directory(SRC_LIST4 Timer)
# aux_source_directory(SRC_LIST5 Log)

# set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})          # 可执行代码生成在项目的根目录下
# add_executable(WebServer Main.cc ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})

# target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})





cmake_minimum_required(VERSION 2.6)

project(WebServer C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

include_directories(
  Base 
  Http 
  Log
  Timer 
  Net
)

SET(CXX_FLAGS
  -g
  -Wall
  -std=c++11
)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Threads REQUIRED)


aux_source_directory(Base SRC_LIST1)
aux_source_directory(Http SRC_LIST2)
aux_source_directory(Log SRC_LIST3)
aux_source_directory(Timer SRC_LIST4)
aux_source_directory(Net SRC_LIST5)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(WebServer Main.cc ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})

target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
