# Later CompactStar migration (not performed or authorized here)

Authenticated consumer checkout: `812463ac9ed374f64ac9cadd500066ab723d3a6c`.
No CompactStar or canonical Zaki file was changed by this task.

CONFIND's [owner acceptance](CONFIND_2_0_ACCEPTANCE.md) establishes library-level
authority on the qualified local Mac configuration only. It does not grant
TaskManager stellar, Linux, EKU cluster or cross-platform bitwise authority.
The constraints below are mandatory for the separate migration predeclaration
and must be characterized and resolved before scientific authority is granted.

A separately authorized migration must:

1. Replace vendored Zaki/CONFIND with `find_package(Zaki 2.0 CONFIG REQUIRED)` and
   `find_package(CONFIND 2.0 CONFIG REQUIRED)`, using their exported targets
   `Zaki::Zaki` and `CONFIND::CONFIND`. Remove vendored headers/libraries and
   binaries from the active consumer integration under that separate authority.
2. Remove the three active `ContourFinder::Plot` calls and the three active
   `SetPlotConnected` calls in `CompactStar/Core/src/TaskManager.cpp`.
3. Remove `Curve2D::Plot` calls throughout the consumer and visualize numerical
   exports externally. Carry the wider Zaki 2.0 plotting-removal migration,
   including `DataSet::Plot`, `LogLogPlot`, `SemiLog*` and `PlotParam` use in EOS
   and Extensions. Remove Python/NumPy only after verifying that no other
   consumer dependency still needs them.
4. Add TaskManager regression fixtures on real sequence/EOS data before science
   re-qualification. Preserve `FindCriticalCurve`'s raw `GetContourSet` order and
   point sequence; its all-but-last loop and empty-contour underflow require
   explicit review, not an incidental rewrite.
5. Preserve `FindMtotContour` curve order and `mass_curve[0]`, which sets later
   B_tot sampling limits. Preserve `FindBtotContour` intersection sequence and
   `Bisect(...).first` cut-index behavior.
6. Use an explicit independent evaluator factory when parallelizing expensive
   stellar sampling. Do not share mutable builders via copied wrappers. Decide
   the sampling-coordinate contract before replacing historical callable paths;
   direct indexed sampling and legacy Fast rounding may differ on non-dyadic grids.
   Include TaskManager's existing static-partition threading in the consumer
   qualification. Account for accepted N-4 resampling/no-cache, all-or-nothing
   exceptions, completion of in-flight work and lack of a small explicit worker
   ceiling. CONFIND export now preserves complete long paths; carry that behavior
   into consumer output/path checks.
7. Retain frozen radius-based de-duplication, NaN handling and CONREC topology until
   a separate scientific decision and re-qualification authorize changes.
8. Qualify N-1 Coord3D weak-symbol/interposition behavior using the actual
   CompactStar consumer translation units, floating-point compilation policies
   and link resolution. A consumer may alter weak `Coord3D::operator<` / `XYDist2`
   resolution and change radius de-duplication under some build configurations.
   Do not infer consumer safety from CONFIND-only qualification. `Coord3D`,
   `operator<`, `XYDist2`, `RMDuplicates` and `SortNew` remain frozen pending
   separately governed scientific adjudication.
9. Characterize and resolve the M-1-class optimized log-axis `pow`/`exp10`
   exposure outside CONFIND. Independent review found canonical Zaki Release
   may use `__exp10` in `Axis::operator[]` (Log branch), affecting paths including
   `GridVals_2D::Interpolate`; historical vendored Zaki used `pow`. Compare the
   actual canonical Zaki log-axis arithmetic and interpolation against historical
   authority in the declared optimized consumer configurations. CONFIND's private
   HistoricalMath fix does not qualify these external paths.
10. Characterize CompactStar TaskManager's own `pow(10,...)` sites, including
    log task-range construction, and its `GridVals_2D::Interpolate` paths. Compare
    optimized `pow` versus `exp10` behavior and resolve any stellar-equivalence
    consequences under the separate predeclaration before scientific authority.
11. Require full TaskManager stellar-output equivalence/regression evidence on
    declared real sequence/EOS inputs, including raw critical-curve ordering,
    `mass_curve[0]`, intersections and Bisect indices. Library-level exactness
    alone cannot establish this authority.
12. Keep Linux and EKU cluster qualification later and separate, after the
    governed local consumer and stellar evidence. Qualify compiler-specific
    builtin/contraction controls and the CMake 3.22 minimum for those environments;
    local Darwin GCC/Clang evidence is not Linux/cluster or cross-platform bitwise
    authority.

The bounded C23 fixture in this branch freezes raw ordering, a maximum scan
excluding the last point, mass-curve start, intersections, GetIdx and Bisect output.
It does not execute TaskManager, a neutron-star solver, or an EOS campaign. The
synthetic 99×150 log-log fixture also does not execute a stellar solve. That
remaining integration gap is explicit. No cluster campaign or release is implied.

The accepted R-N1 regression-protection gap is a deferred test-only follow-up:
independent review verified all six historical inverse-log site groups, while
the committed suite directly discriminates only `case36`. This does not reopen
the accepted CONFIND M-1 correction or authorize a production change here.

The exact next action after CONFIND canonical integration is a separately
authorized CompactStar migration / TaskManager stellar-equivalence predeclaration
carrying all requirements above. No migration begins in the acceptance task.
