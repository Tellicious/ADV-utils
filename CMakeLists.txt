# 
# This file provides set of variables for end user
# and also generates one (or more) libraries, that can be added to the project using target_link_libraries(...)
#
# Before this file is included to the root CMakeLists file (using add_subdirectory() function), user can set some variables:
#
# libName_COMPILE_OPTS: If defined, it provide compiler options for generated library.
# libName_COMPILE_DEFS: If defined, it provides "-D" definitions to the library build
# add set(${libName}_DYN_MEM_MGMT "USE_FREERTOS" CACHE STRING "" FORCE) before add_subdirectory() to automatically configure the library to use FreeRTOS memory management functions (malloc, calloc, free)
#
cmake_minimum_required(VERSION 3.22)

set (libName ADVUtils)

set(${libName}_src
    ${CMAKE_CURRENT_LIST_DIR}/src/basicMath.c
    ${CMAKE_CURRENT_LIST_DIR}/src/button.c
    ${CMAKE_CURRENT_LIST_DIR}/src/event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/hashFunctions.c
    ${CMAKE_CURRENT_LIST_DIR}/src/IIRFilters.c
    ${CMAKE_CURRENT_LIST_DIR}/src/list.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LKHashTable.c
    ${CMAKE_CURRENT_LIST_DIR}/src/LPHashTable.c
    ${CMAKE_CURRENT_LIST_DIR}/src/matrix.c
    ${CMAKE_CURRENT_LIST_DIR}/src/movingAvg.c
    ${CMAKE_CURRENT_LIST_DIR}/src/numMethods.c
    ${CMAKE_CURRENT_LIST_DIR}/src/PID.c
    ${CMAKE_CURRENT_LIST_DIR}/src/quaternion.c
    ${CMAKE_CURRENT_LIST_DIR}/src/queue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.c
)

# Setup include directories
set(${libName}_inc
    ${CMAKE_CURRENT_LIST_DIR}/inc/
)

# Append definitions needed to automatically configure build with FreeRTOS
if ("${${libName}_DYN_MEM_MGMT}" STREQUAL "USE_FREERTOS")
    list(APPEND ${libName}_COMPILE_DEFS
        ADVUTILS_MEMORY_MGMT_HEADER="FreeRTOS.h"
        ADVUTILS_MALLOC=pvPortMalloc
        ADVUTILS_CALLOC=pvPortCalloc
        ADVUTILS_FREE=vPortFree
    )
endif()

# Register library to the system
add_library(${libName} STATIC)
target_sources(${libName} PRIVATE ${${libName}_src})
target_include_directories(${libName} PUBLIC ${${libName}_inc})
target_compile_options(${libName} PRIVATE ${${libName}_COMPILE_OPTS})
target_compile_definitions(${libName} PRIVATE ${${libName}_COMPILE_DEFS})
if ("${${libName}_DYN_MEM_MGMT}" STREQUAL "USE_FREERTOS")
    target_link_libraries(${libName} freertos_kernel)
endif()