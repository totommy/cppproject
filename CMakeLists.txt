cmake_minimum_required(VERSION 4.1.1)
project(DockerTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include 檔案目錄
include_directories(include)

# 編譯 server 可執行檔
add_executable(server src/test.cpp)

# 跨平台線程設定
find_package(Threads REQUIRED)
if(Threads_FOUND)
    target_link_libraries(server PRIVATE Threads::Threads)
endif()

# Windows 要加 ws2_32
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()