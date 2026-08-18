<p align="center"> <img src="https://github.com/Tellicious/ADV-utils/assets/9076397/3ec512f1-2de6-4226-bc07-e4bfdd686a28" width=50% height=50%> </p>

# Collection of useful C utilities (MISRA-compliant)

[![Unit tests](https://github.com/Tellicious/ADV-utils/actions/workflows/runTests.yml/badge.svg?event=push)](https://github.com/Tellicious/ADV-utils/actions/workflows/runTests.yml)
[![Codecov](https://codecov.io/gh/Tellicious/ADV-utils/graph/badge.svg?token=OJG3076HXJ)](https://codecov.io/gh/Tellicious/ADV-utils)

## Libraries included:
- ***commonTypes:*** collection of common type definitions
- ***basicMath:*** math constants, conversion factors, bitwise operators, `fastSqrt`, `fastInvSqrt`, `fastSin` and `fastCos`
- ***button:*** interrupt-based button object with debounce and multiple press types detection
- ***event:*** callback-based event manager with support for functions with type `void(*)(void)` and `void(*)(void* val)`
- ***fix16:*** Q16.16 fixed point scalar operations (add, subtract, multiply, divide, sqrt, abs, convert)
- ***IIRFilters:*** simple discrete-time IIR filters, with on-the-fly conversion continuous -> discrete of derivative, integrator, leaky integrator, 2nd order low-pass, high-pass, band-pass and band-stop filters
- ***list:*** linked list
- ***LKHashTable:*** dynamic linked hash-table object (based on _list_)
- ***LPHashTable:*** linear-probing hash-table object with auto-resize capability
- ***matrix:*** handling of matrix objects and matrix operations
- ***movingAvg:*** moving average object
- ***numMethods:*** common numerical methods to solve linear systems (LU, Cholesky, QR least-squares), perform Gauss-Newton sphere approximation and solve discrete-time algebraic Riccati equation. The float linear solvers return `UTILS_STATUS_ERROR` on a singular system or on non-finite (NaN/Inf) pivots or results instead of propagating them
- ***PID:*** PID controller with several anti-windup options (aero-specific, integral-clamping, back-calculation) and selectable derivative-on-error or derivative-on-measurement mode
- ***quaternion:*** quaternion operations, conversion to Euler angles and to active rotation matrix, and construction from axis-angle
- ***queue:*** queue structure
- ***timer:*** user-configurable timers
- ***vector3f:*** 3-element vector operations (add, subtract, scale, dot, norm, normalize, cross)

## Library configuration:
- User must define `ADVUTILS_USE_STATIC_ALLOCATION` and/or `ADVUTILS_USE_DYNAMIC_ALLOCATION` to select wheter to use static and/or dynamic memory management. `list`, `LPHashTable` and `LKHashTable` are available only with `ADVUTILS_USE_DYNAMIC_ALLOCATION`
- Functions that use static allocation are defined by `Static` suffix
- To automatically use thread-safe FreeRTOS-specific implementation of dynamic memory management functions (`malloc`, `calloc` and `free`) user can add `set(ADVUtils_DYN_MEM_MGMT "USE_FREERTOS" CACHE STRING "" FORCE)` before `add_subdirectory()` in project CMakeLists. This will automatically add the following defines to ADVUtils compilation:
    - `ADVUTILS_MEMORY_MGMT_HEADER="FreeRTOS.h"`
    - `ADVUTILS_MALLOC=pvPortMalloc`
    - `ADVUTILS_CALLOC=pvPortCalloc`
    - `ADVUTILS_FREE=vPortFree`
    - `target_link_libraries(ADVUtils freertos_kernel)`
- If `ADVUTILS_MEMORY_MGMT_HEADER` is not defined, the library will use `stdlib` version of `malloc`, `calloc` and `free`
- User can use another different implementation of `malloc`, `calloc` and `free` by defining the following four macros:
    - `ADVUTILS_MEMORY_MGMT_HEADER`
    - `ADVUTILS_MALLOC`
    - `ADVUTILS_CALLOC`
    - `ADVUTILS_FREE`
- User can select to use faster (and less precise) math functions from `basicMath` by adding `USE_FAST_MATH` to compile definitions
- Assertion model: when `DEBUG` is defined the library checks input parameters with `ADVUTILS_ASSERT`, whose built-in handler traps in an infinite loop; it is compiled out in release builds. To keep the checks active in release as well, define `ADVUTILS_USE_ASSERT_ALWAYS`. The handler is fully overridable, either by defining `ADVUTILS_ASSERT(x)` directly, or by defining `ADVUTILS_ASSERT_HEADER` to point at a header that declares your own assert. Two common override patterns:
    - Log and trap:
        ```c
        #define ADVUTILS_ASSERT(x)                                        \
            do {                                                          \
                if (!(x)) {                                               \
                    printf("assert failed: %s:%d\n", __FILE__, __LINE__); \
                    for (;;) { }                                          \
                }                                                         \
            } while (0)
        ```
    - Forward to a user handler (e.g. a weak `ADVUtils_assertFailed` you can breakpoint or override at link time):
        ```c
        void ADVUtils_assertFailed(const char* file, int line);
        #define ADVUTILS_ASSERT(x)                                       \
            do {                                                         \
                if (!(x)) {                                              \
                    ADVUtils_assertFailed(__FILE__, __LINE__);           \
                }                                                        \
            } while (0)
        ```

## Specific configurations (via defines):
  
### list:
- `LIST_STYPE`: type of list size property, defining the maximum amount of values that can be stored in the list. Default is `uint16_t`
  
### LKHashTable:
- `LKHT_LIST_SIZE`: maximum size of the list objects used by the hash-table. Default it `UINT16_MAX`
- `LKHT_HASHFUN`: hash function used by hash-table. Default is `hash_FNV1A` (FNV-1a hash) and other options are `hash_djb` (Dan Bernstein hash) and `hash_sdbm` (sdbm hash)

### LPHashTable:
- `LPHT_MIN_SIZE`: minimum size of hash-table when resizing. Default is `5`
- `LPHT_MAX_SIZE`: maximum size of hash-table when resizing. Default is `UINT32_MAX`
- `LPHT_MIN_SATURATION`: minimum saturation of hash-table, that triggers a down-sizing. Default is `0.2`, meaning `20%`
- `LPHT_MAX_SATURATION`: maximum saturation of hash-table, that triggers an up-sizing. Default is `0.7`, meaning `70%`
- `LPHT_HASHFUN`: hash function used by hash-table. Default is `hash_FNV1A` (FNV-1a hash) and other options are `hash_djb` (Dan Bernstein hash) and `hash_sdbm` (sdbm hash)

### movingAvg:
- `MOVAVG_STYPE`: type of moving average size property, defining the maximum amount of values that can be stored in the moving average. Default is `uint16_t`

### queue:
- `QUEUE_STYPE`: type of queue size property, defining the maximum amount of values that can be stored in the queue. Default is `uint16_t`

## Tests coverage:
| Object      | Coverage |
| ----------- | -------: |
| basicMath   |     100% |
| button      |     100% |
| event       |     100% |
| fix16       |     100% |
| IIRFilters  |     100% |
| list        |     100% |
| LKHashTable |     100% |
| LPHashTable |     100% |
| matrix      |     100% |
| movingAvg   |     100% |
| numMethods  |     100% |
| PID         |     100% |
| quaternion  |     100% |
| queue       |     100% |
| timer       |     100% |
| vector3f    |     100% |