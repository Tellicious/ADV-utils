# Define tests list
set(TESTS_LIST
    basicMath
    button
    event
    hashFunctions
    IIRFilters
    list
    LKHashTable
    LPHashTable
    matrix
    movingAvg
    numMethods
    PID
    quaternion
    queue
    timer
)

# Fetch CMocka library
include(FetchContent)
SET(CMAKE_TLS_VERIFY FALSE)

set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)

FetchContent_Declare(
  cmocka
  GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
  GIT_TAG        master
  GIT_SHALLOW    1
)

FetchContent_MakeAvailable(cmocka)

# Create helper file to simplify CMocka includes
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/../inc/cmocka_includes.h
"#ifndef __CMOCKA_INCLUDES_H__
#define __CMOCKA_INCLUDES_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <cmocka.h>

#define ADVUTILS_ASSERT(x) ADVUtils_testAssert((int)(x), #x, __FILE__, __LINE__);

void* ADVUtils_testCalloc(const size_t number_of_elements, const size_t size);
void* ADVUtils_testMalloc(const size_t size);
void ADVUtils_testAssert(const int result, const char* const expression, const char* const file, const int line);
uint32_t ADVUtils_testHash(char* key);

#endif /* __CMOCKA_INCLUDES_H__ */
")

# Find math library (needed for compilation under Ubuntu)
if (NOT WIN32)
    find_library(MATH_LIBRARY m)
else()
    set(MATH_LIBRARY "")
endif()

foreach(FILE ${TESTS_LIST})
    # Set sources
    set(SOURCES_${FILE}
        test_${FILE}.c
    )
    # Create an executable for testing
    add_executable(run_${FILE}Tests ${SOURCES_${FILE}})
    # Add the include directories
    target_include_directories(run_${FILE}Tests PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/../inc)
    # Link CMocka and the math library
    target_link_libraries(run_${FILE}Tests PRIVATE cmocka ADVUtils ${MATH_LIBRARY})
    target_compile_options(run_${FILE}Tests PRIVATE ${compiler_OPTS})
    target_compile_definitions(run_${FILE}Tests PRIVATE ${user_DEFS})
    target_link_options(run_${FILE}Tests PRIVATE --coverage)
    if((${FILE} STREQUAL "LKHashTable") OR (${FILE} STREQUAL "LPHashTable"))
    target_compile_definitions(run_${FILE}Tests PRIVATE ${ADVUtils_COMPILE_DEFS})
    endif()
endforeach()

# Add tests
foreach(FILE ${TESTS_LIST})
    add_test(NAME ${FILE}Test COMMAND run_${FILE}Tests)
endforeach()