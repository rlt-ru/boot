cmake_minimum_required(VERSION 3.16)

# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(boot C CXX ASM)

include_directories(src)
include_directories(src/fatfs)
set(SOURCES ${SOURCES} ${CMAKE_SOURCE_DIR}/src/main.c
  ${CMAKE_SOURCE_DIR}/src/app.c
  ${CMAKE_SOURCE_DIR}/src/fatfs/ff.c
  ${CMAKE_SOURCE_DIR}/src/fatfs/ffsystem.c
  ${CMAKE_SOURCE_DIR}/src/fatfs/ffunicode.c
  ${CMAKE_SOURCE_DIR}/src/crc_posix.c
)

add_subdirectory(${CMAKE_SOURCE_DIR}/ports/satellit-a)
add_subdirectory(${CMAKE_SOURCE_DIR}/ports/satellit-a10)
