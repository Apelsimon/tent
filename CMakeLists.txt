cmake_minimum_required (VERSION 3.10)

project(tent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(curlpp)
add_subdirectory(libtorrent)
add_subdirectory(test)

set(SRC
    src/byte_buffer.cpp
    src/endpoint.cpp
    src/file_handler.cpp
    src/file_reader.cpp
    src/http_client.cpp
    src/http_messages.cpp
    src/main.cpp
    src/msg_factory.cpp
    src/net_reactor.cpp
    src/peer_info.cpp
    src/piece_handler.cpp
    src/session.cpp
    src/session_sm.cpp
    src/signal_handler.cpp
    src/tcp_socket.cpp
    src/torrent_agent.cpp
    src/tracker_client.cpp    
)

# dependencies 
find_library( LIBTORRENT NAMES libtorrent-rasterbar.so
    PATHS
        ${PROJECT_SOURCE_DIR}/libtorrent/build    
    REQUIRED 
)

find_package(Boost COMPONENTS system REQUIRED)

set(DEPENDENCIES
    ${LIBTORRENT}
    ${Boost_LIBRARIES}
    curlpp
)

set(INCLUDES
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libtorrent/include
    ${Boost_INCLUDE_DIRS}
)

# tent exe
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)

# tent lib
add_library(${PROJECT_NAME}_ STATIC ${SRC})
target_include_directories(${PROJECT_NAME}_ PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME}_ ${DEPENDENCIES})
target_compile_options(${PROJECT_NAME}_ PRIVATE -Wall -Wextra -pedantic -Werror)


