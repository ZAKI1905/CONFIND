# Later CompactStar migration (not performed or authorized here)

Authenticated consumer checkout: `812463ac9ed374f64ac9cadd500066ab723d3a6c`.
No CompactStar or canonical Zaki file was changed by this task.

A separately authorized migration must:

1. Replace vendored Zaki/CONFIND with `find_package(Zaki 2.0 CONFIG REQUIRED)` and
   `find_package(CONFIND 2.0 CONFIG REQUIRED)`, using their exported targets.
2. Remove the three active `ContourFinder::Plot` calls and the three active
   `SetPlotConnected` calls in `CompactStar/Core/src/TaskManager.cpp`.
3. Remove `Curve2D::Plot` calls throughout the consumer and visualize numerical
   exports externally. Remove Python/NumPy only after verifying that no other
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
7. Retain frozen radius-based de-duplication, NaN handling and CONREC topology until
   a separate scientific decision and re-qualification authorize changes.

The bounded C23 fixture in this branch freezes raw ordering, a maximum scan
excluding the last point, mass-curve start, intersections, GetIdx and Bisect output.
It does not execute TaskManager, a neutron-star solver, or an EOS campaign. That
remaining integration gap is explicit. No cluster campaign or release is implied.
