project(syn_asyn_operation)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS inc/*.h)

find_package(OpenMP REQUIRED)

message(STATUS ${CMAKE_SOURCE_DIR})
message(STATUS ${PROJECT_SOURCE_DIR})

foreach(src_file ${SRCS})
    # 获取文件名（不带扩展名）
    get_filename_component(file_name ${src_file} NAME_WE)
    add_executable_with_pch(${PROJECT_NAME}_${file_name} SRCS ${src_file})
endforeach()

# target_link_libraries(${PROJECT_NAME}_demo_v10 PRIVATE OpenMP::OpenMP_CXX)