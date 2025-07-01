cmake_minimum_required(VERSION 3.18)

if(WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install CACHE PATH "installation path" FORCE)

project(Concurrent LANGUAGES CXX)

include(MyUsefulFuncs)

add_subdirectory(use_thread)
add_subdirectory(shared_memory)
add_subdirectory(syn_asyn_operation)

# 只在 MSVC（Visual Studio）下添加 PowerShell 脚本目标
if(MSVC)
    find_program(POWERSHELL_EXE

        # NAMES pwsh.exe powershell.exe
        NAMES powershell.exe
        HINTS
        "$ENV{SystemRoot}/System32/WindowsPowerShell/v1.0"
        ENV PATH
    )

    if(NOT POWERSHELL_EXE)
        message(FATAL_ERROR "找不到 PowerShell，请确认已安装")
    endif()

    message(STATUS "PowerShell 可执行文件: ${POWERSHELL_EXE}")

    add_custom_target(generate_compile_commands
        COMMENT "自动生成 compile_commands.json"
        COMMAND ${CMAKE_COMMAND} -E echo "▶ 调用 PowerShell 脚本…"
        COMMAND ${POWERSHELL_EXE}
        -NoProfile
        -ExecutionPolicy Bypass
        -File "${CMAKE_SOURCE_DIR}/generate_msvc_compile.ps1"
        -Config $<CONFIG>
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()