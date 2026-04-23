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
include_directories(C:/Users/hans/Desktop/main_hw1/main_cpp_hw1/include)

# 6. 生成可执行程序
add_executable(${PROJECT_NAME} ${SOURCES})

# 使用 CMAKE_CURRENT_SOURCE_DIR 确保路径从当前 CMakeLists 所在位置开始找
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(OpenCV_DIR "C:/Program Files/opencv/build/x64/vc16/lib")
# 指定 Qt 安装路径
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_package(OpenCV REQUIRED)

# 7. 链接库 
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)