set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(MCPU_FLAGS "-mthumb -mcpu=cortex-m33 -mthumb-interwork")
set(VFP_FLAGS "-mfloat-abi=hard -mfpu=fpv5-sp-d16")
set(SPEC_FLAGS "--specs=nano.specs")
set(LD_FLAGS "--specs=nosys.specs")
# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(BUILD_SHARED_LIBS FALSE CACHE STRING "disable build shared libs")
set(CMAKE_C_FLAGS   "${MCPU_FLAGS} ${VFP_FLAGS} ${SPEC_FLAGS} -fdata-sections -ffunction-sections -mlong-calls" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${SPEC_FLAGS} -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions -mlong-calls" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${MCPU_FLAGS} ${VFP_FLAGS} ${SPEC_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${MCPU_FLAGS} ${LD_FLAGS} -Wl,--gc-sections,--print-memory-usage" CACHE INTERNAL "exe link flags")

set(PROJECT_NAME ${PROJECT_NAME}-satellit-a10)
set(VER_MAJ 0)
set(VER_MIN 1)
set(VER_DEV 2)
set(VNAR "ВНАР-X")

# redefine linker-script
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/STM32U575ZITXQ_FLASH.ld)
set(MCU ${CMAKE_CURRENT_LIST_DIR}/cube)
include(cube)

add_definitions(-DSTM32U585xx)
add_definitions(-DVNAR="${VNAR}" -DVER_STR="${VER_MAJ}.${VER_MIN}.${VER_DEV}"
  -DVER_MAJ=${VER_MAJ} -DVER_MIN=${VER_MIN} -DVER_DEV=${VER_DEV})
add_link_options(-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
set(SOURCES ${cube_SOURCES} ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/button.c
  ${CMAKE_CURRENT_LIST_DIR}/src/system.c
  ${CMAKE_CURRENT_LIST_DIR}/src/diskio.c)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf ${LIBS} ${MODULES})

set(ARTIFACT_NAME ${PROJECT_NAME}-${VER_MAJ}.${VER_MIN}.${VER_DEV})
set(HEX_FILE ${PROJECT_BINARY_DIR}/${ARTIFACT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
  COMMENT "Building ${HEX_FILE}")
