# Historical gate (frozen before modernization)

Reconstruction commit: `6c68b51c1105da34910c36530b9e2bd6bfeb3023`.
Source-basis commit: `4a082603cb6bfb8ac062ee318a2b0fe8658f82c7`.
Predeclaration commit: `b51c4c6c6d31e99d09da48f41272473c9ad11944`.

AppleClang 21.0.0, arm64, C++17, -O0 -ffp-contract=off. Historical
archives/headers are authenticated in the predeclaration. The historical executable
links their required GSL, zlib, libomp and Python3.12 runtime; no plotting or Python
execution was requested. Those dependencies belong only to the historical oracle,
not the maintained library. Canonical Zaki was built from clean c8c68131 in a fresh
out-of-tree Debug build with AppleClang, tests/examples disabled, then installed
outside its checkout. The reconstruction links that package's library and public
headers. All five reconstructed units compile successfully. Disassembly of the
reconstruction archive contains zero fmadd/fmsub/fnmadd/fnmsub/fmla/fmls instructions.

The oracle serial output was generated twice, byte-identical, and frozen/hash-
recorded before first reconstructed execution. Oracle/reconstructed output:
`c16766f02711212f5d7dc502a284ea4275c2bf981b57b2ecfeaec28b7544ad83`.
All 5,844 lines agree exactly. Export output for two named levels also matches
byte-for-byte. `tests/reference/SHA256SUMS` is the authoritative artifact ledger.

## Coverage and record schema

F01=C1; F02=C2; F03=C3; F04=C4; F05=C5/C9; F06/F07=C6;
F10=C7; F11=C8; F12=C10; F09=C11; F13=C12; explicit C13;
explicit C14; F15 plus non-dyadic C15; historical-only C16=F16;
F18=C17; F14=C18; F17=C19; F19=C20; explicit C21/C22/C23.
Fixture variants intentionally extend the owner's minimum 23 categories.

FIXTURE: name, contour count. CONTOUR: index, hex level, level bits, found,
raw count, exact xyz equality-distinct count, bit-pattern xyz distinct count,
rounded-radius bit classes. RAW: index, hex xyz and all three IEEE bit patterns.
CURVE/POINT: complete ordered curve xy, hex and bits. NaN payload bits are retained.
Repeated raw rows preserve multiplicity. Empty/unfound conversion is deliberately
not invoked; its marker is not an expected value for historically undefined code.

C23 freezes raw critical order and a bounded maximum-over-all-but-last operation,
mass_curve[0], intersection sequence, GetIdx and both Bisect sizes plus first-half
point sequence. The historical intersection API emits repeated intersections in
this fixture; those are retained, not repaired. Full TaskManager/star/EOS campaign
qualification remains outside scope and must be done during later migration.

## Historical defects (not threaded acceptance targets)

Empty found-contour conversion: UB, avoided. Callable multi-level Fast executes the
historical new[]/delete mismatch: its repeatable numerical output is evidence,
not proof of memory safety. Vendored diagnostic ODR/layout defects are inherited.
Parallel/Ludicrous results are stored in two separate evidence artifacts only;
they are not modern goldens. ny>nx unsafe threaded configurations are not executed.
NaN curves and radius de-duplication defects remain frozen numerical semantics.
Reversed axes are recorded, not silently prohibited during modernization.

## Reproduction

The complete reconstruction harness and historical OpenMP harness are preserved
as text under docs/provenance. Build them with the exact authenticated vendored
headers and archives using the compile/link recipe in the reconnaissance report
Appendix A.3, adding -ffp-contract=off; use the serial harness for serial.tsv.
Compile that same serial harness against reconstruction commit 6c68b51 and fresh
canonical Zaki headers/library. Compare files byte-for-byte, never regenerate
reference files from modern output. `tests/export.cpp` uses the same pairings and
compares the two generated contour_*.tsv files. Modern tests consume the frozen
files without requiring access to historical archives or their dependencies.
