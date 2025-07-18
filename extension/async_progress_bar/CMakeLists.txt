project(async_progress_bar)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT MSVC)
    find_program(CCACHE_PROGRAM ccache)

    if(CCACHE_PROGRAM)
        message(STATUS "Found CCache: ${CCACHE_PROGRAM}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
    endif()
endif()

set(QT_PATH "D:/soft/Qt/Qt5.14.2/5.14.2/mingw73_64" CACHE PATH "QT_PATH")
list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(Qt${QT_VERSION_MAJOR}_FOUND)
    message(STATUS "Qt information:")
    message(STATUS "    QT_PATH: " ${QT_PATH})
    message(STATUS "    Qt${QT_VERSION_MAJOR}::Widgets found at " ${Qt${QT_VERSION_MAJOR}_DIR} "(Qt5/6_DIR)")
    message(STATUS "    CMAKE_PREFIX_PATH:" ${CMAKE_PREFIX_PATH})
endif()

# 查找源文件
file(GLOB UI_FILES ${PROJECT_SOURCE_DIR}/ui/*.ui)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADER_FILES ${PROJECT_SOURCE_DIR}/inc/*.h)
file(GLOB RESOURCE_FILES ${PROJECT_SOURCE_DIR}/qrc/*.qrc)

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "ui")
list(APPEND CMAKE_AUTORCC_SEARCH_PATHS "qrc")

add_executable(${PROJECT_NAME} ${UI_FILES} ${SRC_FILES} ${HEADER_FILES} ${RESOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/inc)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)