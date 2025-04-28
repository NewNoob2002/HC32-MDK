cmake_minimum_required(VERSION 3.20)

# 启用compile_commands.json导出，支持clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSTEM_NAME Generic)
set(CPU_CORE cortex-m4)
set(CMAKE_SYSTEM_PROCESSOR ${CPU_CORE})

# 设置编译器工具链
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(X1-Pannel C CXX ASM)

# 设置构建类型相关选项
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Os)
else()
    add_compile_options(-Og -g)
endif()

# 添加Include目录
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

# 收集源文件
file(GLOB_RECURSE SOURCE_FILE
    ${CMAKE_CURRENT_LIST_DIR}/app/source/*.c
    ${CMAKE_CURRENT_LIST_DIR}/app/source/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/app/source/*.s
    ${CMAKE_CURRENT_LIST_DIR}/app/source/*.S
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/*.c
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/*.s
    ${CMAKE_CURRENT_LIST_DIR}/bsp/ev_hc32f460_lqfp100_v2/*.S
    ${CMAKE_CURRENT_LIST_DIR}/Driver/src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/Driver/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Driver/src/*.s
    ${CMAKE_CURRENT_LIST_DIR}/Driver/src/*.S
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/*.c
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/*.s
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/*.S
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/*.c
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/*.s
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/MillisTaskManager/*.S
)

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map)

# 启动文件和链接脚本
SET(STARTUP_FILE ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/GCC/startup_hc32f460.S)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/HDSC/hc32f4xx/Source/GCC/linker/HC32F460xE.ld)

# 链接选项
add_link_options(
    -Wl,-Map=${MAP_FILE}
    -mcpu=${CPU_CORE} -mthumb
    -Wl,--print-memory-usage
    -Wl,--gc-sections
    -T ${LINKER_SCRIPT}
    -specs=nosys.specs
)

# 编译选项
add_compile_options(
    -mcpu=${CPU_CORE}
    -mthumb
    -mthumb-interwork
    -ffunction-sections
    -fdata-sections 
    -fno-common 
    -fmessage-length=0
    -Wall
)

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILE} ${STARTUP_FILE})

# 生成HEX和BIN文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf" "${HEX_FILE}"
    COMMAND ${CMAKE_OBJCOPY} -O binary "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf" "${BIN_FILE}"
    COMMAND ${SIZE} --format=berkeley "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf"
    COMMENT "Generating HEX and BIN files"
)

# 为clangd创建compile_commands.json的链接
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E create_symlink 
#             ${CMAKE_BINARY_DIR}/compile_commands.json
#             ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
#     COMMENT "Creating symlink to compile_commands.json for clangd"
# )

