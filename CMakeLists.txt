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
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    cmake_minimum_required(VERSION 3.22)

    project(ADVUtilsTests C)

    # Setup compiler settings
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS ON)
    option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

    # Set user definitions
    set(user_DEFS
        ADVUTILS_USE_DYNAMIC_ALLOCATION
        ADVUTILS_USE_STATIC_ALLOCATION
        #USE_FAST_MATH
        $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:DEBUG>
    )

    # Compiler options
    set(compiler_OPTS
        -fdiagnostics-color=always
        -fstack-protector-all
        -fno-builtin
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wno-gnu-statement-expression
        --coverage
        -O0
        -g3
    )

    # Add ADVutils library
    set (ADVUtils_COMPILE_OPTS
        ${compiler_OPTS}
    )
    set (ADVUtils_COMPILE_DEFS
        ${user_DEFS}
        ADVUTILS_MEMORY_MGMT_HEADER="cmocka_includes.h"
        ADVUTILS_MALLOC=ADVUtils_testMalloc
        ADVUTILS_CALLOC=ADVUtils_testCalloc
        ADVUTILS_FREE=test_free
        ADVUTILS_ASSERT_HEADER="cmocka_includes.h"
        ADVUTILS_UNIT_TESTS
        LKHT_LIST_SIZE=2 
        LKHT_HASHFUN=ADVUtils_testHash
        LPHT_MIN_SIZE=2
        LPHT_MAX_SIZE=20
        LPHT_MIN_SATURATION=0.2
        LPHT_MAX_SATURATION=0.7
        LPHT_HASHFUN=ADVUtils_testHash
    )
endif()

add_subdirectory(src)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(ADVUtils cmocka)
    set(CMAKE_CTEST_ARGUMENTS "-j 8" "--rerun-failed" "--output-on-failure" "--verbose")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()