cmake_minimum_required(VERSION 3.1)
project(wsautofill)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Xlnt REQUIRED)

include_directories(
	include
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
					  xlnt
					  yaml-cpp
)