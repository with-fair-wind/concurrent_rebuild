project(use_thread)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS inc/*.h)

message(STATUS ${CMAKE_SOURCE_DIR})
message(STATUS ${PROJECT_SOURCE_DIR})

add_executable_with_pch(${PROJECT_NAME}_demo_v1 SRCS src/demo_v1.cpp)
add_executable_with_pch(${PROJECT_NAME}_demo_v2 SRCS src/demo_v2.cpp)

# add_executable()(${PROJECT_NAME} SHARED ${SRCS})
# target_precompile_headers(${PROJECT_NAME} PUBLIC inc/stdafx.h)
# target_include_directories(${PROJECT_NAME} PRIVATE inc)