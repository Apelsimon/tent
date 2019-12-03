cmake_minimum_required (VERSION 3.10)

project(tent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(libtorrent)
add_subdirectory(test)

set(SRC
    src/main.cpp
    src/test.cpp
)

# dependencies 
find_library( LIBTORRENT NAMES libtorrent
    PATHS
        ${PROJECT_BINARY_DIR}/libtorrent/    
    REQUIRED
)

set(DEPENDENCIES
    ${LIBTORRENT}
)

# tent exe
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libtorrent/include
)
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})


# tent lib
add_library(${PROJECT_NAME}_ STATIC ${SRC})
target_include_directories(${PROJECT_NAME}_ PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libtorrent/include
)
target_link_libraries(${PROJECT_NAME}_ ${DEPENDENCIES})

# boost dependencies 
find_package(Boost COMPONENTS system REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME}_ ${Boost_LIBRARIES})
endif()