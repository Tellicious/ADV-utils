# Changelog

## v1.17.2

**New features:**
- Updated runTests action to upload results to Codecov

**Improvements:**
- Changed `timer` object to improve behavior on wrap-around and extended unit-tests to include all branches
- Removed unnecessary check from `LKHashTable`
- Extended `button` unit-tests coverage

**Bugfix:**
- Minor fix to `button.h`

## v1.17.1

**Bugfix:**
- Moved `LKHashTable` and `LPHashTable` configurable parameters to source file
  
## v1.17.0

**New features:**
- Added some configurable parameters (via defines) to `list`, `LKHashTable`, `LPHashTable`, `movingAvg` and `queue`
- Added `queuePopBackArr` function to `queue`
  
**Improvements:**
- Improved performance of `LPHashTable` by removing unnecessary memory releases / allocations
- Extended unit tests coverage to 100% for all objects
  
**Bugfix:**
- Fixed several bugs in `LPHashTable` that were causing relevant functional errors
- Fixed a few bugs in `LKHashTable` that were causing memory leaks in some conditions
- Minor bugfix to `matrixDet` and `matrixDetStatic`
- Minor bugfix to `quaternionToEuler`
- Minor bugfix to `numMethods`
- Other minor improvements and bugfixes

## v1.16.0

**Improvements:**
- Major change to `PID_calcBackCalc` to align output with Simulink PID block
- Optimized PID calculation functions to remove 2 unnecessary struct members
- Changed many inline functions to defines in `PID.h`
- Extended PID unit tests to validate also set, get and reset functions

## v1.15.0

**New features:**
- Added function to delete event object
- Added functions to delete hash-table objects
- Added unit tests for all functions

**Improvements:**
- Improved `timer.c` function to detect multiple events
- Added asserts to check input parameters correctness in `IIRFilters.c`
- Changed `fastSqrt` and `fastInvSqrt` to return `NaN` if a negative value is given as input
- Minor change to `PID_calcAeroClamp` to fully stop integration when saturation is reached
- Minor improvement to `PID_calcBackCalc`
- Changed return type of `eventInit`
- Minor change in LPHashTable resizing logic
- Added some more asserts to check input parameters to `matrix.c`

**Bugfix:**
- Fixed a bug causing an endless loop on certain conditions on `lpHashTableGet`
- Removed useless and not working iterator from `LPHashTable`
- Bugfix on `BIT_TOGGLE`, `BIT_TOGGLE_IN_PLACE`, `MAX` and `MIN` macros
- Major bugfix on `PID_calcIntegralClamp`
- Bugfix on `listInsert`
- Minor bugfix on `matrixInversedStatic_rob`
- Minor bugfix to `numMethods.h`
- Minor bugfix on `event.h`
- Fixed descriptions in some headers

## v1.14.0

**New features:**
- Added callback-based event manager in `event.h/.c`

**Improvements:**
- Added check on correct memory allocation using `ADVUTILS_ASSERT`

## v1.13.0

**New features:**
- Added `DARE` and `DAREStatic` functions to `numMethods` to solve discrete-time algebraic Riccati equation

**Bugfix:**
- Minor formatting fix

## v1.12.0

**Improvements:**
- Added band-pass and band-stop filter coefficients calculation to `IIRFilters.c` to simplify initialization

**Bugfix:**
- Fixed minor bug in `IIRFilters.c`

## v1.11.1

**Improvements:**
- Added assertions to `matrix.c` and `numMethods.c`

## v1.11.0

**New features:**
- Added low-pass and high-pass filter coefficients calculation to `IIRFilters.c` to simplify initialization

**Improvements:**
- Moved faster math functions macros to `basicMath.h`

## v1.10.1

**Improvements:**
- Changed `quaternion.c` to give the user the possiblity to choose whether to use fast or standard math functions through the addition of `USE_FAST_MATH` to compile definitions

## v1.10.0

**New features:**
- Added bitwise operations macros to `basicMath`
- Added `fastSin` and `fastCos` to `basicMath`

## v1.9.1

**Bugfix:**
- Fixed typo in `CMakeLists.txt`

## v1.9.0

### Contains breaking change on memory management. `ADVUTILS_USE_STATIC_ALLOCATION` and/or `ADVUTILS_USE_DYNAMIC_ALLOCATION` must be defined by user

**New features:**
- Added `*Static` variant of multiple functions with static memory management
- Added possibility of setting static and/or dynamic memory management for ADVUtils library via definition of `ADVUTILS_USE_STATIC_ALLOCATION` and `ADVUTILS_USE_DYNAMIC_ALLOCATION`. At least one of the two must be defined by the user
- Added possibility of using FreeRTOS-specific dynamic memory management functions with `set(ADVUtils_DYN_MEM_MGMT "USE_FREERTOS" CACHE STRING "" FORCE)` in CMake

## v1.8.1

**Bugfix:**
- Initialized some variables in `GaussNewton_Sens_Cal` to avoid a `-Wmaybe-uninitialized` warning

## v1.8.0

**New features:**
- `GaussNewton_Sens_Cal` can now calculate automatically both sphere radius and starting conditions, that are no longer needed as input parameters
- `GaussNewton_Sens_Cal` returns now an error if `NaN` is detected

## v1.7.2

**Improvements:**
- Changed return status of `GaussNewton_Sens_Cal` so that now it returns `UTILS_STATUS_TIMEOUT` when the maximum number of iterations is reached

## v1.7.1

**Improvements:**
- Improved robustness of `quaternionNorm`, including check on infinite and nan

## v1.7.0

**New features:**
- Added `CMakeLists.txt`

**Improvements:**
- Fixed some typos

## v1.6.0

**New features:**
- Added `utilsStatus_t` to commonTypes.h

**Breaking changes:**
- Updated all libraries to use `utilsStatus_t` instead of their own status enum

**Improvements:**
- Minor formatting changes

## v1.5.0

**New features:**
- Added commonTypes.h containing common typedefs (`axis3f_t`)

**Breaking changes:**
- Updated quaternion object to use commonTypes.h

## v1.4.0

**Improvements:**
- Added reset functions to IIRFilters

## v1.3.0

**New features:**
- Added IIR filter objects (generic, derivative and integrator)

**Improvements:**
- Fixed snippets
- Renamed a few variables inside PID object

## v1.2.0

**Improvements:**
- Added deadband macro to basicMath and minor changes

## v1.1.1

**Improvements:**
- Reformatted entire code

## v1.1.0

### Contains breaking change on button initialization function

**New features:**
- Added possibility to create a pulse-detection button via `BUTTON_TYPE_PULSATING`
- Added new very-long press detection
- Added buttonGetStatus to return debounced button status

**Improvements:**
- button resetTicks are now defined during initialization and no longer a macro
- Reduced button_t memory footprint by using bitfields and by using smaller types
- Minor typo correction on LKHashTable and LPHashTable

## v1.0.0

- Initial release