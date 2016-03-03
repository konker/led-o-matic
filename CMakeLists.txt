cmake_minimum_required(VERSION 2.8)
project("LED-O-Matic LED Matrix Driver Daemon")
set(CMAKE_BUILD_TYPE Debug)

option(KLM_DRIVER "Which LED panel specific driver to use" OFF)
option(LEDOMATIC_NO_FORK "Led-o-maticd will not fork and run as a deamon" OFF)

if(NOT KLM_DRIVER)
    message(FATAL_ERROR "No KLM_DRIVER specified. Aborting.")
else()
    message("Using KLM_DRIVER: ${KLM_DRIVER}")
endif()

include_directories(include)
include_directories(../inih/include)
include_directories(../konker_led_matrix/include)
include_directories(../hexfont/include)
include_directories(../tinyutf8/include)

link_directories(${CMAKE_SOURCE_DIR}/../inih/build)
link_directories(${CMAKE_SOURCE_DIR}/../konker_led_matrix/build)
link_directories(${CMAKE_SOURCE_DIR}/../hexfont/build)
link_directories(${CMAKE_SOURCE_DIR}/../tinyutf8/build)

file(GLOB SOURCES "src/*.c")

if(LEDOMATIC_NO_FORK)
    add_definitions(-DLEDOMATIC_NO_FORK)
endif()

add_executable(led-o-maticd ${SOURCES})
target_link_libraries(led-o-maticd klm ${KLM_DRIVER} hexfont tinyutf8 inih pthread)

