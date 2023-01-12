cmake_minimum_required(VERSION 3.24)
project(parity_game_solver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s -O3")
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(parity_game_solver main.cpp Parser.h Parser.cpp ParityGame.cpp ParityGame.h Solver.cpp Solver.h)
