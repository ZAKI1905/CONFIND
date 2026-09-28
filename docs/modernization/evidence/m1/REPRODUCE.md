# M-1 qualification evidence

All execution was local Darwin arm64 on 2026-09-28. Qualified implementation:
`fe5cf02b9fd3e7b633e68f310a2eb74cb6d1ffe0`. The final documentation commit adds
this record and fixes new ledger entries to the existing repository-relative
path convention without changing any expected byte or digest.

## Reference authority

Run `shasum -a 256 -c tests/reference/SHA256SUMS` from repository root. Then,
with the authenticated vendored CompactStar archive/header tree available:

```sh
COMPACTSTAR=/Users/keeper/Documents/CompactStar/repo/CompactStar \
OUTPUT=/private/tmp/confind-m1-oracle-replay bash tests/m1_oracle.sh
```

That script verifies both archive digests, compiles the three committed oracle
harnesses at -O0 with contraction-off, generates each artifact twice and compares
against references. Original 52-variant and export reproduction used
`docs/provenance/characterize_reconstruction.cpp.txt` and `tests/export.cpp` with
the same authority link closure; exact local commands are in
`original_oracle.py.txt`. The historical link's Python/libomp requirements are
not maintained-package dependencies.

The pre-fix gate is committed at dbc51f9 before e4309f3 changes production.
Reproduce it using b243851 library sources and the harness/test files from
dbc51f9. Do not expect a final-candidate rebuild to reproduce the old failure.
`pre-fix-gate.json` records full output hashes and first differences. The small
fixture changes curve count; its positional difference count includes added
lines. `baseline-randomized.json` reproduces the independent review's 840/3621.

## Final build matrix

Use a fresh external canonical-Zaki installation and fresh build directories:

```sh
cmake -S . -B /private/tmp/confind-m1-replay-release \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
  -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" \
  -DCMAKE_PREFIX_PATH=/path/to/fresh-zaki-prefix \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCONFIND_BUILD_BENCHMARK=ON
cmake --build /private/tmp/confind-m1-replay-release --parallel 4 --verbose
ctest --test-dir /private/tmp/confind-m1-replay-release --output-on-failure -j 3
```

Repeat Debug. For ASan+UBSan, build both canonical Zaki and CONFIND in fresh
Debug directories with `-fsanitize=address,undefined -fno-omit-frame-pointer`;
run with `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. TSan uses
`-fsanitize=thread -fno-omit-frame-pointer` on both libraries and
`TSAN_OPTIONS=halt_on_error=1`. Intentional-race control source/log is retained;
its data-race failure is expected, separate from the clean 29/29 product suite.
Instrumentation imports: CONFIND ASan 94 / UBSan 33; canonical Zaki ASan 528 /
UBSan 178; TSan CONFIND 67 / Zaki 360. These are imported-symbol-line counts,
not counts of instrumented memory operations.

`*-final-tests.log` are the fresh full-matrix results. Each includes serial,
export, empty-contour/exception/ownership, package, differential, worker-count,
and stress checks. `*-stress-hashes.tsv` contain all 960 matrix stress hashes;
`stellar-stress-hashes.tsv` contains the additional 100 large log-log perturbations.
`final-equivalence.json` records exact matching hashes for all four new contour/
differential references in each matrix configuration. No approximate comparator.

## Code generation

`final-*-symbols.txt`, `final-*-helper-disassembly.txt`, `final-codegen.json` and
`*-compile-commands.json` preserve artifact and flag evidence. Disassemble the
complete archive (not source text), scan for fmadd/fmsub/fnmadd/fnmsub/fmla/fmls,
and check undefined symbols for pow/exp10:

```sh
nm -u /path/to/build/libCONFIND.a
xcrun llvm-objdump --disassemble --reloc /path/to/build/libCONFIND.a
xcrun llvm-objdump --disassemble --reloc \
  /path/to/build/CMakeFiles/CONFIND.dir/source/Internal/HistoricalMath.cpp.o
```

The final helper branches to `_pow`. The independent control in
`control-source.cpp.txt`, compiled at -O3 -ffp-contract=off without no-builtin,
imports `___exp10`; its artifact symbols and disassembly are retained. Compile
commands show -fno-builtin and -fno-lto on exactly one production TU. A separate
Release build with CMAKE_INTERPROCEDURAL_OPTIMIZATION=ON passes 29/29
(`lto-tests.log`). AppleClang/Clang/GCC policy compile commands each show
`-std=c++17`; all three policy test logs pass 29/29. GCC consumed its own fresh
canonical-Zaki build to use the matching standard-library ABI.

## Thread-test discrimination

On independent scratch copies only, repeat the review's exact mutations:

- M13: replace `while (!cancelled.load(std::memory_order_relaxed))` with
  `while (true)` in IndexedExecutor.hpp.
- M14: replace `if (count <= 1)` with `if (count >= 1)` there.
- M15: replace Evaluate `x0+i*delta_x` by
  `x0+(i*((log_x?log10(grid.xAxis.Max()):grid.xAxis.Max())-x0))/nx`, and the
  analogous y expression.

Build/run confind_thread_discrimination with tests/reference/m1-nodes.tsv.
All three fail with the intended diagnostic; see M13/M14/M15-result.log. The
unmutated execution records all requested worker counts and both coordinate
modes in thread-discrimination.log. No production test hook was introduced.

## Package and benchmark

Fresh canonical Zaki and CONFIND were installed into one external prefix, then
that prefix was renamed. The consumer uses only find_package and CONFIND::CONFIND.
`package-result.json`, consumer build/test logs and package-linkage.txt document
success, no source/original-prefix paths in CONFIND exports, no FP/builtin/LTO
consumer flag leak, and no OpenMP/ROOT/Python dependency. Maintained source has no
plotting or vendored-Zaki restoration.

`benchmark.tsv` is the unchanged deterministic 4,225-task CPU benchmark, cost
40,000, run after the matrix completed. All worker checksums match. Run the fresh
Release tests/confind_benchmark with argument 40000 to repeat. There is no fixed
performance threshold and no real stellar campaign claim.

The `*.py.txt` files preserve exact local execution scripts with historical local
paths and dependencies. They are audit records rather than portable build tools;
use the recipes above with fresh paths for replay. They do not replace the old
full-source audit normalizer deferred as N-6. INDEPENDENT_REVIEW.md preserves the
complete review read before predeclaration.
