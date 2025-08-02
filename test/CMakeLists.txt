cmake_minimum_required(VERSION 3.10)
project(ini_handler_tests LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 定义测试目标和源文件
set(TEST_TARGET ini_test)
set(TEST_SOURCES ini_test.cc)

# 头文件目录（指向项目根目录的include文件夹）
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# 添加测试可执行文件
add_executable(${TEST_TARGET} ${TEST_SOURCES})

# 设置可执行文件输出目录为test目录
set_target_properties(${TEST_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接头文件目录
target_include_directories(${TEST_TARGET}
    PRIVATE
    ${include_DIR}
)
