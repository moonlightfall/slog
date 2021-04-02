#cmake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

#项目名称
project(test_press_slog)

#操作系统类型
set(CMAKE_SYSTEM_NAME Linux)

#编译器类型
set(CMAKE_C_COMPILER "gcc")

#设置库文件根路径
# set(LIBDIR_BASE ${PROJECT_SOURCE_DIR}/../lib/x86)

aux_source_directory(${PROJECT_SOURCE_DIR}/../src SRC_DIR)
set(SRC_FILES ${SRC_DIR})

#添加头文件
include_directories(${PROJECT_SOURCE_DIR}/../inc
                   )

#添加库文件搜索路径
# link_directories(${LIBDIR_BASE})

#设置编译器选项
set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -std=gnu99")

#生成可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES} test_press_slog.c)

#链接库文件
# target_link_libraries(${PROJECT_NAME} slog)
target_link_libraries(${PROJECT_NAME} pthread)