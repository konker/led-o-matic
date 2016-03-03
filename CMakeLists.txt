cmake_minimum_required(VERSION 2.8)
project("LED-O-Matic LED Matrix Driver Daemon")
set(CMAKE_BUILD_TYPE Debug)

set(DEFAULT_HEXFONT_DIR "${CMAKE_SOURCE_DIR}/../hexfont")
set(DEFAULT_TINYUTF8_DIR "${CMAKE_SOURCE_DIR}/../tinyutf8")
set(DEFAULT_KLM_DIR "${CMAKE_SOURCE_DIR}/../konker_led_matrix")
set(DEFAULT_INIH_DIR "${CMAKE_SOURCE_DIR}/../inih")

option(LEDOMATIC_NO_FORK "Led-o-maticd will not fork and run as a deamon" OFF)
option(KLM_DRIVER "Which LED panel specific driver to use" OFF)

option(HEXFONT_DIR "Location of the hexfont library" OFF)
option(TINYUTF8_DIR "Location of the tinyutf8 library" OFF)
option(KLM_DIR "Location of the konker_led_matrix library" OFF)
option(INIH_DIR "Location of the inih library" OFF)

if(NOT KLM_DRIVER)
    message(FATAL_ERROR "No KLM_DRIVER specified. Aborting.")
else()
    message("Using KLM_DRIVER: ${KLM_DRIVER}")
endif()

if(NOT HEXFONT_DIR)
    message(WARNING "No HEXFONT_DIR specified. Using default: ${DEFAULT_HEXFONT_DIR}")
    set(HEXFONT_DIR ${DEFAULT_HEXFONT_DIR})
endif()

if(NOT TINYUTF8_DIR)
    message(WARNING "No TINYUTF8_DIR specified. Using default: ${DEFAULT_TINYUTF8_DIR}")
    set(TINYUTF8_DIR ${DEFAULT_TINYUTF8_DIR})
endif()

if(NOT KLM_DIR)
    message(WARNING "No KLM_DIR specified. Using default: ${DEFAULT_KLM_DIR}")
    set(KLM_DIR ${DEFAULT_KLM_DIR})
endif()

if(NOT INIH_DIR)
    message(WARNING "No INIH_DIR specified. Using default: ${DEFAULT_INIH_DIR}")
    set(INIH_DIR ${DEFAULT_INIH_DIR})
endif()

include_directories(include)
include_directories(${HEXFONT_DIR}/include)
include_directories(${TINYUTF8_DIR}/include)
include_directories(${KLM_DIR}/include)
include_directories(${INIH_DIR}/include)

link_directories(${HEXFONT_DIR}/build)
link_directories(${TINYUTF8_DIR}/build)
link_directories(${KLM_DIR}/build)
link_directories(${INIH_DIR}/build)

file(GLOB SOURCES "src/*.c")

if(LEDOMATIC_NO_FORK)
    add_definitions(-DLEDOMATIC_NO_FORK)
endif()

add_executable(led-o-maticd ${SOURCES})
target_link_libraries(led-o-maticd klm ${KLM_DRIVER} hexfont tinyutf8 inih pthread)

