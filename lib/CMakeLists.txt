include_directories(../include)
add_library(opcodeCC SHARED OpCodeCounter.cpp)
add_library(DeadCodeElimination SHARED DeadCodeElimination.cpp)
#add_library(vanilla SHARED vanilla.cpp)