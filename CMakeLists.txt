cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CPU_CORE cortex-m4)
set(CMAKE_SYSTEM_PROCESSOR ${CPU_CORE})



set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_ONJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(TEST C CXX ASM)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
if(${BUILD_TYPE_UPPER} STREQUAL "RELEASE")
    add_compile_options(-Os)
else()
    add_compile_options(-Og -g)
endif()

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Include/
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Include/
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/
    ${CMAKE_CURRENT_LIST_DIR}/bsp/components/
    ${CMAKE_CURRENT_LIST_DIR}/app/source/
    ${CMAKE_CURRENT_LIST_DIR}/Driver/inc/
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/
)

add_definitions(-DDEBUG -DUSE_DDL_DRIVER -DHC32F460)

file(GLOB SOURCE_FILE
    ${CMAKE_CURRENT_LIST_DIR}/app/source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/*.*
    ${CMAKE_CURRENT_LIST_DIR}/Driver/src/*.*
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/*.*
)



set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map)

SET(STARTUP_FILE ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/GCC/startup_hc32f460.S)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/GCC/linker/HC32F460xE.ld)


add_link_options(
        -Wl,-Map=${MAP_FILE}
        -mcpu=${CPU_CORE} -mthumb
        -Wl,--print-memory-usage
        -Wl,--gc-sections
        -T ${LINKER_SCRIPT}
        -specs=nosys.specs
    )

add_compile_options(
        -mcpu=${CPU_CORE}
        -mthumb
        -mthumb-interwork
        -ffunction-sections
        -fdata-sections -fno-common -fmessage-length=0
    )

add_executable(${PROJECT_NAME}.elf ${SOURCE_FILE} ${STARTUP_FILE})

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
# )

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
COMMAND ${CMAKE_OBJCOPY} -O binary 
"${EXECUTABLE_OUTPUT_PATH}/TEST.elf" 
"${EXECUTABLE_OUTPUT_PATH}/TEST.bin"
COMMENT "Generating BIN file"
)

