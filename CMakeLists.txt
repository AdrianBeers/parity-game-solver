cmake_minimum_required(VERSION 3.24)
project(parity_game_solver)

set(CMAKE_CXX_STANDARD 14)

add_executable(parity_game_solver main.cpp Parser.h Parser.cpp ParityGame.cpp ParityGame.h)
