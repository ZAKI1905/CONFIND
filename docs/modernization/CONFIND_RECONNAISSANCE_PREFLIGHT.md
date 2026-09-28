# CONFIND — Independent Reconnaissance and Modernization Preflight

| Field | Value |
|---|---|
| Status | **RECONNAISSANCE COMPLETE — OWNER DECISIONS REQUIRED (stage C0)** |
| Date | 2026-09-28 |
| Task class | Read-only reconnaissance and modernization planning, local Mac only |
| Final disposition | **A — CONFIND ARCHITECTURE FUNDAMENTALLY SOUND — SOURCE BASIS IDENTIFIED — BOUNDED MODERNIZATION READY FOR OWNER REVIEW** |

**Authorities examined**

| Authority | Identity |
|---|---|
| CONFIND Git (this repository) | `89c5d9b731534e4289d9f686549d9f0ac178e567` |
| Canonical ZakiLib | `c8c68131b04e9d216673725075bef38df81e6041` |
| Recovered CONFIND source snapshot (manifest SHA-256) | `ed76163c22e0a1f8ba3f71f62f5a56527528850650bbf7127da9c0c908d14083` |
| CompactStar | `812463ac9ed374f64ac9cadd500066ab723d3a6c` |
| CompactStar vendored `Darwin/arm64/libConfind.a` | `09ed1a7c43a83b42f64ee8e0bda3b879af970126ee75159a802179a4d0a49eb2` |

> **This record authorizes nothing.** It does not select a source basis, change any contour
> semantics, or authorize a branch, build, migration or release. It exists so that the owner can
> make the stage-C0 decisions listed in §39.

"Sound" applies to the numerical core, which is Paul Bourke's CONREC algorithm. It does not apply
to the step that turns points into curves: radius-ordered de-duplication followed by greedy
nearest-neighbour chaining. That step is defective, but CompactStar's results depend on it, so it
is frozen as Class-C (§30).

Label conventions used throughout: **lineages** C1 (current Git), C2 (recovered snapshot),
C3 (CompactStar vendored arm64 binary; C3′ = the vendored x86_64 binary). **Fixtures** are written
"fixture C#" (§23) and **stages** "stage C#" (§31).

---

## Summary of findings that are new relative to the prior evidence

1. **The vendored arm64 `ContourFinder::Plot()` is an empty 8-instruction stub.** It stores its
   arguments and returns.
   - The prior recovery record (CompactStar `docs/validation/PHASE6_CONFIND_SOURCE_RECOVERY.md` §11)
     says the active `Plot` converts contours to `DataSet` and draws PDFs. That is true only of the
     recovered source and — judging by its imports — the x86_64 archive.
   - In the recovered source, `Plot()` → `ConvertToDataSet()` → `SortNew()` de-duplicates and
     re-orders the stored contours in place.
   - So **the recovered source as-is is not behaviourally equivalent** on CompactStar's
     `FindCriticalCurve` path, which calls `Plot()` before `GetContourSet()`.
   - Fixture F19 on the vendored binary: the raw point sequence is bitwise identical before and
     after `Plot()`.
2. **The two vendored archives are different builds.**
   - arm64: built 2023-09-16, macOS 13 SDK, uid 501, UCR-Drive source path, no `ConvertToDataSet`.
   - x86_64: built 2023-09-21, SDK 11.1, uid 503, `/Volumes/GoogleDrive` path, includes
     `ConvertToDataSet` and imports the DataSet plotting API.
   - The arm64 banner prints **"1.0 / 05, 01, 2023"**, while the vendored `ConfindConfig.h` says
     `09, 21, 2023`. The binary's CMake configure ran on 2023-05-01; the headers came from a later
     configure.
3. **Recovered source minus two things is structurally identical to the vendored arm64 objects,
   function by function.** The two things are `Cont2D::ConvertToDataSet` and the body of
   `ContourFinder::Plot`. The recovered source was compiled in scratch at `-O0` and compared on:
   - call targets, floating-point opcode sequences and branch conditions for every CONFIND function;
   - field offsets and integer immediates for the 22 functions on CompactStar's path.

   The remaining differences are compiler artefacts only.
4. **FMA contraction.** Apple clang 21 fuses `a*b+c` into `fmadd` even at `-O0`: 15 fused
   instructions in the core objects, against 0 in the vendored binary.
   - Affected expressions: CONREC's edge interpolation, the cell origin `G + i·Δ`, and `XYDist2`.
   - With `-ffp-contract=off` the probe emits 0 fused instructions and its FP sequences match the
     vendored binary exactly.
   - **Bitwise equivalence with the vendored oracle requires `-ffp-contract=off`.**
5. **Radius ordering of `Coord3D` provably discards distinct contour points** (measured on the
   vendored binary):
   - origin-centred circle: 56 distinct points → 8;
   - line `x + y = 2.5`: 11 → 6;
   - two circles placed symmetrically about the origin: 32 → 9;
   - at CompactStar-like scale (coordinates ~3·10¹⁵, off-origin, anisotropic) it removed **only
     ulp-level twins**: 370 → 368 and 307 → 300.

   Changing it would change CompactStar outputs.
6. **The vendored archive pairing has live layout mismatches in Zaki's diagnostic types.** Observed
   in scratch executables linked from the same two vendored archives that CompactStar links;
   expected identically in CompactStar's own executables (not directly verified there).
   - **`LogEntry`**: CONFIND's own `Z_LOG_ERROR` messages are **silently dropped** (demonstrated).
   - **`InstrumentationTimer`**: the linked image constructs each timer with libConfind's old
     constructor and destroys it with libZaki's new strong destructor (demonstrated).
7. **NaN inputs corrupt the output curve.** A NaN sample is classified as "on the level" and emits
   points carrying `z = NaN`. `operator==` can never match those points, so `SortNew` keeps
   re-inserting one of them until the output is full. In the fixture the true contour below y = 3
   was lost and the curve ended with (2,2) repeated 8 times.
8. **Parallel/Ludicrous modes take the x-resolution from the y-axis** (`res_x = grid.yAxis.res`).
   Present in the vendored binary (a load from offset `#0xe8` instead of `#0xb8`). Fixture F16e shows
   the contour truncated: 60 vs 156 raw points.

---

## 1. Authenticated identities

| Identity | Result |
|---|---|
| **A. CONFIND Git** `external/CONFIND` | HEAD `89c5d9b731534e4289d9f686549d9f0ac178e567`; `master` tracks `origin/master` (+0/−0); remote `https://github.com/ZAKI1905/CONFIND.git`. Live `ls-remote`: only `HEAD` = `refs/heads/master` = `89c5d9b`. 64 commits, all reachable (2019-12-26 → 2021-09-27; last code commit 2020-04-09). 0 tags, 1 branch. Reflog contains only the clone (2026-08-31). `fsck`: no unreachable objects. Not shallow. Clean, nothing untracked or ignored. |
| **B. Canonical Zaki** | `c8c68131b04e9d216673725075bef38df81e6041` = `master` = `origin/master`; tracked tree clean. Ignored artefacts exist. `build_keeper/libZaki.a` (2026-01-05) still references Python, so it is **not** a `c8c6813` build; it was used for nothing authoritative. |
| **C. Recovered snapshot** | 18 files. Regenerated manifest SHA-256 = **`ed76163c22e0a1f8ba3f71f62f5a56527528850650bbf7127da9c0c908d14083`**, byte-identical to the stored manifest (re-verified at the end); read-only permissions intact. ZIP `be41ee1c71b88627f0b11703c9d732abd7378ebbc162ee10649af771dec9ae0f`; `CONFIND_RECOVERED_SOURCE_AUTHORITY.txt` `c45ddcae7f79e14b2d21ed559a07398f86a25fb8126a0379f6f635323ece9d8e` (matches the transfer manifest); `TRANSFER_MANIFEST.txt` `639a60475fff176847d7a3f68297e911f5c8b9124475c9b81091d7f9b8562a4f`. The snapshot is a **composite**: `include/Confind/*` was copied from CompactStar's consumed-header root, everything else from the Confind implementation root. So "EXACT_HEADER_MATCH" holds **by construction**; it is not independent evidence. |
| **D. CompactStar** | `812463ac9ed374f64ac9cadd500066ab723d3a6c` = `master` = `origin/master`; tracked tree clean. arm64 `libConfind.a` `09ed1a7c43a83b42f64ee8e0bda3b879af970126ee75159a802179a4d0a49eb2` (703,208 B); x86_64 `libConfind.a` `869fd9232ed1e1c24d227a2282392d612d7c6a0a119263275d887fd229794520` (903,904 B). The 7 vendored headers are hash-identical to the recovered `include/`; the 6 `* 2.hpp` files are byte-identical Finder duplicates. Vendored arm64 `libZaki.a` `3dd4789a20c35064b3133bb863c54c4f64e7df31c94b83201d68f5463902dfef` (members dated 2026-01-05). Both CONFIND archives and the headers entered in `7fd0132` (2025-04-17); `dfb4443` only deleted `.DS_Store` files — the archive and header blobs are unchanged since `7fd0132`. |

## 2. Lineage table

| | **C1 — Git** | **C2 — Recovered** | **C3 — Vendored arm64** (C3′ = x86_64) |
|---|---|---|---|
| Dates | Code 2019-12-26 → 2020-04-09 | `Cell.cpp` 2020-05-14 … `Cont2D.cpp` 2023-09-21 15:11, `ContourFinder.cpp` 16:19; headers 2023-09-21 | Configured 2023-05-01 (banner), compiled 2023-09-16. C3′: 2023-09-21. |
| Files | `include/{Base,Cell,Common,ContourFinder}.h`, `src/*.cc` (`Cont2D` and `Bundle` live inside `ContourFinder.h`) | `include/Confind/*.hpp` (7), `source/*.cpp` (5) | Five objects `{Base,Cell,Cont2D,Common,ContourFinder}.cpp.o`; headers = C2 |
| Namespace | `CONFIND`, but `Base` is in the **global** namespace | `CONFIND` throughout | = C2 |
| Contour API | `SetGrid`, `SetFunc`, `SetMemFunc` (raw, non-owning), `SetContVal`, `SetGridVals(Mode = Optimal)`; public `Find*`; `ExportContour(string, char*)`. No `GetContourSet`, no `ConvertToCurve2D`, no sampled-grid entry point. | Adds `GetContourSet`, `SetGridVals(GridVals_2D*)`, `SetContVal(values, labels)`, `ConvertToCurve2D`, `ConvertToDataSet`, `GetVal`, `SetLabel`, `MemFuncContWrapper`. `SetGridVals` default becomes `Fast`; `Find*` become private; `SetMemFunc` **takes ownership** via `unique_ptr`. | C2 minus `ConvertToDataSet` |
| Zaki includes | `zaki/…/*.h` (lower case), vendored in the repository | `Zaki/…/*.hpp` | Same; built against an **older** diagnostics header generation (`LogEntry` without `Id`; `InstrumentationTimer(const std::string&)`) |
| Plotting | ROOT (`TMultiGraph`, `TLegend`, `TCanvas`, `TGraph`, `TStyle`, `TColor`) | ROOT commented out; `Plot` uses Zaki `DataSet` → matplotlib | `Plot` = **empty stub**. C3′ imports the DataSet plotting API. |
| ROOT / Python | ROOT required; no Python | ROOT none; CMake `Python3` + `NumPy` REQUIRED | 0 ROOT symbols, 0 Python symbols |
| OpenMP | Unconditional `omp.h`; Parallel and Ludicrous modes | `__has_include(<omp.h>)` guard (`Z_OMP`); CMake `OpenMP` REQUIRED | `omp_*` imports present |
| Logging / instrumentation | `Z_LOG_*`, `PROFILE_FUNCTION` | Adds object tracking (`Z_OBJ_*`), class-level `operator new`/`delete` using `Z_NEW`/`Z_DELETE`, atomic instance counter, `PtrStr` | = C2 |
| Algorithm | CONREC. Cell corners computed as `G+(i+1)·Δ`. `FindNextContours` starts at **k = 1**. `corners.reserve` followed by `[]` (UB). "Optimal" mode chosen by timing. | CONREC. Corners `(G+i·Δ)+Δ`. **k = 0**. `corners.resize`. `TimeFunc` always returns Ludicrous. `res_x = yAxis.res` defect. | = C2 (verified) |
| Export / de-duplication | `Export` writes points unsorted; de-duplication only inside `Plot(connected)` | `Export` and `ConvertToCurve2D` call `SortNew` (de-dup + sort) | = C2 |
| Build | Makefile, C++14, `clang++-mp-9.0` / `g++`, ROOT absolute paths | CMake, C++17 (gnu++17), vendored `libZaki.a`, config header written into the source tree | `-O0`, no DWARF, Apple clang (Xcode-14 era), minOS / SDK 13.0 |

## 3. Git history findings

- The Git history is complete and has **no bridge** to C2 or C3. `GetContourSet`,
  `ConvertToCurve2D`, `ConvertToDataSet`, `GridVals_2D`, `Curve2D` and `Confind::` never occur, and
  no revision has `Confind/*.hpp` or upper-case `Zaki/` includes. The 2020→2023 evolution is not in
  Git.
- Commit boundaries:

| Feature | First commit | Date |
|---|---|---|
| CONREC core (`EvalCenter`, `case36`, triangle status) | `1e1b1eb` | 2019-12-26 |
| OpenMP; `Orientation`; ROOT plotting | `2bbff01` | 2019-12-27 |
| `FindNextContours` | `28313fc` | 2019-12-27 |
| **`std::set<Coord3D>` / `RMDuplicates` / `SortNew`**, `Z_LOG_` | **`01d4363`** | **2020-04-06** |
| Ludicrous mode, `FindContourFast` | `cac5002` | 2020-04-09 |

- ROOT is never removed in Git.
- The Zaki `Coord3D` vendored in Git in 2020 already orders by XY radius, so radius
  de-duplication has been the behaviour since it was introduced.

## 4. Vendored binary

- **Members**: `__.SYMDEF`, `Base`, `Cell`, `Cont2D`, `Common`, `ContourFinder` `.cpp.o`, dated
  2023-09-16 11:33, uid/gid 501/20.
- **Embedded source paths**: `…GoogleDrive-mzake001@ucr.edu/My Drive/Work/Coding/Confind/src/*.cpp`
  and `…/Confind/dependencies/include/Zaki/File/VecSaver.hpp`.
- **Exports**: 173 demangled defined symbols mention `CONFIND::` in arm64 and 170 in x86_64. The x86_64
  count includes `Cont2D::ConvertToDataSet()`, which arm64 lacks; arm64 has 4 extra libc++ template
  instantiations.
- **Imports**:
  - Zaki: `Curve2D::Reserve`, `LogManager::Emit`, `MemManager::New` and `Delete`,
    `ObjManager::Emit`, `InstrumentationTimer::Stop`, `Banner`/`TextBox`, `Directory`,
    `Coord3D::Str`.
  - OpenMP: `omp_get_num_threads`, `omp_get_thread_num`, `omp_set_num_threads`,
    `.gomp_critical_user_.var`.
  - C runtime: `abs`, `log10`, `pow`, `mkdir`, `strerror`, and zlib `compress` (from Zaki's inlined
    `VecSaver`, which includes `zlib.h`), among others.
- **Absent entirely**: ROOT, Python, DataSet and DataColumn — 0 symbols of any of them.
- **Compiled in as weak definitions**:
  - `Coord3D::operator<`, `operator==` and `XYDist2`, all unfused (no FMA);
  - `InstrumentationTimer(const std::string&)` constructor and destructor (old layout);
  - `LogEntry` 5-argument constructor (old layout, no `Id`).
- **`Plot`**: 8 instructions; stores its arguments and returns.
- **Toolchain**: Mach-O arm64, minOS 13.0, SDK 13.0, `-O0` (every value spilled), no DWARF.
- **Closest honest identity:** the recovered snapshot minus `Cont2D::ConvertToDataSet` and minus the
  body of `Plot`, compiled at `-O0` with Xcode-14-era Apple clang against an older Zaki diagnostics
  generation. This is **structural equivalence, not byte identity**.
  - Across all CONFIND functions the only real differences are `Plot` and the missing
    `ConvertToDataSet`.
  - Residual differences are four codegen artefacts: FMA (disappears with `-ffp-contract=off`),
    switch lowering, duplicated exception-cleanup code, and libc++ template spellings.
  - For the 22 functions on CompactStar's path, field offsets and immediates are identical in 19;
    the other 3 differ only in codegen (jump table vs compare chain, inlined `abs`, order of literal
    loads).
- **Parallel-mode checks**: the OpenMP outlined bodies match on FP and branch sequences. The
  following were verified directly in the disassembly: `res_x` loads from `#0xe8`;
  `FindNextContours` starts at `k = 0`; `SetGridVals(Mode)` allocates with `operator new[]` and frees
  with non-array `operator delete`; `ThreadTaskLudicrous` sizes `corners` from `xAxis.res`
  (`#0xb8`).

## 5. Source-basis recommendation

| Candidate | Historical coherence | Closeness to vendored | Maintainability | Legacy load | Zaki 2.0 | Risk to semantics |
|---|---|---|---|---|---|---|
| A. Modernize Git HEAD | High within Git | **Low** (different corner arithmetic, k = 1, no sampled-grid path, no curves) | Low | ROOT, vendored binaries | Many adaptations | **High**; all 2020–23 changes would have to be re-derived |
| B. Promote recovered as-is | Medium | High, **except** `Plot`'s in-place sort, which changes `FindCriticalCurve` | Medium | DataSet plotting, Python | **12 compile errors** against `c8c6813`: 6 private `DataColumn` accesses + 6 removed DataSet plotting APIs, all inside `ConvertToDataSet` and `Plot` | Medium |
| C. Git plus ported changes | Medium | Ends at D with more steps | Low | High | — | High |
| D. Recovered minus plotting, adapted | Good | High | Good | Low | Clean | Low |
| **E. (recommended; a precise form of D)** Import C2 byte-exact as a commit on a new branch of this repository (Git history kept as ancestry). Then **one reconstruction commit**: delete `ConvertToDataSet` and empty or remove `Plot`. | **Best** (C1 → C2 → C3 recorded explicitly) | **Proven structural identity** with C3 | Good | Minimal | **0 errors, 0 warnings** against `c8c6813` at default warning flags (probe) | **Lowest** |

## 6. Current Git architecture (C1)

| Unit | Responsibility / input → output | Ownership, mutation, errors | Threads | Zaki | Plotting | Used by CompactStar |
|---|---|---|---|---|---|---|
| `Base` (global namespace) | Name, work directory (`mkdir`), banner on first object (clears the screen) | Non-atomic static counter; `sprintf` into `tmp[75]` can overflow | Unsafe counter | Logger, Banner | Banner | No |
| `Common` | `Color::name`, ROOT colour map | — | — | — | **ROOT** | No |
| `Cell` + `vertex` / `triangle` | Per-cell CONREC: 5 vertices, 4 triangles → pairs of contour points | Non-owning `Bundle*`; log-and-sentinel errors; `operator[]` flag check inverted | One cell per loop iteration | `Coord3D`, Logger, profiling | — | No |
| `Cont2D` | Level plus flat point list; `AddPts`, `RMDuplicates`, `SortNew`, dead `Sort`, `Export` (unsorted) | Mutating | — | `Coord3D`, SaveVec, `Exists` | Colour | No |
| `ContourFinder` | Grid, levels, function; 5 modes; export; ROOT `Plot` | Owns raw `TMultiGraph`/`TLegend`; `delete` on `new[]`; copy constructor dereferences a null `genFuncPtr`; `SetScanMode` always rejects | OpenMP modes | Grid2D, Func2D, MemFuncWrapper | **ROOT** | No (Git lacks CompactStar's API) |
| `Bundle` | Per-thread copies of grid, contour and function pointers | Non-owning | Per thread | Grid2D, Func2D | — | No |

## 7. Recovered architecture (C2) and its differences from C1

The same classes, split into `Bundle.hpp` and `Cont2D.hpp`. `Base` gains object tracking,
`operator new`/`delete` accounting and an atomic counter. `Cell` is reused via `SetIdx`, and there
is a sampled-grid path.

| Class of difference | Items |
|---|---|
| NUMERICAL_CONTOUR_ALGORITHM | Corner coordinates `(G+i·Δ)+Δ` instead of `G+(i+1)·Δ` (ulp-level); `FindNextContours` k = 1 → 0; `reserve` → `resize` (removes UB); `TimeFunc` fixed to Ludicrous; `res_x = yAxis.res` defect; Parallel mode copies the whole contour into the bundle; unsigned `unfound_contours`; Ludicrous now decrements only unfound contours |
| API_ADDITION | `GetContourSet`, `SetGridVals(GridVals_2D*)`, `SetContVal(values, labels)`, `ConvertToCurve2D`, `GetVal`, `SetLabel`, `MemFuncContWrapper`, `Bundle` constructor 0 and `Add*`, `Cell::operator=` deleted |
| API_RENAME / CHANGE | `SetWrkDir(Directory)`, `SetName(string)`, `ExportContour(Directory, FileMode)`; `SetGridVals` default `Optimal` → `Fast`; `Find*` made private; `SetMemFunc` now **owning**; `SortNew()` loses its argument; `Color` index made unsigned |
| ZAKI_ADAPTATION | `zaki/*.h` → `Zaki/*.hpp`; `Directory`, `FileMode`, `VecSaver`, `Curve2D`; `unique_ptr` from `Clone()` |
| PLOTTING_ONLY | ROOT removed; DataSet `Plot`; plot X/Y range setters |
| EXPORT_ONLY | `Export` → TSV via `VecSaver`, now sorting first; label-based file names; `ConvertToDataSet` (used only by `Plot`) |
| LOGGING / INSTRUMENTATION | Object tracking, class-level new/delete, `PtrStr`, `Z_LOG_NOTE`, `sprintf` → `snprintf` |
| BUILD_ONLY | CMake, config header, OpenMP / GSL / ZLIB / Python requirements |
| UNKNOWN | None material |

## 8. Numerical-core flow (CompactStar's sampled-grid path; serial)

```
SetGrid(Grid2D) ─► Δx = (xmax−xmin)/nx          [log axis: (log10 xmax − log10 xmin)/nx]
SetContVal(L)   ─► cont_set += Cont2D(level) for each level; unfound += |L|
SetGridVals(&gv) ─► FindNextContours(gv.m_GridValArr)          (no dimension or null check)
  for k = 0..|L|−1, skipping levels already found:
    Bundle b(grid, copy of cont_set[k]);  one Cell object reused for every cell
    for j = 0..ny−1 { for i = 0..nx−1 {                         (j outer, i inner; scan mode ignored)
      SetIdx(i,j): x0 = G_x + i·Δx, y0 = G_y + j·Δy; reset flags
      z: BL = v[i+(nx+1)j]   BR = v[i+1+(nx+1)j]   TR = v[i+1+(nx+1)(j+1)]   TL = v[i+(nx+1)(j+1)]
      corners: BL(x0,y0)  BR(x0+Δx,y0)  TR(x0+Δx,y0+Δy)  TL(x0,y0+Δy)
      centre = ((BL.x+BR.x)/2, (BL.y+TL.y)/2, (zBL+zBR+zTR+zTL)/4)
      triangles: T0(c,BL,BR) T1(c,BR,TR) T2(c,TR,TL) T3(c,TL,BL)
      if all 5 vertices strictly below, or all strictly above: nothing
      else per triangle, the CONREC case appends 2 points [log axes: x,y ← 10^x, 10^y]
      cont_set[k].AddPts(cell points)       → flat list; segments are implicit consecutive pairs
    }}  SetFound()
GetContourSet() ─► copy of raw points (cell-scan order, duplicates kept)   ◄ FindCriticalCurve uses this
ConvertToCurve2D() / Export() ─► SortNew():
   RMDuplicates: std::set<Coord3D> keyed on r² = x²+y²; first-inserted point of each r² class kept
   start = smallest x (tie → largest y)
   chain: repeatedly take the nearest point by XYDist2 (strict <; first in set order wins)
          among points not already in the output (by exact x,y,z ==)
```

- Exact floating-point branch decisions: vertex status `z > L` / `z < L`, the all-5 short-circuit,
  the triangle case, the r² ordering, `==` in `Exists`, and the tie-breaks.
- No parallel region is on this path.

## 9. Topology and ambiguous cells

**The algorithm is not marching squares.** It is CONREC: each rectangle is split into 4 triangles
around a synthetic centre whose value is the **mean of the 4 corners**.

Per triangle, the vertex statuses select one of ten cases:

| Case | Vertex statuses | Output |
|---|---|---|
| (c) / (f) | two on one side, one on the other | one segment across two edges |
| (d) / (h) | two on the level, one off | the edge between the two on-level vertices |
| (e) | one on, one above, one below | segment from the on-level vertex to the opposite edge |
| (a) / (j) | all below / all above | nothing |
| (b) / (i) | one on the level, two on the same side (a touch) | nothing |
| (g) | all three on the level | nothing — a triangle lying exactly at the level is dropped |

For the 16 corner sign patterns (no corner exactly on the level):

| Pattern | Behaviour |
|---|---|
| 0000, 1111 | No contour |
| One odd corner (8 patterns) | One arc cutting off the odd corner. Topology is fixed; the **geometry depends on the centre**: 3 points if the centre sides with the majority, 5 points bending around the centre if it sides with the minority. |
| Adjacent pair (4 patterns) | One crossing chain; which centre-edges it crosses depends on the centre |
| **Saddles 0101 / 1010** | **Decided by the mean.** Centre above the level → the two above-corners are joined and the two below-corners are cut off; centre below → the reverse; centre exactly on the level → an X through the centre. |

- Oracle checks, ±1 checkerboard (BL = TR = +1, BR = TL = −1):
  - level 0 → an X: centre (0.5, 0.5) plus the four edge midpoints;
  - level +0.5 (centre below) → arcs cutting off BL and TR: (0.25, 0), (0.25, 0.25), (0, 0.25) and
    (0.75, 1), (0.75, 0.75), (1, 0.75);
  - level −0.5 (centre above) → arcs cutting off BR and TL: (0.75, 0), (0.75, 0.25), (1, 0.25) and
    (0, 0.75), (0.25, 0.75), (0.25, 1).
- This is the mean-value decider, **not** the bilinear asymptotic decider, so it can differ from
  bilinear topology for asymmetric saddles.
- Corners exactly on the level produce vertices on the contour (fixture F12: a line along the grid,
  5 distinct points).
- NaN corners are treated as on the level (fixture F18).

## 10. Numerical tolerances

**There are none in the numerical core.** Every decision is exact.

| Item | Formula | Class |
|---|---|---|
| Level classification | `z > L`, `z < L`, otherwise "on" (including NaN) | Algorithm-defining |
| Centre value | `(a+b+c+d)/4`, fixed order | Algorithm-defining (saddle decider) |
| Edge interpolation | `r = (L−z1)/(z2−z1)`; `x = r·(x2−x1)+x1`. No division by zero: the endpoints have strictly opposite status. | Algorithm-defining; sensitive to FMA |
| Cell origin | `G + i·Δ`, then `+Δ` | Algorithm-defining; sensitive to FMA |
| Point identity (`std::set`) | Equal `fl(fl(x²)+fl(y²))` | Algorithm-defining (output) |
| Point identity (`Exists`) | Exact x, y, z | Algorithm-defining |
| Nearest-neighbour order | Strict `<`; start point = minimum x, tie → maximum y | Algorithm-defining |
| Log axes | `log10`, `pow(10,·)` | Algorithm-defining; depends on the platform maths library |
| `Orientation` `5·Δx·Δx`, `test_pt = (2,2,0)`, `dis = 6` | — | **Dead code** (`Sort()` is never called) |
| "Optimal"-mode timing 2e-2 / 4e-2 ms, 50 trials | — | Mode selection (Git only); commented out in C2/C3 |
| Segment joining | None — segments are never joined | — |
| Termination | `SortNew` always adds one point per iteration | Robustness |

## 11. `Coord3D` and de-duplication

`Coord3D` is byte-identical across canonical Zaki, CompactStar's vendored Zaki and the 2020 Git
copy, and matches what is compiled into C3:
- `==` compares x, y and z exactly;
- `<` is `XYDist2(origin) < rhs.XYDist2(origin)`, with `XYDist2 = (Δx)² + (Δy)²` (z ignored).

Nothing else in CompactStar's link defines these symbols, so libConfind's unfused copies are the
ones that run. They are used by `RMDuplicates` (the `std::set`), `SortNew` (`Exists`, which relies
on `==`, and the nearest-neighbour distances) and, transitively, by `ConvertToCurve2D` and `Export`.

1. **Does it collapse distinct points?** Yes. Two points are treated as equal whenever their
   computed r² is exactly equal.
2. **In what geometry?** Points related by an x↔y swap or a sign reflection about the **origin**,
   plus ulp-level twins whose r² rounds to the same value. Measured on the oracle:

   | Fixture | Distinct → kept | Distinct points lost | ulp twins lost |
   |---|---|---|---|
   | Origin circle r² = 1.7 | 56 → 8 | 48 | 0 |
   | Origin circle r² = 1 | 32 → 5 | 27 | 0 |
   | `x + y = 2.5` | 11 → 6 | 5 | 0 |
   | Saddle, level +0.1 | 22 → 5 | 17 | 0 |
   | Two circles symmetric about the origin | 32 → 9 | 23 | 0 |
   | Off-centre circle | 57 → 49 | 0 | 8 |
   | Nearby branches | 20 → 14 | 0 | 6 |
   | **CompactStar scale, level 0.3** | **370 → 368** | **0** | **2** |
   | **CompactStar scale, level 0.6** | **307 → 300** | **0** | **7** |

   At CompactStar scale the collapsed pairs are at most 0.03–0.125 apart at coordinates of ~3·10¹⁵.
3. **Was it intended?** Most likely as duplicate removal only; the geometry loss looks accidental.
4. **Is it historical?** Yes, since 2020-04-06.
5. **Does the vendored binary use the same comparator?** Yes, confirmed by disassembly.
6. **Could changing it alter CompactStar results?** Yes. It changes which points survive, and
   therefore:
   - `mass_curve[0]`, the curve's start point;
   - the intersections and the `Bisect` index;
   - the exported B_tot contours, which feed `Solve_Mixed` TOV solves.
7. **Is the de-duplication numerically load-bearing?** Yes for `ConvertToCurve2D` and
   `ExportContour`. Not for `FindCriticalCurve` under vendored behaviour, which sees raw points.
8. **Distinguishing fixture** (fixture C13):
   - (a) an origin-symmetric circle — radius ordering collapses, lexicographic ordering keeps all,
     tolerance identity keeps all;
   - (b) synthetic ulp twins fed through `AddPts` — radius ordering collapses them only if r² is
     equal, lexicographic keeps both, tolerance identity merges them;
   - (c) exact duplicates — all three merge.

**Owner options:**
- **O1 — keep it frozen and characterized (recommended now).**
- O2 — a CONFIND-owned lexicographic or exact comparator.
- O3 — a tolerance-based identity.
- O4 — replace the de-duplication and nearest-neighbour chaining with real segment joining.

O2–O4 are Class-C changes that need CompactStar re-qualification. Do not change Zaki's
`operator<`; CONFIND should stop relying on it only after adjudication.

## 12. OpenMP

**Where it is used:** `FindContourLudicrous` and `FindContourParallel`, each with a `single`
region, several unnamed `critical` sections (copying contours, reading the grid, cloning the
function, merging results) and per-thread `Bundle`s. Both also call `omp_set_num_threads`, which
changes the setting globally for the rest of the calling process.

**Defects:**
- The y-range is split linearly, even for log axes.
- Each thread gets `res_y = ny/n_t + 1` rows, so rows overlap and the last thread evaluates one row
  **beyond y_max**.
- `res_x = yAxis.res`: the contour is truncated when ny < nx, and the `corners` buffer overflows
  when ny > nx.

**Output ordering:** the merge order is the order in which threads finish. Point counts grow with
the thread count (204 → 218 → 256). Raw order differed between two runs for Parallel mode at t = 2
and t = 4; Ludicrous merges the same way and is nondeterministic by construction (its two observed
runs happened to agree). The curve after `ConvertToCurve2D` was identical in the tested fixture,
but that is not guaranteed.

**Thread safety:**
- Logger, ObjManager and MemManager use mutexes; they are safe but serialize every tracked `Cell`
  event.
- `MemFuncWrapper` keeps a **static registry with no lock**; destroying per-thread bundles
  concurrently is a data race.

**Hot-path overhead:** profiling timers and object tracking run on every cell.

**CompactStar uses none of this.** **Recommendation: remove OpenMP from the maintained core, and
revisit only after a serial reference is characterized.**

## 13. ROOT

- **C1**: plotting only. Affected files: `Common.h/.cc` (`TColor`, `RColorMap`),
  `ContourFinder.h/.cc` (`TMultiGraph`, `TLegend`, `TCanvas`, `TGraph`, `TStyle`, `TAxis`;
  constructor/destructor, `Plot`, `GetGraph`, `GetLegend`, `MakeLegend`), and the Makefile's ROOT
  flags.
- **C2**: commented out.
- **C3**: 0 symbols.
- **ROOT can be removed completely**; in the recommended basis it is already gone apart from
  comments.

## 14. Plotting-removal inventory

| Item | Lineages | Classification |
|---|---|---|
| `ContourFinder::Plot` | C1 ROOT; C2 DataSet; C3 empty | **REMOVE** (deleting it equals C3 behaviour) |
| `SetPlotXLabel/YLabel/Label/Connected`, `SetLegendLabels`, `MakeLegend` (calls `strcmp(nullptr)` by default), `SetPlotX/YRange`, `SetWidth/Height`, `GetGraph/GetLegend`, plot members | All | REMOVE |
| `Cont2D::ConvertToDataSet` | C2 | REMOVE (absent from C3) |
| `Color`, `SetColor/GetColor`, `RColorMap` | All | REMOVE |
| `Base::ShowBanner` (clear-screen escape codes) | All | NOT_ACTUALLY_PLOTTING, but presentation → REMOVE |
| `Sort`, `Orientation`, `comp_Orient` | All | REMOVE (dead) |
| `RMDuplicates`, `SortNew`, `ConvertToCurve2D` | All | **NUMERICAL_DATA_EXPORT_MUST_REMAIN** |
| `ExportContour` / `Cont2D::Export` | All | NUMERICAL_DATA_EXPORT_MUST_REMAIN (CompactStar calls it once) |
| `operator<<`, `Print` | All | NOT_ACTUALLY_PLOTTING (optional) |
| `DataSet::Plot`/`LogLogPlot`/`SemiLogX/Y`, `PlotParam`, matplotlib, Python in CMake | C2 | REMOVE |
| gnuplot | — | None found |
| `Curve2D::Plot` | CompactStar side | Already removed in Zaki 2.0; `TaskManager` must drop its calls |

## 15. Compatibility with ZakiLib 2.0 (`c8c6813`)

Two pieces of evidence:
- The reconstructed C3-equivalent source compiles with **0 errors and 0 warnings** (default warning
  flags) against `c8c6813` headers.
- All 24 out-of-line Zaki symbols it references are defined in `c8c6813` sources
  (`ObjCopyConstruct`'s key function is out-of-line in `ObjObserver.cpp`).

| Symbol | Classification |
|---|---|
| `Grid2D`, `Axis`, `Range`, `Coord2D`, `Curve2D` (`Reserve`, `Append`, `pts`), `Func2D` (`Clone` → `unique_ptr`, `Eval`), `MemFuncWrapper`, `VecSaver::Export1D`, `Directory`, `FileMode`, `Exists` | UNCHANGED_PUBLIC_API |
| `GridVals_2D` | UNCHANGED (public raw member `m_GridValArr`; an accessor is advisable later) |
| `Coord3D` | UNCHANGED_PUBLIC_API **and** SCIENTIFIC_SEMANTIC_CONFLICT (`<` is inconsistent with `==`) — frozen |
| `DataSet`, `DataColumn` | MECHANICAL (only reached through code being deleted) |
| Logger, Instrumentor, ObjObserver, MemoryManager, Banner/TextBox | Macro and class APIs present in `c8c6813`; class layouts differ from the generation C3 was built against; **remove from CONFIND** |
| GSL headers | Included by CONFIND but never used → drop; GSL arrives through `Zaki::Zaki` |
| MISSING_PUBLIC_API | **None** |

## 16. DataColumn migration

The six accesses were confirmed by compile errors against the current `DataColumn`:

| Location (recovered) | Code | Public replacement |
|---|---|---|
| `Cont2D.cpp:109` | `tmp_ds[0].label = "X"` | `SetLabel("X")` |
| `Cont2D.cpp:110` | `tmp_ds[1].label = "Y"` | `SetLabel("Y")` |
| `Cont2D.cpp:114` | `tmp_ds[0].vals.emplace_back(x)` | `PushBack(x)` |
| `Cont2D.cpp:115` | `tmp_ds[1].vals.emplace_back(y)` | `PushBack(y)` |
| `ContourFinder.cpp:1381` | `plt_ds[2*i].label = …` | `SetLabel(…)` |
| `ContourFinder.cpp:1386` | `plt_ds[2*i+1].label = …` | `SetLabel(…)` |

- `PushBack` is `emplace_back`, so the result is bitwise the same.
- With these six substitutions the source compiles against CompactStar's vendored Zaki (whose
  `DataColumn.hpp` is byte-identical to canonical).
- `DataColumn(label, vector)` is a cleaner alternative.
- **Preferred: delete both functions.** C1 has zero accesses, and no legitimate CONFIND operation
  needs new Zaki access.

## 17. The `GridVals_2D` boundary

- **Read-only, non-owning**; the pointer is not kept after the call.
- **Indexing:** `v(i,j) = m_GridValArr[i + (nx+1)·j]` (x fastest), identical to both Zaki
  constructors.
- **Nx and Ny come from CONFIND's own grid**, not from `n_x`/`n_y`. There is no dimension check (a
  mismatch reads out of bounds) and no null check (a moved-from object is dereferenced).
- **Node-coordinate rounding differs:** `Axis::operator[]` computes `min + i·(max−min)/res`, while
  CONFIND computes `min + i·((max−min)/res)`. The two differ at the ulp level.
- **Missing samples** become `def_val = −1` and are treated as real data (fixture F14).
- **Future accessor contract** (Zaki; not now): `Nx()`, `Ny()`, `const double* Values() const`,
  `At(i,j)`. CONFIND 2.0 can validate `Nx == nx`, `Ny == ny` and non-null.
- **Modernization can proceed** on the raw member, which is still public in `c8c6813`.

## 18. Ownership and memory safety

**Genuine undefined behaviour:**
1. `delete m_GridValArr` on memory from `new[]` (C1 `ContourFinder.cc:506`, C2 `ContourFinder.cpp:306`,
   and C3, verified in the binary). The oracle executed it without visible failure.
2. `SortNew` reads `pts[0]` on an **empty found contour** — i.e. `ConvertToCurve2D`/`Export` when a
   level has no crossing (reachable from CompactStar).
3. C1 `corners.reserve` followed by `[]` (fixed in C2).
4. Ludicrous overflows `corners` when ny > nx (C2/C3; verified in the binary).
5. `comp_Orient` is not a strict weak ordering (dead code).
6. C1 copy constructor dereferences a null `genFuncPtr`, and the clone leaks.
7. C2 `SetMemFunc` takes ownership of a raw pointer: passing a stack object (the pattern in Git
   example 3) destroys it twice.
8. `MemFuncWrapper` registry data race (OpenMP).
9. **ODR/layout mismatches in the vendored archive pairing** (`LogEntry`, `InstrumentationTimer`).
10. C1 `sprintf` overflows (`tmp[75]` with a path).
11. `strcmp(nullptr)` in `MakeLegend`.
12. C1 variable-length array.

**Defect, not UB:** C2 `Export` silently truncates paths longer than 150 characters, so it writes to
the wrong file.

**Style / performance:**
- global-mutex object tracking on every `Cell`;
- `Cont2D` copies via `*this = other` with no move constructor, so `GetContourSet` copies everything;
- `Cell::operator[]` flag check is inverted, and `GetIdx` returns `{-1,-1}` in a `size_t` pair.

**Not problems:** `Cell`'s raw `Bundle*` (the bundle outlives the cell); the `GridVals` pointer
(not retained).

## 19. Error handling

- CONFIND has no exceptions, asserts or `exit`. It logs `Z_LOG_ERROR` and then returns a sentinel
  (`0`, `-1`, `{-1,-1}`, `SIZE_MAX`) or just continues.
- Specific cases:
  - `EvalFunc` returns **−1 as a real sample** on an invalid axis scale.
  - `operator+=` merges contours of different levels anyway.
  - `SetScanMode` always rejects its argument.
- Zaki: the first `GridVals_2D` constructor calls `std::exit`; `Axis::operator[]` uses `assert`.
- **With the vendored archive pairing, every CONFIND error is silent**: its log entries are dropped
  (summary finding 6).

**Recommended policy:**
- The core never logs or exits.
- Precondition violations throw `std::invalid_argument` at the API boundary before any computation:
  no grid, `res == 0`, non-finite bounds, a log axis with bound ≤ 0, a size or null mismatch, no
  levels.
- An empty result is not an error: `ConvertToCurve2D` returns an empty curve (UB removal).
- The NaN policy is Class-C.

## 20. Build system

- **C1**: see §2. It also commits binaries, objects and `.DS_Store` files.
- **C2**:
  - CMake 3.10 with ZLIB, GSL, Python3/NumPy and OpenMP all REQUIRED; vendored `libZaki.a` path;
    `CMAKE_CXX_FLAGS` modified globally.
  - `configure_file` writes the config header **into the source tree with a timestamp**
    (non-reproducible; it explains the 05-01 vs 09-21 mismatch).
  - It installs its vendored dependencies.
  - **Not buildable as preserved.** The snapshot is a logical re-layout (its three
    `build/*CMakeLists.txt` files came from the root, `include/` and `src/` of the original tree;
    the binaries' embedded paths confirm the original `src/`), and `Examples/`, the doxygen tree and
    the vendored `dependencies/` were not preserved.
- **C3**: `-O0`, no `-g`, OpenMP on.
- **CompactStar**:
  - forces `/usr/bin/clang++`; defaults to Debug; C++17 without extensions;
  - Python3/NumPy, GSL and OpenMP REQUIRED;
  - links `${ZAKI_LIB}` **before** `${CONFIND_LIB}`.

**Target design:**

```cmake
cmake_minimum_required(VERSION 3.20)
project(CONFIND VERSION 2.0.0 LANGUAGES CXX)
find_package(Zaki 2.0 CONFIG REQUIRED)        # GSL, ZLIB come transitively
add_library(Confind STATIC src/Cell.cpp src/Cont2D.cpp src/ContourFinder.cpp)
add_library(CONFIND::CONFIND ALIAS Confind)
target_compile_features(Confind PUBLIC cxx_std_17)
set_target_properties(Confind PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(Confind PUBLIC Zaki::Zaki)
target_compile_options(Confind PRIVATE $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-ffp-contract=off>)  # stage C0 decision
# generated/Confind/Version.hpp in the binary directory (no timestamps); tests behind CONFIND_BUILD_TESTS;
# install(EXPORT … NAMESPACE CONFIND::) with a CONFINDConfig.cmake that does find_dependency(Zaki 2.0)
```

No OpenMP, GSL, Python or ROOT, no vendored Zaki, no compiler forcing, no absolute paths.

## 21. Portability

| Target | Issues |
|---|---|
| **All** | FMA policy: Apple clang fuses at `-O0`; GCC's GNU dialect defaults to `-ffp-contract=fast`. Hardened standard libraries will trap the existing UB. |
| **Linux** | Likely missing direct includes, inferred from reading (not compiled on Linux): `<cstring>`, `<cerrno>`, `<sstream>`, `<atomic>`, `<chrono>`, `<iterator>`, `<cmath>`, `<cstdlib>`. `ACCESSPERMS` + `mkdir` in `SetWrkDir` (not POSIX). **GNU ld link order**: CompactStar lists Zaki before CONFIND; ld64 tolerates it, GNU ld would leave libConfind's Zaki references undefined — fixed by target-based linking. |
| **macOS arm64 / AppleClang** | Compiles cleanly today (probe) |
| **Linux GCC / Clang** | Not tested (out of scope) |
| **Removed with the modernization** | C1 variable-length array and absolute paths; `-std=gnu++17`; `libm` differences for log axes |

## 22. Current tests and examples

- **C1**: 6 examples. `0_Function` (demo + ROOT plot + export), `1_MemFunction`,
  `2_MemFunctionOptimized`, `3_Modify_Object` (timing benchmark), `4_Parallel` (timing benchmark),
  `omp_prototype` (a stub). None has an assertion, so there are **0 tests**. The committed `.dat`
  files are weak C1-era reference data only.
- **C2**: none. The CTest block is commented-out tutorial boilerplate.
- **C3**: none.
- **CompactStar**: `TaskManager.cpp` is compiled, but **no tracked test or executable calls it**.
- **Coverage is effectively zero** for every item in §23.

## 23. Proposed characterization suite

**Common rules:**
- Public API only.
- The serial path is built with `-ffp-contract=off` and compared **bitwise** (hex-float).
- Record three things for every fixture:
  - the raw `GetContourSet` sequence (order and multiplicity);
  - the `ConvertToCurve2D` sequence;
  - counts: raw / exact-distinct / r²-classes, plus the `found` flags.
- Use dyadic grids so node coordinates are exact.
- The oracle values below are scratch observations from the vendored binary, to be regenerated
  under governance.

| ID | Input / grid / level | Expected (oracle: raw / distinct / curve) | Detects |
|---|---|---|---|
| C1 | constant 1, [0,4]², 4×4, levels {0.5, 1, 2} | 0 / 0 / –; `found` = true; historically `ConvertToCurve2D` = UB → new: empty | Spurious points; empty handling |
| C2 | z = x, level 1.25 | 24 / 13 / 13; one open line x = 1.25 bitwise from (1.25, 0) to (1.25, 4) | Interpolation, orientation |
| C3 | z = x, level 1.5 (cell centres on the level) | 16 / 9 / 9 | Cases (b), (e), (i) |
| C4 | z = y, level 2.25 | 24 / 13 / 13 | x/y swaps, triangle order |
| C5 | z = x + y, level 2.5; level 3 (through vertices) | 20 / 11 / **6**; 24 / 7 / **4** | Vertex on level; r² collapse |
| C6 | z = x² + y², [−2,2]², 8×8, levels 1.7 / 1.0; off-centre control | 96 / 56 / **8**; 48 / 32 / **5**; off-centre 96 / 57 / 49 | Closed curve, collapse, twins |
| C7 | two circles, 12×6, level 0.5 | 64 / 32 / 9 (single nearest-neighbour chain across both components) | Chain jumps; component merging |
| C8 | z = x, level 0 and level 4 (boundaries) | 8 / 5 / 5 each | Boundary coincidence |
| C9 | z = x + y, level 3 | See C5 | Vertex crossing |
| C10 | z = x, level 2 (interior grid line) | 16 / 5 / 5 | Case (d)/(h) edges; (g) dropped |
| C11 | z = x·y, n = 3 (centre cell) at levels 0 / ±0.1; vertex saddle; checkerboard at 0 / ±0.5 | 24/13/4; 32/22/5 (both); 32/9/3; 8/5/3, 8/6/4, 8/6/3 | **Mean-decider tie-break** |
| C12 | (x−2)², 16×2, level 0.01 | 24 / 20 / 14 (6 twins) | Nearby branches |
| C13 | C6 origin circle plus synthetic twin/duplicate `AddPts` sets | See §11 | Radius vs lexicographic vs tolerance ordering |
| C14 | Reversed axis (min > max) | Record current behaviour; flag as unsupported input for future validation | Axis handling |
| C15 | Sampled vs callable (Fast, Normal) on dyadic grids | **Bitwise equal** raw and curve hashes | Input-path equivalence |
| C16 | Parallel / Ludicrous, t = 1, 2, 4 | t = 1 bitwise equal to serial; t ≥ 2 raw counts and order vary; `res_x` truncation (60 vs 156) | Only if OpenMP is kept |
| C17 | NaN sample | 32 / 13 / 12; curve ends with (2,2)×8 | NaN policy |
| C18 | Missing sample (`def_val`) | 32 / 17 / 17 | Sentinel handling |
| C19 | CompactStar-scale 50×50, levels 0.3 / 0.6 / 0.9 | 728/370/368, 596/307/300, 272/137/137 | Realistic twins |
| C20 | `Plot()` / `GetContourSet` raw-layout invariance | Raw sequence unchanged | Plot side effect must not return |
| C21 | Multi-level order and `found` flags | — | Level loop |
| C22 | **Log axes** (x and/or y) | New — not yet run | `pow`/`log10` path (CompactStar's `d_ax` may be Log) |
| C23 | CompactStar consumer fixtures: critical-curve maximum over all raw points but the last; `mass_curve[0]` start rule; `Bisect` index | New — run at stage C7 | CompactStar semantics |

## 24. Can the vendored binary serve as an oracle?

**Yes — on Darwin arm64, for the serial paths — and it was demonstrated.**

- The scratch harness (Appendix C) uses only the vendored headers and links vendored arm64
  `libConfind.a` + `libZaki.a` (the exact pairing CompactStar links), plus GSL, zlib, libomp and
  Python 3.12.
- Serial output was bitwise identical across runs.

**Caveats:**
- The diagnostic-type UB described in §18 is present, but none was observed to affect numerics.
- Avoid UB inputs: an empty contour passed to `ConvertToCurve2D`, and Ludicrous with ny > nx.
- The harness must not emit competing weak `Coord3D` comparators (Appendix C's harness doesn't, and
  is compiled with `-ffp-contract=off`).
- OpenMP output is nondeterministic.
- The x86_64 archive was not executed.

**The proposed authority order is coherent:**
1. vendored binary behaviour through the authenticated headers;
2. the reconstructed source, proven structurally identical;
3. recovered-only additions (`ConvertToDataSet`, DataSet `Plot`), which are deleted anyway;
4. Git behaviour, only where nothing else exists — it is not CompactStar-relevant.

- Every C3-callable fixture can run against the vendored binary.
- The reconstructed source can run all fixtures.
- The recovered source as-is can too, except that its `Plot` differs.

Scratch evidence identities and reproduction commands are in Appendix A.

## 25. Numerical authority surface

| Class | APIs |
|---|---|
| **NUMERICAL_AUTHORITY** | `SetGrid`, `SetContVal`, `SetGridVals(GridVals_2D*)`, `SetFunc`/`SetMemFunc` + serial `SetGridVals(Normal/Fast)`, the internal CONREC `Cell`, `GetContourSet` (raw), `Cont2D::size`/`[]`/`GetVal`/`GetFound`, **`ConvertToCurve2D`** (with `SortNew`/`RMDuplicates`) |
| CONVENIENCE_ADAPTER | `MemFuncContWrapper`, `SetContVal(values, labels)`, `SetLabel`, `GetN_X` / `GetX_Min` / `GetDeltas` / `ij_2_xy` / `Get*Scale`, `AddPts`, `+`, `+=` |
| EXPORT | `ExportContour`, `Cont2D::Export`, `operator<<`, `Print`, `ConvertToDataSet` (C2) |
| VISUALIZATION | `Plot` and all plot setters, `Color`, legend, width/height |
| DIAGNOSTIC | Banner, `SetName`/`GetName`/`PtrStr`, `Z_LOG`, `PROFILE`, `Z_OBJ`, `Z_NEW`, class-level new/delete, `SetWrkDir` (keep only as an export root) |
| LEGACY / DEAD | `Sort`/`Orientation`/`comp_Orient`, `SetScanMode` and the Y-scan loop, `Optimal`/`TimeFunc`/RNG/`SetOptimizationTrials`, Parallel/Ludicrous/`SetThreads` |

## 26. CompactStar usage map (`CompactStar/Core/src/TaskManager.cpp`, the only consumer)

| Site | Calls | Output → effect on science | Status |
|---|---|---|---|
| `FindCriticalCurve` (L280–352) | `SetGrid`, `SetWrkDir`, `SetContVal` (~92 levels), `SetGridVals(&B_vis_grid)`, `SetPlotConnected`, **`Plot`**, `GetContourSet` | Maximum of `M_tot_grid.Evaluate` over raw points `p < size−1` (L374) → critical curve → smoothed and exported. **Scientific.** Under C3, raw points are unsorted. `size()−1` underflows on an empty contour. | Production numerics, not exercised by tracked code |
| `FindMtotContour` (L492–537) | Same setup, `GetContourSet`, **`ConvertToCurve2D`**, `Plot`, `ExportContour` | `mass_curve` → intersection; **`mass_curve[0]`** sets the B_tot level range. **Scientific.** | Production |
| `FindBtotContour` (L565–628) | Same, `ConvertToCurve2D` per level, `Plot` | Intersection, **`Bisect(...).first`**, exported → `Precision_Task` → `Solve_Mixed` TOV solves. **Scientific.** | Production |
| Commented-out blocks and `#if 0` | — | — | Dead |

**Smallest API CompactStar needs:**
- `ContourFinder()`, `SetGrid`, `SetContVal(vector)`, `SetGridVals(GridVals_2D*)`, `GetContourSet`,
  and — only if file export stays in CONFIND — `SetWrkDir` + `ExportContour`;
- `Cont2D::size`, `operator[]` (`Coord3D` x, y), `GetVal`, `ConvertToCurve2D`.

Deleting the 3 `SetPlotConnected` calls and the 3 `Plot` calls **preserves** vendored behaviour.

## 27. Should CONFIND stay standalone?

**Yes — keep it standalone, but narrow.** CompactStar could absorb about 600 lines of code. The
case for keeping it separate:
- Its semantics must stay **frozen and characterized** independently of CompactStar's evolution.
- The Zaki 2.0 package pattern already exists, so the dependency cost is one `find_package`.
- It has historical reuse (the DMSS/DMSolarSignal trees consumed older variants).
- A separate repository gives an auditable home for the frozen behaviour and its golden files.

Reimplementing inside CompactStar would tempt a switch to "correct" marching squares or segment
joining, which is Class-C.

Reconsider absorption only if the owner confirms CompactStar is the sole consumer **and** chooses O4
(§11).

## 28. Long-term public API (no overdesign)

For 2.0, keep `CONFIND::ContourFinder` and `Cont2D` minus plotting and diagnostics; CompactStar then
only drops its plot calls.

A later 2.x can add a small functional layer:
- `FindContours(const Grid2D&, const GridVals_2D&, levels)`
- `FindContours(const Grid2D&, std::function<double(double,double)>, levels)`
- returning `std::vector<Contour{level, raw points}>`
- plus `HistoricalCurve(const Contour&)`, which is exactly today's `SortNew` semantics.

No `Plot`, ROOT, Python or mutable Zaki internals; all validation happens at the API boundary.

## 29. Version

**CONFIND 2.0.0.** The release removes the plotting API, ROOT, Python and bundled Zaki, and changes
the dependency model and the ownership contract. The prior labels were "1.0.0" (Git banner) and
"1.0" (C2/C3).

## 30. Class-C changes deferred for separate adjudication

These must not be bundled into modernization:
- `Coord3D` ordering and de-duplication semantics (O1–O4).
- The saddle decider (mean vs bilinear).
- Point equality (exact `x, y, z`).
- Segment joining (currently none) and the nearest-neighbour chaining with its start-point rule.
- Curve and iteration order.
- OpenMP decomposition and merge order (if reintroduced).
- Interpolation formula and operand order.
- Cell-origin arithmetic `(G+i·Δ)+Δ`.
- Grid indexing and node-coordinate convention.
- NaN policy.
- The rule for triangles lying exactly at the level (case (g) dropped).
- Log-axis interpolation in log space.
- `def_val` missing-sample handling.
- The `res_x` defect (moot if the modes are removed).

## 31. Modernization stages

| Stage | Content | Change type | Risk | Gate | Owner decision |
|---|---|---|---|---|---|
| **C0** | Decide: basis E; `-ffp-contract=off`; OpenMP removal; UB-removal policy; branch name; where file export lives | — | — | — | **Yes** |
| **C1** | New CONFIND branch from `89c5d9b`. Commit A: import C2 byte-exact (layout map documented). Commit B: delete `ConvertToDataSet` and the body of `Plot`. | Source only | None numerically | Manifest verified; compiles against `c8c6813`; fingerprint vs C3 | Authorize |
| **C2** | Characterization harness (§23) plus golden capture from the vendored arm64 oracle | Tests | None | Reconstructed source == oracle, **bitwise**, on all serial fixtures; two identical runs | — |
| **C3** | CMake rewrite against `Zaki::Zaki` | Build | FMA policy | Goldens unchanged | — |
| **C4** | Remove plotting, banner, `Color`, diagnostics, dead code, OpenMP modes, GSL includes | Deletion | Low | Goldens unchanged; no `Zaki::Util` diagnostic symbols | Breaking API |
| **C5** | UB / ownership / error cleanup: `delete[]`, empty-contour guard, `GridVals` validation, explicit includes, no path truncation, owning semantics documented | Behaviour defined for invalid inputs only | Low | Goldens; ASan/UBSan clean; negative tests | UB policy |
| **C6** | Install/export package; version 2.0.0 | Build | Low | External `find_package` smoke test | — |
| **C7** | Mac equivalence: CompactStar built against source-built Zaki + CONFIND from **one generation**; consumer fixture C23 | Qualification | Medium | Bitwise vs vendored (or a ratified tolerance) | Ratify |
| **C8** | CompactStar migration: drop `Plot`/`SetPlotConnected` and `Curve2D::Plot` calls, Python/NumPy, vendored libraries; target-based linking | CompactStar | Medium | CompactStar suites | Separate |
| **C9** | Linux GCC/Clang qualification | Portability | Medium | Goldens under the pinned FP policy | Later |

## 32. Top five first implementation changes (after approval)

1. **Byte-exact import plus the two-function reconstruction.**
   - Why: establishes the proven C3-equivalent basis.
   - Files: whole tree.
   - Scientific risk: none.
   - Breaking: drops `ConvertToDataSet` (never in C3); `Plot` becomes a no-op (as in C3).
   - Validation: manifest, compile against `c8c6813`, fingerprint comparison.
2. **Characterization harness and oracle goldens.**
   - Files: `tests/**`.
   - Risk: none; not breaking.
   - Validation: reproducible across two runs, `-ffp-contract=off`.
3. **CMake on `Zaki::Zaki` with `-ffp-contract=off`.**
   - Files: `CMakeLists.txt`, `cmake/*.in`; generated version header.
   - Risk: FP policy.
   - Breaking: build interface only.
   - Validation: goldens bitwise.
4. **Delete plotting, presentation and diagnostics.**
   - Files: all headers and sources.
   - Risk: low.
   - Breaking: yes (plot API).
   - Validation: goldens, symbol audit.
5. **UB removals with frozen numerics** (`delete[]`, empty contour, `GridVals` checks, includes,
   path handling).
   - Risk: low.
   - Breaking: defines behaviour only for previously-UB inputs.
   - Validation: goldens, sanitizers, new negative tests.

## 33. Answers to Q1–Q22

- **Q1** Should CONFIND stay standalone? Yes, narrow and frozen (§27).
- **Q2** Source basis? E: recovered `ed76163c` imported onto Git history, then the two-function
  reconstruction.
- **Q3** Confidence that it preserves CompactStar's numerics? **High** for CompactStar's serial
  sampled-grid path: structural identity plus oracle runs. The residual risks are the FP-contraction
  policy, libm, and missing exact source bytes.
- **Q4** Closest defensible relationship to the vendored binary: recovered snapshot minus
  `ConvertToDataSet` and the `Plot` body, compiled `-O0` with Xcode-14-era Apple clang, configured
  2023-05-01, built 2023-09-16. Structural equivalence, not byte identity.
- **Q5** Can ROOT be removed completely? Yes.
- **Q6** Can Python/matplotlib be absent? Yes.
- **Q7** Can all plotting APIs be deleted? Yes. Deletion equals the vendored behaviour, provided the
  recovered `Plot` side effect is never adopted.
- **Q8** DataColumn replacements: `SetLabel`, `PushBack`, `DataColumn(label, vector)` — preferably by
  deleting the two functions instead.
- **Q9** New Zaki API needed before modernization? None.
- **Q10** What does `Coord3D::operator<` do to results? Exact-r² equivalence classes; the
  first-inserted point survives; output is re-ordered by ascending r², which drives the
  nearest-neighbour start point and tie-breaks. It collapses origin-symmetric distinct points and
  ulp twins; at CompactStar scale, twins only.
- **Q11** Change `Coord3D` semantics now? **No** — it is historical, compiled into the authority, and
  changes CompactStar outputs.
- **Q12** Keep OpenMP? No; remove and revisit later.
- **Q13** Deterministic across thread counts? No for raw output. Serial output is deterministic.
- **Q14** Load-bearing tolerances: none exist. The load-bearing items are the exact decisions in §10.
- **Q15** Characterization tests required first: fixtures C1–C23 in §23, bitwise on the serial path.
- **Q16** Can the vendored binary be an oracle? Yes, on Darwin arm64 with the caveats in §24;
  demonstrated.
- **Q17** Cannot be authenticated:
  - the exact source bytes and build command of the arm64 archive;
  - the compiler patch version;
  - the Zaki header generation it used;
  - 2020–2023 history;
  - the intent behind radius de-duplication;
  - other consumers of the parallel modes;
  - x86_64 runtime behaviour.
- **Q18** API CompactStar still needs: §26.
- **Q19** Dead or legacy code to remove: the Git-era tree (Makefile, `.h`/`.cc`, binaries, objects,
  libraries, vendored `zaki`, example outputs, `.DS_Store`, `.vscode`) and everything in the
  DIAGNOSTIC, VISUALIZATION and LEGACY/DEAD rows of §25.
- **Q20** Target 2.0.0? Yes.
- **Q21** Needed before CompactStar can consume source-built CONFIND: stages C0–C7, plus the
  CompactStar-side changes in stage C8. Building Zaki and CONFIND from one generation eliminates the
  ODR mismatch.
- **Q22** Needed before Linux/cluster: Mac migration done; FP-contraction policy enforced for GCC;
  explicit includes; no `ACCESSPERMS`; target-based link order; portable serial goldens; a libm
  comparison policy for log axes.

## 34. Files likely touched in the first implementation phase (stages C1 and C2)

- **New:** `include/Confind/{Base,Bundle,Cell,Common,Cont2D,ContourFinder}.hpp`,
  `include/Confind/ConfindConfig.h`, `src/{Base,Cell,Common,Cont2D,ContourFinder}.cpp`,
  `CMakeLists.txt`, `README.md`, `LICENSE`, `tests/CMakeLists.txt`, `tests/characterization/*.cpp`,
  `tests/golden/*.txt`, and a provenance note carrying the manifest.
- **Reconstruction commit:** `Cont2D.hpp/.cpp` (delete `ConvertToDataSet`) and `ContourFinder.cpp`
  (`Plot` body).
- **Git-era tree removed:** `Makefile`, `include/*.h`, `src/*.cc`, `bin/`, `obj/`, `lib/`,
  `dependencies/`, `examples/`, `img/`, `.vscode/`, `.DS_Store`.

## 35. Tests for the first implementation phase

- **T00** — header hashes and symbol-surface audit.
- **T01–T23** — fixtures C1–C23.
- **T24** — reconstructed-source goldens bitwise equal to vendored-oracle goldens.
- **T25** — two-run determinism.
- **T26** — `GetContourSet` never sorted by `Plot`.
- **T27** — `ConvertToCurve2D` idempotence (`already_sorted`).

## 36. Blockers

- The stage C0 owner decisions.
- No exact source bytes exist for the 2023-09-16 build.
- Oracle capture requires Darwin arm64 and the ODR-affected vendored `libZaki.a`.
- No tracked CompactStar test exercises `TaskManager`.
- `Curve2D::Plot` is gone in Zaki 2.0, but CompactStar still calls it.
- CompactStar still requires Python/NumPy.
- Linux is untested.

## 37. Uncertainties

- The exact Xcode-14 clang build.
- The exact Zaki header generation behind the vendored binary.
- Byte-level identity of the source (unexecuted C3 paths are covered only by fingerprints).
- libm portability.
- How the ld64 weak/strong symbol choice resolves in CompactStar's own executables (observed only in
  the harness link).
- Whether the scientific behaviour holds on real CompactStar sequence grids (only synthetic fixtures
  were run).
- Log-axis behaviour (not yet run).
- Other consumers.

## 38. Final disposition

**A — CONFIND ARCHITECTURE FUNDAMENTALLY SOUND — SOURCE BASIS IDENTIFIED — BOUNDED MODERNIZATION
READY FOR OWNER REVIEW.**

## 39. Recommended next action

1. Review this report and make the stage C0 decisions:
   - source basis **E**;
   - FP-contraction policy (`-ffp-contract=off`);
   - removal of OpenMP from the core;
   - UB-removal policy for invalid and empty inputs;
   - branch name, e.g. `modernize/confind-2.0-basis`;
   - whether `ExportContour`/`SetWrkDir` stay in CONFIND.
2. Then separately authorize stage C1 (byte-exact import plus the two-function reconstruction) and
   stage C2 (the characterization harness with vendored-oracle goldens captured on this Mac), before
   any modernization edit.

---

## Appendix A — Evidence reproduction

All probes ran in a session-scoped scratch directory outside every repository; those files are
ephemeral. The probe sources are embedded in Appendix C, so everything below can be regenerated.

**Toolchain:** Apple clang 21.0.0 (`clang-2100.3.34.2`), Xcode SDK 27.0, Xcode `llvm-objdump`;
GSL 2.7 (MacPorts, `/opt/local`); libomp (Homebrew); Python 3.12.10 (miniforge); zlib from the SDK.
No `Zaki`/`zaki`/`Confind` headers exist under `/opt/local/include`, so no include shadowing.

**A.1 Recovered source vs vendored objects (structural comparison).**
Copy the recovered `include/Confind/*` and `source/*.cpp` into a scratch tree, apply the six
`DataColumn` substitutions of §16, then for each of the five sources:

```sh
xcrun clang++ -std=gnu++17 -O0 -arch arm64 -mmacosx-version-min=13.0 -pthread \
  -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -I<probe>/include \
  -I<CompactStar>/dependencies/include -I/opt/local/include -c src/<F>.cpp -o obj/<F>.cpp.o
# FMA check: same command plus -ffp-contract=off  → 0 fmadd/fmsub (default build: 15)
ar -x <CompactStar>/dependencies/lib/Confind/Darwin/arm64/libConfind.a   # vendored objects
python3 cmpfuncs3.py <vendored>/<F>.cpp.o obj/<F>.cpp.o                  # Appendix C.3
```

The field-offset / integer-immediate check over the 22 CompactStar-path functions used the inline
script in Appendix C.4.

**A.2 Reconstructed basis vs canonical Zaki `c8c6813`.**
From the recovered sources: remove `Cont2D::ConvertToDataSet` (definition, declaration and the
`DataSet.hpp` include) and replace the body of `ContourFinder::Plot` with `{}`. Then compile each
source with `-std=c++17 -O0 -arch arm64 -pthread -Xpreprocessor -fopenmp` and include path
`-I<Zaki repository root>` → **0 errors, 0 warnings**. The recovered source as-is (`-fsyntax-only`)
gives 12 errors, all inside `ConvertToDataSet` and `Plot`.

**A.3 Vendored-binary oracle.**

```sh
xcrun clang++ -std=c++17 -O0 -ffp-contract=off -arch arm64 -Xpreprocessor -fopenmp \
  -I/opt/homebrew/opt/libomp/include -I<CompactStar>/dependencies/include -I/opt/local/include \
  -c harness.cpp -o harness.o
xcrun clang++ -arch arm64 harness.o \
  <CompactStar>/dependencies/lib/Confind/Darwin/arm64/libConfind.a \
  <CompactStar>/dependencies/lib/Zaki/Darwin/arm64/libZaki.a \
  -L/opt/local/lib -lgsl -lgslcblas -lz -L/opt/homebrew/opt/libomp/lib -lomp \
  -L/Users/keeper/miniforge3/lib -lpython3.12 \
  -Wl,-rpath,/opt/homebrew/opt/libomp/lib -Wl,-rpath,/Users/keeper/miniforge3/lib \
  -Wl,-rpath,/opt/local/lib -o harness
./harness out.txt        # run twice; serial output bitwise identical
```

Full-dump variants (used for the collapse classification in §11) differ only in the `maxdump`
argument of the `analyse(...)` calls. `logprobe.cpp` (Appendix C.2) is built and linked the same
way.

**A.4 Scratch identities (SHA-256).**

| Item | SHA-256 |
|---|---|
| `harness.cpp` (Appendix C.1) | `810e27b39709ac1c212fed2ff513f00d254b8f3d500100219ad9779a3b435219` |
| `logprobe.cpp` (Appendix C.2) | `5dd62393a82429021c0b931e36537a044ff7551c2c9fc297b77095c0d09c35dc` |
| `cmpfuncs3.py` (Appendix C.3) | `d5734b28386ee0dad292165a9764a7fc282d7313bdc103257235346dc09b2473` |
| `harness` executable | `449fb8e4b4a8af7722f7ccdc492853f35505bf748310eba6551cc827d922448a` |
| Oracle output, run 1 | `f9d182b6bfd0a96bed75659376a94e8a9bbc00e0b3a28e9629945bace4f90776` |
| Oracle output, run 2 | `247f46fe3ec44c546684453c40ef32f1301c7a56a6c140e0029f10c9cde582c0` (differs from run 1 only in Parallel t = 2 / t = 4 raw hashes) |
| Regenerated recovered manifest | `ed76163c22e0a1f8ba3f71f62f5a56527528850650bbf7127da9c0c908d14083` |

## Appendix B — Oracle fixture results (vendored arm64 binary)

Counts are raw / exact-distinct / `ConvertToCurve2D`. "Lost" separates genuinely distinct points from
ulp-level twins (maximum pairwise separation ≤ 10⁻¹² × coordinate scale).

| Fixture | Setup | Raw | Distinct | Curve | Lost distinct | Lost twins |
|---|---|---|---|---|---|---|
| F01 | constant, levels 0.5 / 1 / 2 | 0 | 0 | skipped (UB) | — | — |
| F02 | z = x, L = 1.25 | 24 | 13 | 13 | 0 | 0 |
| F03 | z = x, L = 1.5 (centres on level) | 16 | 9 | 9 | 0 | 0 |
| F04 | z = y, L = 2.25 | 24 | 13 | 13 | 0 | 0 |
| F05a | z = x + y, L = 2.5 | 20 | 11 | 6 | 5 | 0 |
| F05b | z = x + y, L = 3 | 24 | 7 | 4 | 3 | 0 |
| F06a | origin circle, L = 1 | 48 | 32 | 5 | 27 | 0 |
| F06b | origin circle, L = 1.7 | 96 | 56 | 8 | 48 | 0 |
| F07 | off-centre circle, L = 1.7 | 96 | 57 | 49 | 0 | 8 |
| F09a | saddle (centre cell), L = 0 | 24 | 13 | 4 | 9 | 0 |
| F09b / F09c | saddle, L = ±0.1 | 32 | 22 | 5 | 17 | 0 |
| F09d | saddle at a vertex, L = 0 | 32 | 9 | 3 | 6 | 0 |
| F09e / f / g | checkerboard, L = 0 / +0.5 / −0.5 | 8 | 5 / 6 / 6 | 3 / 4 / 3 | 2 / 2 / 3 | 0 |
| F10 | two circles, L = 0.5 | 64 | 32 | 9 | 23 | 0 |
| F11a / F11b | z = x, L = 0 / 4 (boundaries) | 8 | 5 | 5 | 0 | 0 |
| F12 | z = x, L = 2 (interior grid line) | 16 | 5 | 5 | 0 | 0 |
| F13 | (x−2)², L = 0.01 | 24 | 20 | 14 | 0 | 6 |
| F14 | missing sample (`def_val`) | 32 | 17 | 17 | 0 | 0 |
| F15 | sampled = callable Fast = callable Normal | — | — | — | bitwise-equal raw and curve hashes | |
| F16a | off-centre circle 16×16, serial, L = 1.7 / 0.9 | 204 / 140 | 109 / 76 | 104 / 72 | 0 | 5 / 4 |
| F16b/c | Parallel / Ludicrous t = 1 / 2 / 4 (L = 1.7) | 204 / 218 / 256 | 109 | 104 | — | — |
| F16e | Parallel t = 1, nx = 16, ny = 8 | 60 (serial: 156) | 36 | 31 | `res_x` truncation | |
| F17 | CompactStar scale 50×50, L = 0.3 / 0.6 / 0.9 | 728 / 596 / 272 | 370 / 307 / 137 | 368 / 300 / 137 | 0 | 2 / 7 / 0 |
| F18 | NaN sample | 32 | 13 | 12 (ends with (2,2)×8) | 1 | 0 |
| F19 | raw sequence before / after `Plot()` | identical | | | | |

`logprobe`: a harness-side `Z_LOG_ERROR` is printed and counted ("A total of 1 errors"); the
`Z_LOG_ERROR` raised inside libConfind by `SortNew()` on an unfound contour is neither printed nor
counted.

## Appendix C — Probe sources

The blocks below are the exact scratch files identified in Appendix A.4.

The content of each fenced block in C.1–C.3 reproduces the named file byte-for-byte, so its
SHA-256 can be checked against Appendix A.4.

### C.1 `harness.cpp` — vendored-binary oracle

<details>
<summary>Show source (245 lines)</summary>

```cpp
// SCRATCH-ONLY behavioural probe of the vendored CompactStar arm64 libConfind.a.
// Not part of any repository. Uses only the authenticated vendored public headers.
// Deliberately does NOT use Coord3D::operator< / operator== / XYDist2 so that this
// translation unit emits no competing weak definitions of the comparator.
#include <Confind/ContourFinder.hpp>
#include <Zaki/Util/Logger.hpp>
#include <Zaki/Vector/DataSet.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <limits>
#include <set>
#include <string>
#include <utility>
#include <vector>

static FILE* OUT = nullptr;

static uint64_t bits(double d) { uint64_t u; std::memcpy(&u, &d, 8); return u; }

// FNV-1a over the exact bit patterns of a point sequence (order-sensitive)
static uint64_t seqhash(const std::vector<std::pair<double, double>>& v)
{
  uint64_t h = 1469598103934665603ULL;
  for (auto& p : v)
    for (uint64_t w : {bits(p.first), bits(p.second)})
      for (int k = 0; k < 8; ++k) { h ^= (w >> (8 * k)) & 0xff; h *= 1099511628211ULL; }
  return h;
}

// r^2 exactly as the vendored (unfused, -O0) Coord3D::XYDist2({0,0,0}) computes it.
// This TU is compiled with -ffp-contract=off so the expression is not fused.
static double r2_unfused(double x, double y)
{
  volatile double xx = (x - 0.0) * (x - 0.0);
  volatile double yy = (y - 0.0) * (y - 0.0);
  return xx + yy;
}

struct Summary
{
  size_t raw = 0, exact_distinct = 0, r2_classes = 0, curve = 0;
  uint64_t raw_hash = 0, curve_hash = 0;
};

static void dump_points(const char* tag, const std::vector<std::pair<double, double>>& v, size_t maxn)
{
  for (size_t i = 0; i < v.size() && i < maxn; ++i)
    std::fprintf(OUT, "    %s[%zu] = (%.17g, %.17g)  [%a, %a]\n", tag, i, v[i].first, v[i].second,
                 v[i].first, v[i].second);
  if (v.size() > maxn) std::fprintf(OUT, "    %s ... (%zu more)\n", tag, v.size() - maxn);
}

static Summary analyse(const char* name, std::vector<CONFIND::Cont2D> cs, size_t maxdump, bool curve_ok = true)
{
  Summary total;
  for (size_t c = 0; c < cs.size(); ++c)
  {
    std::vector<std::pair<double, double>> raw;
    for (size_t p = 0; p < cs[c].size(); ++p) raw.emplace_back(cs[c][p].x, cs[c][p].y);
    std::set<std::pair<uint64_t, uint64_t>> ex;
    std::set<uint64_t> r2;
    for (auto& q : raw) { ex.insert({bits(q.first), bits(q.second)}); r2.insert(bits(r2_unfused(q.first, q.second))); }

    std::vector<std::pair<double, double>> cur;
    bool do_curve = curve_ok && !raw.empty();
    if (do_curve)
    {
      CONFIND::Cont2D copy = cs[c];
      Zaki::Math::Curve2D cv = copy.ConvertToCurve2D();
      for (auto& q : cv.pts) cur.emplace_back(q.x, q.y);
    }
    std::fprintf(OUT, "  [%s] contour %zu level=%.17g found=%d raw=%zu exact_distinct_xy=%zu distinct_r2=%zu curve=%s%zu raw_hash=%016llx curve_hash=%016llx\n",
                 name, c, cs[c].GetVal(), (int)cs[c].GetFound(), raw.size(), ex.size(), r2.size(),
                 do_curve ? "" : "SKIPPED:", cur.size(), (unsigned long long)seqhash(raw),
                 (unsigned long long)seqhash(cur));
    if (maxdump)
    {
      dump_points("raw", raw, maxdump);
      if (do_curve) dump_points("curve", cur, maxdump);
    }
    total.raw += raw.size(); total.exact_distinct += ex.size(); total.r2_classes += r2.size(); total.curve += cur.size();
  }
  return total;
}

// ---------------------------------------------------------------------------
using Field = std::function<double(double, double)>;

static Zaki::Math::Grid2D grid(double x0, double x1, size_t nx, double y0, double y1, size_t ny)
{
  return {{{x0, x1}, nx, "Linear"}, {{y0, y1}, ny, "Linear"}};
}

// Sampled-grid path (the one CompactStar uses): SetGrid + SetContVal + SetGridVals(GridVals_2D*)
static std::vector<CONFIND::Cont2D> run_sampled(const Zaki::Math::Grid2D& g, const Field& f,
                                                const std::vector<double>& lv,
                                                const std::vector<std::pair<size_t, size_t>>& drop = {})
{
  const size_t nx = g.xAxis.res, ny = g.yAxis.res;
  const double dx = (g.xAxis.Max() - g.xAxis.Min()) / nx;  // same expression as CONFIND::SetDeltas
  const double dy = (g.yAxis.Max() - g.yAxis.Min()) / ny;
  std::vector<double> iv, jv, zv;
  for (size_t j = 0; j <= ny; ++j)
    for (size_t i = 0; i <= nx; ++i)
    {
      bool skip = false;
      for (auto& d : drop) if (d.first == i && d.second == j) skip = true;
      if (skip) continue;
      iv.push_back(i); jv.push_back(j);
      zv.push_back(f(g.xAxis.Min() + i * dx, g.yAxis.Min() + j * dy));
    }
  Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{
      Zaki::Vector::DataColumn("i", iv), Zaki::Vector::DataColumn("j", jv), Zaki::Vector::DataColumn("z", zv)});
  Zaki::Math::GridVals_2D gv(ds, 0, nx, 1, ny, 2);
  CONFIND::ContourFinder con;
  con.SetGrid(g);
  con.SetContVal(lv);
  con.SetGridVals(&gv);
  return con.GetContourSet();
}

static Field g_callable;
static double callable_trampoline(double x, double y) { return g_callable(x, y); }

static std::vector<CONFIND::Cont2D> run_callable(const Zaki::Math::Grid2D& g, const Field& f,
                                                 const std::vector<double>& lv,
                                                 CONFIND::ContourFinder::Mode m, int threads = 1)
{
  g_callable = f;
  CONFIND::ContourFinder con;
  con.SetGrid(g);
  con.SetContVal(lv);
  con.SetFunc(&callable_trampoline);
  con.SetThreads(threads);
  con.SetGridVals(m);
  return con.GetContourSet();
}

int main(int argc, char** argv)
{
  OUT = std::fopen(argc > 1 ? argv[1] : "oracle_out.txt", "w");
  Zaki::Util::LogManager::SetLogLevels(Zaki::Util::LogLevel::Error);

  // F01 constant field (ConvertToCurve2D deliberately skipped: SortNew reads pts[0] on empty -> UB)
  analyse("F01_constant", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double, double) { return 1.0; }, {0.5, 1.0, 2.0}), 4, false);

  // F02..F04 linear crossings
  analyse("F02_vertical_x=1.25", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {1.25}), 40);
  analyse("F03_vertical_centre_on_level_x=1.5", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {1.5}), 40);
  analyse("F04_horizontal_y=2.25", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double, double y) { return y; }, {2.25}), 40);
  analyse("F05a_diagonal_x+y=2.5", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return x + y; }, {2.5}), 60);
  analyse("F05b_diagonal_through_vertices_x+y=3", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return x + y; }, {3.0}), 60);

  // F06..F08 circles (origin-centred = Coord3D radius-ordering stress; off-centre = control)
  analyse("F06a_circle_origin_r2=1_(vertices_on_level)", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return x * x + y * y; }, {1.0}), 80);
  analyse("F06b_circle_origin_r2=1.7", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return x * x + y * y; }, {1.7}), 80);
  analyse("F07_circle_offcentre_r2=1.7", run_sampled(grid(-2, 2, 8, -2, 2, 8), [](double x, double y) { return (x - 0.3) * (x - 0.3) + (y - 0.2) * (y - 0.2); }, {1.7}), 80);

  // F09 saddle z = x*y ; origin at a cell centre (n odd) and at a vertex (n even)
  analyse("F09a_saddle_centre_cell_level0", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {0.0}), 60);
  analyse("F09b_saddle_centre_cell_level+0.1", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {0.1}), 60);
  analyse("F09c_saddle_centre_cell_level-0.1", run_sampled(grid(-1.5, 1.5, 3, -1.5, 1.5, 3), [](double x, double y) { return x * y; }, {-0.1}), 60);
  analyse("F09d_saddle_vertex_level0", run_sampled(grid(-2, 2, 4, -2, 2, 4), [](double x, double y) { return x * y; }, {0.0}), 60);
  // pure checkerboard 2x2 samples: corners (+1,-1,+1,-1): centre avg 0
  analyse("F09e_checkerboard_level0", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {0.0}), 40);
  analyse("F09f_checkerboard_level+0.5", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {0.5}), 40);
  analyse("F09g_checkerboard_level-0.5", run_sampled(grid(0, 1, 1, 0, 1, 1), [](double x, double y) { return (x == y) ? 1.0 : -1.0; }, {-0.5}), 40);

  // F10 two disjoint closed contours
  analyse("F10_two_circles", run_sampled(grid(-3, 3, 12, -1.5, 1.5, 6), [](double x, double y) { return std::min((x - 1.5) * (x - 1.5) + y * y, (x + 1.5) * (x + 1.5) + y * y); }, {0.5}), 0);

  // F11/F12 boundary & grid-edge coincidence
  analyse("F11a_on_left_boundary_x=0", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {0.0}), 40);
  analyse("F11b_on_right_boundary_x=4", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {4.0}), 40);
  analyse("F12_along_interior_grid_line_x=2", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x; }, {2.0}), 60);

  // F13 two nearby branches
  analyse("F13_near_branches", run_sampled(grid(0, 4, 16, 0, 1, 2), [](double x, double) { return (x - 2) * (x - 2); }, {0.01}), 40);

  // F14 missing samples -> def_val = -1 (GridVals_2D second ctor)
  analyse("F14_missing_sample_defval", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double) { return x + 10.0; }, {11.5}, {{2, 2}}), 60);

  // F15 callable (Fast / Normal) vs sampled equivalence on exact dyadic grids
  {
    Field circ = [](double x, double y) { return x * x + y * y; };
    Field sadl = [](double x, double y) { return x * y; };
    auto g1 = grid(-2, 2, 8, -2, 2, 8);
    analyse("F15a_sampled_circle", run_sampled(g1, circ, {1.7, 1.0}), 0);
    analyse("F15b_callable_Fast_circle", run_callable(g1, circ, {1.7, 1.0}, CONFIND::ContourFinder::Fast), 0);
    analyse("F15c_callable_Normal_circle", run_callable(g1, circ, {1.7, 1.0}, CONFIND::ContourFinder::Normal), 0);
    auto g2 = grid(-1.5, 1.5, 3, -1.5, 1.5, 3);
    analyse("F15d_sampled_saddle", run_sampled(g2, sadl, {0.1}), 0);
    analyse("F15e_callable_Fast_saddle", run_callable(g2, sadl, {0.1}, CONFIND::ContourFinder::Fast), 0);
  }

  // F16 OpenMP modes (square grid avoids res_x = yAxis.res heap overflow in Ludicrous)
  {
    Field f = [](double x, double y) { return (x - 0.3) * (x - 0.3) + (y - 0.2) * (y - 0.2); };
    auto g = grid(-2, 2, 16, -2, 2, 16);
    analyse("F16a_Fast_serial", run_callable(g, f, {1.7, 0.9}, CONFIND::ContourFinder::Fast), 0);
    for (int t : {1, 2, 4})
    {
      std::string n1 = "F16b_Parallel_t" + std::to_string(t), n2 = "F16c_Ludicrous_t" + std::to_string(t);
      analyse(n1.c_str(), run_callable(g, f, {1.7, 0.9}, CONFIND::ContourFinder::Parallel, t), 0);
      analyse(n2.c_str(), run_callable(g, f, {1.7, 0.9}, CONFIND::ContourFinder::Ludicrous, t), 0);
    }
    // res_x = yAxis.res defect: nx=16 > ny=8 (safe direction: truncation, no overflow)
    auto gr = grid(-2, 2, 16, -2, 2, 8);
    analyse("F16d_Fast_nx16_ny8", run_callable(gr, f, {1.7}, CONFIND::ContourFinder::Fast), 0);
    analyse("F16e_Parallel_t1_nx16_ny8", run_callable(gr, f, {1.7}, CONFIND::ContourFinder::Parallel, 1), 0);
  }

  // F17 CompactStar-scale coordinates (~1e15), smooth field, 50x50
  {
    auto g = grid(1e15, 4e15, 50, 3e13, 1.4e15, 50);
    Field f = [](double x, double y) { double u = x / 1e15 - 2.3, v = y / 1e15 - 0.6; return std::exp(-(u * u + 2 * v * v)); };
    analyse("F17_CompactStar_scale", run_sampled(g, f, {0.3, 0.6, 0.9}), 0);
  }

  // F18 NaN sample in the field (status() treats NaN as 'on' the level)
  analyse("F18_nan_sample", run_sampled(grid(0, 4, 4, 0, 4, 4), [](double x, double y) { return (x == 2 && y == 2) ? std::numeric_limits<double>::quiet_NaN() : x; }, {1.25}), 60);

  // F19 Plot() side-effect probe: raw order before and after Plot()
  {
    auto g = grid(-2, 2, 8, -2, 2, 8);
    Field f = [](double x, double y) { return (x - 0.3) * (x - 0.3) + (y - 0.2) * (y - 0.2); };
    std::vector<double> iv, jv, zv; const size_t n = 8; const double d = 0.5;
    for (size_t j = 0; j <= n; ++j) for (size_t i = 0; i <= n; ++i) { iv.push_back(i); jv.push_back(j); zv.push_back(f(-2 + i * d, -2 + j * d)); }
    Zaki::Vector::DataSet ds(std::vector<Zaki::Vector::DataColumn>{Zaki::Vector::DataColumn("i", iv), Zaki::Vector::DataColumn("j", jv), Zaki::Vector::DataColumn("z", zv)});
    Zaki::Math::GridVals_2D gv(ds, 0, n, 1, n, 2);
    CONFIND::ContourFinder con; con.SetGrid(g); con.SetContVal({1.7}); con.SetGridVals(&gv);
    analyse("F19a_before_Plot", con.GetContourSet(), 0);
    con.SetPlotConnected();
    con.Plot("scratch_plot_probe");
    analyse("F19b_after_Plot", con.GetContourSet(), 0);
  }

  std::fclose(OUT);
  return 0;
}
```

</details>

### C.2 `logprobe.cpp` — CONFIND-internal log delivery probe

<details>
<summary>Show source (15 lines)</summary>

```cpp
// SCRATCH-ONLY: does a CONFIND-internal Z_LOG_ERROR reach Zaki's LogManager intact?
#include <Confind/ContourFinder.hpp>
#include <Zaki/Util/Logger.hpp>
#include <cstdio>
int main()
{
  Zaki::Util::LogManager::SetLogLevels(Zaki::Util::LogLevel::Error);
  std::puts("---- control: Zaki-side Z_LOG_ERROR from this TU (current header layout) ----");
  Z_LOG_ERROR("control error from harness TU");
  std::puts("---- CONFIND-side: SortNew() on an unfound contour logs Z_LOG_ERROR inside libConfind ----");
  CONFIND::Cont2D c(1.0);
  c.SortNew();
  std::puts("---- end ----");
  return 0;
}
```

</details>

### C.3 `cmpfuncs3.py` — per-function structural comparator

Usage: `python3 cmpfuncs3.py <vendored.o> <probe.o>`. Compares, per function, the ordered
semantic call targets, floating-point opcode sequence and branch-condition sequence, after
filtering diagnostics and libc++ template noise.

<details>
<summary>Show source (135 lines)</summary>

```python
#!/usr/bin/env python3
"""Scratch-only semantic fingerprint comparison of two Mach-O arm64 objects (-O0).

Per function (demangled name, libc++ ABI tags stripped):
  * ordered list of *semantic* call targets: CONFIND::*, Zaki::* (except inline diagnostics
    ctor/dtor spellings), omp_*, libm (pow/log10/sqrt/fabs/abs), std::set/sort/find usage,
    Coord3D operators;
  * ordered list of FP opcodes (fadd/fsub/fmul/fdiv/fmadd/fcmp...);
  * ordered list of condition codes (b.cond / cset).
"""
import re
import subprocess
import sys

OBJDUMP = subprocess.check_output(["xcrun", "--find", "llvm-objdump"]).decode().strip()
FP_OPS = {"fadd", "fsub", "fmul", "fdiv", "fmadd", "fmsub", "fnmadd", "fnmsub",
          "fcmp", "fcmpe", "fsqrt", "fneg", "fabs", "fcvtzs", "fcvtzu", "scvtf",
          "ucvtf", "fmin", "fmax", "fminnm", "fmaxnm"}


def demangle_all(names):
    uniq = sorted(set(names))
    p = subprocess.run(["c++filt"], input="\n".join(uniq), capture_output=True, text=True)
    return dict(zip(uniq, p.stdout.splitlines()))


def norm(s):
    s = re.sub(r"\[abi:[^\]]*\]", "", s)
    s = re.sub(r"B\d+[a-z]+\d+", "", s)
    s = s.replace("std::__1::", "std::").replace("std::__math::", "")
    return s


def interesting(t):
    if re.search(r"\b(pow|log10|sqrt|fabs|exp|log)\b", t) and "Zaki::" not in t:
        return re.sub(r".*\b(pow|log10|sqrt|fabs|exp|log)\b.*", r"libm:\1", t)
    if t.startswith("_omp_") or t.startswith("omp_"):
        return t.lstrip("_")
    if "CONFIND::" in t.split("(")[0]:
        return t
    if "Coord3D::operator" in t or "Coord3D::XYDist2" in t:
        return t
    if "Zaki::" in t.split("(")[0]:
        if any(x in t for x in ("InstrumentationTimer", "LogEntry", "LogManager", "ObjManager", "MemManager", "ObjCopyConstruct", "ObjEvent", "PtrStr")):
            return None
        return t.split("(")[0]
    if re.search(r"^std::set<Zaki::Physics::Coord3D.*::set<", t):
        return "std::set<Coord3D>::range_ctor"
    if re.search(r"^(void )?std::sort<", t) or re.search(r"^std::find<", t):
        return re.sub(r"<.*", "", t.split("(")[0])
    return None


def disasm(obj):
    out = subprocess.check_output(
        [OBJDUMP, "-d", "-r", "--no-show-raw-insn", obj], stderr=subprocess.DEVNULL
    ).decode(errors="replace")
    funcs = {}
    cur = None
    raw_relocs = []
    for line in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
        if m:
            cur = m.group(1)
            funcs[cur] = {"relocs": [], "fp": [], "cond": [], "n": 0}
            continue
        if cur is None:
            continue
        m = re.search(r"ARM64_RELOC_BRANCH26\s+(.+)$", line)
        if m:
            funcs[cur]["relocs"].append(m.group(1).strip())
            raw_relocs.append(m.group(1).strip())
            continue
        if "ARM64_RELOC" in line:
            continue
        m = re.match(r"^\s+[0-9a-f]+:\s+(\S+)\s*(.*)$", line)
        if m:
            op, args = m.group(1), m.group(2)
            funcs[cur]["n"] += 1
            if op.split(".")[0] in FP_OPS:
                funcs[cur]["fp"].append(op)
            if op.startswith("b.") and op != "b":
                funcs[cur]["cond"].append(op[2:])
            elif op in ("cset", "csel", "csinc", "fcsel"):
                funcs[cur]["cond"].append(args.split(",")[-1].strip())
    dm = demangle_all(list(funcs.keys()) + raw_relocs)
    res = {}
    for k, v in funcs.items():
        name = norm(dm.get(k, k))
        rel = []
        for r in v["relocs"]:
            t = interesting(norm(dm.get(r, r)))
            if t:
                rel.append(t)
        v["sem"] = rel
        res[name] = v
    return res


def main():
    a_obj, b_obj = sys.argv[1], sys.argv[2]
    verbose = len(sys.argv) > 3 and sys.argv[3] == "-v"
    A, B = disasm(a_obj), disasm(b_obj)
    keys = sorted(k for k in set(A) | set(B) if k.startswith("CONFIND::") or "Coord3D::" in k.split("(")[0])
    tallies = {"MATCH": 0, "DIFF": 0, "ONLY-VENDORED": 0, "ONLY-PROBE": 0}
    for k in keys:
        if k in A and k not in B:
            tallies["ONLY-VENDORED"] += 1
            print(f"ONLY-VENDORED  {k}")
            continue
        if k in B and k not in A:
            tallies["ONLY-PROBE"] += 1
            print(f"ONLY-PROBE     {k}")
            continue
        a, b = A[k], B[k]
        ok = a["sem"] == b["sem"] and a["fp"] == b["fp"] and a["cond"] == b["cond"]
        tag = "MATCH" if ok else "DIFF"
        tallies[tag] += 1
        if ok and not verbose:
            continue
        print(f"{tag:14s} {k}   (insns vendored={a['n']} probe={b['n']})")
        if a["sem"] != b["sem"]:
            print(f"     sem vendored: {a['sem']}")
            print(f"     sem probe   : {b['sem']}")
        if a["fp"] != b["fp"]:
            print(f"     fp vendored: {' '.join(a['fp'])}")
            print(f"     fp probe   : {' '.join(b['fp'])}")
        if a["cond"] != b["cond"]:
            print(f"     cond vendored: {' '.join(a['cond'])}")
            print(f"     cond probe   : {' '.join(b['cond'])}")
    print("SUMMARY " + " ".join(f"{k}={v}" for k, v in tallies.items()))


if __name__ == "__main__":
    main()
```

</details>

### C.4 Field-offset / integer-immediate comparator (inline script)

Run from the scratch root, with `ar_arm64/` holding the extracted vendored objects and `probe/obj/`
holding the probe objects of A.1. It compares, for the 22 CompactStar-path functions, the ordered
object-field offsets (loads/stores through non-frame registers) and small integer immediates.
Result: 19 identical; `GetStatus` (jump table vs compare chain), `case48` (inlined `abs` vs call to
`_abs`) and the full `Cell` constructor (order of literal loads) differ only in codegen.

<details>
<summary>Show script</summary>

```python
import re, subprocess
OBJ = subprocess.check_output(["xcrun","--find","llvm-objdump"]).decode().strip()
def dis(obj, sym):
    out = subprocess.check_output([OBJ,"-d","--no-show-raw-insn",f"--disassemble-symbols={sym}",obj],stderr=subprocess.DEVNULL).decode()
    seq=[]
    for l in out.splitlines():
        m = re.match(r"^\s+[0-9a-f]+:\s+(\S+)\s*(.*)$", l)
        if not m: continue
        op,args = m.group(1), m.group(2)
        # field offsets relative to non-stack base registers, and small integer immediates in arithmetic/compare
        for base, off in re.findall(r"\[(x(?!29)\d+|x8|x9|x10|x11),\s*#(0x[0-9a-f]+|\d+)\]", args):
            seq.append(f"F{off}")
        if op in ("cmp","subs","add","sub","mov","udiv","mul","lsl") and re.search(r"#(0x[0-9a-f]+|\d+)$", args) and "sp" not in args and "x29" not in args:
            seq.append(f"{op}:{args.split('#')[-1]}")
    return seq
syms = {
 "Cell.cpp.o": ["__ZN7CONFIND4Cell9FindVertsEv","__ZN7CONFIND4Cell9GetStatusEv","__ZN7CONFIND4Cell6case36ERKNS_8triangleERKi","__ZN7CONFIND4Cell5case5ERKNS_8triangleE","__ZN7CONFIND4Cell6case48ERKNS_8triangleE","__ZN7CONFIND4Cell6SetIdxERKmS2_","__ZN7CONFIND4Cell10EvalCenterEv","__ZN7CONFIND4Cell10SetVertexZERKmRKd","__ZN7CONFIND4Cell12SetTrianglesEv","__ZN7CONFIND4CellC2ERKmRKdS2_S4_PNS_6BundleES4_"],
 "Cont2D.cpp.o": ["__ZN7CONFIND6Cont2D7SortNewEv","__ZN7CONFIND6Cont2D12RMDuplicatesEv","__ZN7CONFIND6Cont2D16ConvertToCurve2DEv","__ZN7CONFIND6Cont2D6AddPtsERKNSt3__16vectorIN4Zaki7Physics7Coord3DENS1_9allocatorIS5_EEEE","__ZN7CONFIND6Cont2DpLERKS0_"],
 "ContourFinder.cpp.o": ["__ZN7CONFIND13ContourFinder16FindNextContoursEPd","__ZN7CONFIND13ContourFinder11SetGridValsEPN4Zaki4Math11GridVals_2DE","__ZN7CONFIND13ContourFinder10SetContValERKNSt3__16vectorIdNS1_9allocatorIdEEEE","__ZNK7CONFIND13ContourFinder13GetContourSetEv","__ZN7CONFIND13ContourFinder7SetGridERKN4Zaki4Math6Grid2DE","__ZN7CONFIND13ContourFinder9SetDeltasEv","__ZN7CONFIND13ContourFinder15FindContourFastERNS_6Cont2DEPd"],
}
for obj, lst in syms.items():
    for s in lst:
        a = dis(f"ar_arm64/{obj}", s); b = dis(f"probe/obj/{obj}", s)
        name = subprocess.run(["c++filt"], input=s, capture_output=True, text=True).stdout.strip()
        if not a and not b: print(f"??  {name} (not found)"); continue
        print(f"{'SAME' if a==b else 'DIFF'}  {name[:95]}  (n={len(a)}/{len(b)})")
        if a != b:
            import difflib
            for l in list(difflib.unified_diff(a,b,lineterm="",n=0))[2:12]: print("      ", l)
```

</details>

---

## Negative attestation

- The reconnaissance made **no repository changes**. Afterwards, CONFIND (`89c5d9b`), ZakiLib
  (`c8c6813`) and CompactStar (`812463a`) were verified at their expected HEADs with clean tracked
  trees; the vendored archive hashes were unchanged; the recovered manifest was re-verified
  (`ed76163c…4083`).
- This document was added to this repository afterwards, at the owner's request.
- No branch, commit, push, merge or tag was made. No contour algorithm, tolerance, `Coord3D` ordering
  or de-duplication rule was changed.
- Compilation was limited to disposable scratch objects of the recovered snapshot and scratch
  executables linking the unchanged vendored archives. No library was produced and nothing was
  linked into CompactStar.
- No Linux or cluster work, no EKU access, and no Google Drive reads.
