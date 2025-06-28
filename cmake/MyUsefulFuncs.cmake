macro(my_add_target name type)
    # 用法: my_add_target(pybmain EXECUTABLE)
    file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp src/*.h)

    if("${type}" MATCHES "EXECUTABLE")
        add_executable(${name} ${srcs})
    else()
        add_library(${name} ${type} ${srcs})
    endif()

    target_include_directories(${name} PUBLIC include)
endmacro()

function(add_executable_with_pch target_name)
    set(options) # 开关选项
    set(oneValueArgs PCH) # 单值选项
    set(multiValueArgs SRCS) # 多值选项

    # 解析输入参数。解析结果都会放到 PARSED_<...> 里
    cmake_parse_arguments(
        PARSED # 前缀：所有结果变量都以 PARSED_ 开头
        "${options}" # 开关选项列表
        "${oneValueArgs}" # 单值选项列表
        "${multiValueArgs}" # 多值选项列表
        ${ARGN} # 函数收到的所有参数
    )
    add_executable(${target_name} ${PARSED_SRCS})
    target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/inc)

    if(PARSED_PCH)
        target_precompile_headers(${target_name} PRIVATE ${PARSED_PCH})
    else()
        target_precompile_headers(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/inc/pch.h)
    endif()

    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
    )
endfunction()

set(SOME_USEFUL_GLOBAL_VAR ON)
set(ANOTHER_USEFUL_GLOBAL_VAR OFF)
