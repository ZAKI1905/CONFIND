# CONFIND 2.0.0

CONFIND is a standalone C++17 numerical contour library based on CONREC. It depends
on ZakiLib 2.x and provides deterministic `std::thread` parallel scalar-field
sampling, serial contour extraction, and numerical export. It has no ROOT, Python,
OpenMP or plotting dependency. Visualize exported data externally, normally in
Python.

Historical raw point order, curve order, radius-based de-duplication, saddle
handling, exact equality and NaN behavior are intentionally retained in 2.0 pending
separate scientific adjudication. Radius de-duplication can discard geometrically
distinct points. The historical arm64 characterization is the numerical authority;
see [qualification](docs/modernization/CONFIND_2_0_REPORT.md).

## Build and consume

Install ZakiLib 2.x first, then provide its installation prefix to CMake:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/zaki-install -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
cmake --install build --prefix /path/to/confind-install
```

Consumer CMake:

```cmake
find_package(CONFIND 2.0 CONFIG REQUIRED)
target_link_libraries(my_program PRIVATE CONFIND::CONFIND)
```

The package resolves `Zaki::Zaki` and `Threads::Threads` transitively. CONFIND
applies `-ffp-contract=off` privately to its numerical translation units on
AppleClang, Clang and GNU. It does not change consumer floating-point flags.
Reference equality assumes identical evaluator/sample values and the qualified
arithmetic environment; arbitrary evaluator compilation or nondeterministic
external solvers are outside that guarantee.

## Deterministic parallel evaluation

```cpp
#include <Confind/ContourFinder.hpp>

CONFIND::ContourFinder finder;
finder.SetGrid({{{-2, 2}, 64, "Linear"}, {{-2, 2}, 64, "Linear"}});
finder.SetContVal({1.0, 1.7});
finder.Evaluate([](std::size_t /* worker */) {
    return [](double x, double y) { return x*x + y*y; };
}, CONFIND::ThreadingOptions{4});
auto contours = finder.GetContourSet();  // Raw order and multiplicity are retained.
```

Each factory call must create an independent evaluator, including its mutable
stellar builder/EOS/cache state. Copying a wrapper does not prove independence.
A shared target is permissible only when the caller guarantees it is immutable
or thread-safe. Deterministic results must depend on `(x,y)`, not worker identity,
call order, or the number of calls received by a worker.

A fixed pool lives for one `Evaluate` call. An atomic index distributes work
dynamically; workers write `values[k]`, where `k=i+(nx+1)*j`. The pool joins before
serial extraction in historical level, row, cell, triangle and point order. There
is no shared contour assembly. Factory construction and evaluator destruction run
serially on the caller. Worker exceptions cancel pending tasks, join every worker,
and rethrow the first observed exception on the caller, without publishing partial
sample contours. Already-running evaluations finish during cancellation. The
finder itself must not be mutated concurrently, including from an evaluator.

Worker count defaults to 1; zero also selects 1. Requests are capped by the sample
count. Callers choose a count appropriate to their CPU and any inner solver
threading. `hardware_concurrency()` may be used as an optional hint.

`Evaluate` samples linear coordinates as `min+i*delta`; log axes apply the same
expression in log10 space and then `pow(10, ...)`. The existing sampled-grid API
`SetGridVals(GridVals_2D*)` uses supplied values without changing indexing/defaults.
Historical `SetFunc`/`SetMemFunc` plus `SetGridVals(Fast/Normal)` remain **serial** and
preserve their own coordinate rounding/cache behavior. On non-dyadic grids those
historical callable paths need not equal direct node sampling. Both behaviors are
characterized separately; use `Evaluate` for expensive parallel sampling.

`SetMemFunc` now takes `std::unique_ptr<Zaki::Math::Func2D>`. `Parallel`, `Ludicrous`,
`Optimal`, `SetThreads`, plotting, color and legend APIs have been removed. No
compatibility plotting stubs remain. Use `Clear()` before reconfiguring a completed
finder for a new independent calculation; adding levels retains found levels.

## Output and qualification

`GetContourSet()` returns raw points; `ConvertToCurve2D()` applies the retained
historical de-duplication/chaining. An empty contour now converts safely to an empty
curve. `SetWrkDir` and `ExportContour` retain numerical TSV export; long paths are
no longer truncated. Valid historical export bytes are frozen in the tests.

The suite includes 52 historical serial fixture variants, thread-count equivalence,
20 scheduler-perturbed runs per worker count, ownership and exception tests, and an
installed package consumer. The frozen references are never regenerated from
modern output. Build with `-DCONFIND_BUILD_BENCHMARK=ON` to run
`build/tests/confind_benchmark [cost]`; this records deterministic variable CPU
work and throughput, without a required speedup.

This is an untagged 2.0.0 candidate awaiting independent review and owner acceptance.
CompactStar migration is separately authorized; see the
[migration record](docs/modernization/COMPACTSTAR_MIGRATION.md).
