# 1. 指定最低版本要求 (Qt6 建议 3.16 以上)
cmake_minimum_required(VERSION 3.16)

# 2. 项目名称
project(MainHomework VERSION 1.0 LANGUAGES CXX)

# 3. 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 关键修复：解决 MSVC 宏报告问题 ---
if(MSVC)
    add_compile_options(/Zc:__cplusplus /utf-8)
endif()

# 4. 指定路径 (注意：这里必须匹配你的编译器)
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/msvc2022_64") 
set(OpenCV_DIR "C:/Program Files/opencv/build/x64/vc16/lib")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 5. 查找库
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)


# 6. 搜集源文件
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
file(GLOB UI_FILES "src/*.ui")

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src")


# 7. 生成并链接
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS} 
    ${UI_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${OpenCV_LIBS} 
    Qt6::Widgets
)