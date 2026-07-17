# ADV-Utils — MISRA C:2012 Deviation Record

Analyser: cppcheck 2.22 + `misra.py` addon, both allocation modes
(`ADVUTILS_USE_DYNAMIC_ALLOCATION`, `ADVUTILS_USE_STATIC_ALLOCATION`), `--std=c99`.
Run MISRA checks with `--inline-suppr` so the in-code suppressions below are honoured.

Every finding not listed here has been fixed in the code. The items below are
retained deliberately, with rationale. 

---

## Project decisions

### Rule 21.3 — use of `malloc`/`calloc`/`free`
Files: list, LKHashTable, LPHashTable, event, matrix, movingAvg, queue.
The dynamic containers require heap allocation in their dynamic-allocation build
mode. Allocation is centralised behind `ADVUTILS_MALLOC/CALLOC/FREE`
(`inc/ADVUtilsMemory.h`), so an integrator can redirect it to a pool allocator if
a fully static build is not used.

### Rule 18.8 — variable-length arrays
Files: matrix, numMethods.
The static-allocation matrix/solver paths size scratch buffers from run-time
matrix dimensions (`float buf[rows * cols]`). VLAs are intrinsic to this design
and cannot be removed without changing the public static API.

### Rule 11.5 — conversion from `void *` to object pointer
Files: LKHashTable, LPHashTable, list, matrix, movingAvg, queue.
The generic containers store elements as `void *` and hand typed pointers back to
the caller; the `void *`->object conversions are inherent to a type-erased
container.

---

## Legitimate idioms (compliance would harm the code)

### Rules 18.4 / 13.3 / 10.3 — pointer walking in hot loops
Files: matrix.c (18.4, 13.3, 10.3), queue.c (18.4).
The matrix multiply/transpose kernels and the queue wrap-around `memcpy`s use
pointer arithmetic (`pIn2 += rhs->cols`, `(uint8_t *)data + offset`) as a
deliberate optimisation on the hottest paths. 18.4 and 13.3
are advisory; rewriting to indexed access risks measurable slowdown and subtle
regressions. Behaviour is covered by the `matrix` and `queue` test suites.

### Rules 10.1 / 12.2 — signed fixed-point (Q15) bit manipulation
File: basicMath.c — `fastSin` interpolation (`angleInt >> INTERP_BITS`, `~v0`,
`v0 & FLIP_BIT`, `v0 &= TABLE_MASK`, the interpolation shift) and the `sin90`
table initialiser. The `FLIP_BIT`/`NEGATE_BIT`/`TABLE_MASK`/`INTERP_MASK` macros
expand to signed constants and the algorithm relies on signed (arithmetic) shift
and signed complement to fold the quarter-wave table; forcing unsigned essential
types would change the computed sine. Values are verified against golden
references in the `basicMath` test (`fastSin`/`fastCos`). Note: the analogous
`fastSqrt` bit-hack *was* made compliant (its union is `uint32_t` and the magic
constants use `(uint32_t)1 << n`), because there the operand is provably
non-negative.

### Rule 19.2 — union type punning
File: basicMath.c — `fastSqrt`/`fastInvSqrt` reinterpret a `float` and a 32-bit
integer through a union. This is the well-known fast (inverse) square-root idiom;
19.2 is advisory and the punning is intentional and portable for the target.
Verified by the `basicMath` test.

### Rule 11.1 — function-pointer conversion
File: event.c — the event list stores two callback shapes
(`void (*)(void)` and `void (*)(void *)`) in one array and casts on
registration/dispatch. This dual-callback design is intentional; a compliant
rewrite would need a tagged union and a wider public API change.

---

## Analyser false positives (suppressed in-code)

### Rule 20.4 — macro named as a keyword (basicMath.h: constPI/constG/constE)
The addon flags these `const`-prefixed identifiers; none is a keyword.
Suppressed inline.
