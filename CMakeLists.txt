# 1. 指定最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. 项目名称及版本
project(MainHomework VERSION 1.0 LANGUAGES CXX)

# 3. 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 搜集源文件
file(GLOB SOURCES "src/*.cpp")

# 5. 指定头文件包含路径 (相当于 -I)
include_directories(include)

# 6. 生成可执行程序
add_executable(${PROJECT_NAME} ${SOURCES})

# 7. 链接库 (如果有需要)
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)