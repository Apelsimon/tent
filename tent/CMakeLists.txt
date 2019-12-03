cmake_minimum_required (VERSION 3.10)

project(tent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(SRC
    src/main.cpp
    src/test.cpp
)

set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_ STATIC ${SRC})
target_include_directories(${PROJECT_NAME}_ PUBLIC ${PROJECT_SOURCE_DIR}/include)
