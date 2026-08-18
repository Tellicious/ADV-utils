## Major update and refactor

**New features:**
- `numMethods`:
  - Added `Cholesky` factorization for symmetric positive-definite matrices
  - Added `LinSolveCholesky` / `LinSolveCholeskyStatic` solvers for symmetric positive-definite linear systems
  - Added `QR_Householder` / `QR_HouseholderStatic` economy Householder QR factorization
  - Added `LinSolveQR` / `LinSolveQRStatic` least-squares linear solvers based on QR
- `quaternion`:
  - Added `quaternionToMatrix` to build the active rotation matrix (DCM) from a quaternion
  - Added `quaternionFromAxisAngle` to build a quaternion from a unit axis and rotation angle
- Other:
  - Added `fix16` module with add, subtract, multiply, divide, sqrt, abs and convert operations on Q16.16 fixed-point scalar 
  - Added `vector3f` module with add, subtract, scale, dot, norm, normalize and cross-product operations on 3-element vectors
  - Added `ADVUTILS_USE_ASSERT_ALWAYS` to keep `ADVUTILS_ASSERT` active in release builds, routed to the same handler as in debug
  - Added Doxygen documentation

**Improvements:**
- `matrix`:
  - Changed `matrixPseudoInv` / `matrixPseudoInvStatic` to use `LUP` linear solver to improve robustness
  - `matrixInversed`, `matrixInversed_rob`, `matrixInversed_SPD`, `matrixPseudoInv` and their `Static` twins now return `utilsStatus_t` instead of `void`, propagating the status of the underlying linear solver (existing call sites that ignore the return value are unaffected)
  - Refactored `matrix` and `numMethods` modules for improved maintenability
- `numMethods`:
  - Changed `GaussNewton_Sens_Cal_9` / `GaussNewton_Sens_Cal_6` / `GaussNewton_Sens_Cal_9Static` / `GaussNewton_Sens_Cal_6Static` to use `QR` linear solver to improve performance 
  - Float linear-algebra solvers now guard against non-finite (NaN/Inf) values: the `Cholesky`, `LU_Crout` and `LU_Cormen` factorizations (and their `Static` twins) reject non-finite pivots, and `LinSolveCholesky`, `LinSolveQR` and `QR_Householder` additionally validate their computed result/factors, returning `UTILS_STATUS_ERROR` instead of propagating NaN/Inf
  - `LinSolveLU`, `LinSolveLUP` and `LinSolveGauss` (and their `Static` twins) now return `utilsStatus_t` instead of `void`, reporting `UTILS_STATUS_ERROR` on a singular system or a non-finite (NaN/Inf) computed result; the underlying `LUP_Cormen` / `LUP_CormenStatic` factorizations additionally reject non-finite pivots
- Documented the assertion model (debug vs. release, override hooks and example handlers) in the Readme
  
See [Changelog](Changelog.md)