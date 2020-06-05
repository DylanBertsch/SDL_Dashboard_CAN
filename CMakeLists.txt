cmake_minimum_required(VERSION 3.15)
project(SDL_Dashboard)

set(CMAKE_CXX_STANDARD 14)

add_executable(SDL_Dashboard main.cpp gui_library.cpp gui_library.h dataCommunication_mega2560.h)

target_link_libraries(SDL_Dashboard SDL2)
target_link_libraries(SDL_Dashboard SDL2_ttf)