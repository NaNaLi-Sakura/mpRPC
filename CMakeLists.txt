cmake_minimum_required(VERSION 3.20)

project(mprpc)

# 生成debug版本，可以进行gdb调试
# set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 设置项目可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# 设置项目库文件的输出路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# 设置项目编译头文件的搜索路径 -I
include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/example)

# 设置项目库文件的搜索路径 -L
link_directories(${CMAKE_SOURCE_DIR}/lib)

# ./src包含了mprpc框架所有的相关代码
add_subdirectory(./src)
# ./example包含了mprpc框架的使用示例代码
add_subdirectory(./example)