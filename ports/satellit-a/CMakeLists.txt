include(cortex_m4)

set(PROJECT_NAME ${PROJECT_NAME}-satellit-a)
set(VER_MAJ 0)
set(VER_MIN 0)
set(VER_DEV 1)
set(VNAR "ВНАР-X")

# redefine linker-script
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407IGTx_FLASH.ld)

set(MCU ${CMAKE_CURRENT_SOURCE_DIR}/cube)
include(cube)

add_definitions(-DSTM32F407xx)
add_definitions(-DVNAR="${VNAR}" -DVER_STR="${VER_MAJ}.${VER_MIN}.${VER_DEV}"
  -DVER_MAJ=${VER_MAJ} -DVER_MIN=${VER_MIN} -DVER_DEV=${VER_DEV})
add_link_options(-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/button.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/port.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/system.c)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf ${LIBS} ${MODULES})

set(ARTIFACT_NAME ${PROJECT_NAME}-${VER_MAJ}.${VER_MIN}.${VER_DEV})
set(HEX_FILE ${PROJECT_BINARY_DIR}/${ARTIFACT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
  COMMENT "Building ${HEX_FILE}")
