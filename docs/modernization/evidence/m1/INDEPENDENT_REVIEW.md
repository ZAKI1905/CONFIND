# CONFIND 2.0 — Independent Scientific / Numerical / Concurrency Review

Candidate `b243851be6e0acc2e5f12ee6786934603965353a` (`modernize/confind-2.0`), local Mac arm64, 2026-09-28.
Read-only review: no repository edits, commits, pushes, merges or tags. All builds, probes and mutations ran in
scratch copies (`git archive` exports) outside every repository.

## Disposition

**C — MATERIAL CORRECTIONS REQUIRED BEFORE OWNER ACCEPTANCE**

BLOCKING 0 · MATERIAL 1 · NONBLOCKING 7 · NOTE 12

The core claims hold: exact provenance, a correct oracle, byte-exact historical serial numerics in the Debug (-O0)
configuration, a correct deterministic executor, and complete OpenMP/ROOT/Python/plotting removal. One material
defect remains. Optimized builds silently change frozen log-axis arithmetic. This configuration is the one the
README recommends, and the report claims it is qualified.

## MATERIAL

### M-1 Optimized builds diverge from the historical authority on log axes

At `-O1` and above, clang rewrites every `pow(10, x)` into `__exp10(x)` in the log-axis paths:
`Cell::case36/case5/case48` (contour coordinates), `EvalSimpleFunc/EvalMemFunc`, and `ContourFinder::Evaluate`.
The Debug archive and the vendored historical archive both call `_pow`. `-ffp-contract=off` does not govern this
rewrite.

- **Libm difference.** Apple libm `pow(10,x)` and `__exp10(x)` differ by 1 ulp on 0.19% of 20,000,000 random
  x in [-30, 30], and on 689 of 403,000 log-grid nodes.
- **Randomized fixtures (600 log-axis).**
  - Candidate Release vs historical oracle: 840 of 3,621 contour records differ, in 243 of 600 fixtures, across
    the sampled, Fast and Normal paths.
  - Candidate Debug vs oracle: 0 differences.
  - On 600 linear-only fixtures, both Debug and Release have 0 differences.
- **CompactStar-shaped grid.** Log-log 99×150 grid taken from `TaskManager.cpp`, with a 52-level
  TaskManager-like ladder and 52,632 raw points.
  - Debug: 0 differences.
  - Release: 258 raw-point lines and 184 curve lines differ, touching all 52 levels.
- **Example.** Raw point x = `0x1.46a1c5f584575p+43` (oracle) vs `0x1.46a1c5f584576p+43` (Release).
- **Root cause confirmed.** A scratch Release build with the rewrite blocked (`-fno-builtin`) is 0/3,621 on both
  log and linear fixtures. `-fno-builtin-pow` does not block it, because libc++ calls `__builtin_pow`.
- **Why the test suite missed it.** The C22 log fixtures use a dyadic 1–100, 4-cell grid, which is
  non-discriminating.
- **Why it matters.**
  - The README tells users to build Release and says log sampling uses `pow(10, ...)`.
  - CompactStar's `d_ax` can be `"Log"`.
  - The predeclaration froze log-axis arithmetic with no tolerance.
- **Correction (owner's choice).** Either:
  - make optimized builds call `pow` in these paths and re-qualify Release, or
  - restrict frozen-numerics claims and build guidance to the -O0/Debug configuration.

  Either way, add a discriminating log-axis characterization frozen from the vendored oracle, plus a
  Release-vs-oracle differential gate.

## NONBLOCKING

- **N-1 Weak-symbol interposition of Zaki inline `Coord3D::XYDist2`/`operator<`.**
  - In the Debug library, a consumer TU compiled with default flags that uses `std::set<Coord3D>` wins
    link-time resolution. CONFIND's frozen de-duplication then changes: F17 curves go 368→367 and 300→299.
  - The Release library is immune because the comparator is inlined.
  - CompactStar@812463a has no such TU, so this is latent.
  - It interacts with M-1: a Debug-only resolution increases exposure.
  - Recommend documenting the consumer constraint, or an owner decision on a CONFIND-owned comparator with
    identical arithmetic.
- **N-2 Test discrimination gaps.** 17 of 20 mutations are detected. Three survive:
  - M14, executor silently serial. No test asserts real concurrency, and the plain small-grid worker tests run
    effectively on one worker. The perturbed stress test is the real concurrency exerciser.
  - M15, `Evaluate` node-coordinate convention changed.
  - M13, cancellation removed.
- **N-3 Behavior changes not fully documented** (non-numerical or error paths):
  - Copy and assignment now propagate `wrk_dir`/name.
  - `SetWrkDir` is recursive and can throw.
  - Empty level sets now throw on the GridVals path; this was historically a no-op.
  - `SetContVal(values, labels)` rejects longer label lists.
  - `SetGrid` rejects res=0 and unknown scales.
  - Export paths over 149 characters are no longer truncated, which changes output file names for long working
    directories.
- **N-4 `Evaluate` operational limits for stellar campaigns:**
  - It re-samples every node even when all levels are found; `SetGridVals(Mode)` has an early return.
  - Samples are discarded after extraction.
  - It is all-or-nothing on exception.
  - There is no cooperative cancellation of in-flight tasks.
  - There is no worker upper bound: 20,000 requested workers means 20,000 factory calls before thread creation
    fails.
- **N-5 CMake CMP0128 stays OLD (`cmake_minimum_required 3.16`).** `CXX_EXTENSIONS OFF` is not honored under
  GCC 15 or Clang 21, which compile as gnu++17. The "-std=c++17 verified" claim holds for AppleClang only. This
  matters for Linux work.
- **N-6 Frozen-source audit evidence is not reproducible.** The normalizer was never committed, and the audit omits
  the `Cell` constructor/`SetBundlePtr`, `SetDeltas`, `FindContour`, `ConvertToCurve2D` and the `EvalFunc` paths.
  My independent diff and fuzzing found no defect in these.
- **N-7 Intermediate commits are not all green.**
  - `cd7de4d` does not link; this is disclosed.
  - `0e4f6b4` and `297c8a7` fail `historical_export` when the build path exceeds the historical 150-character
    truncation; this is not disclosed. With short paths their export numerics are exact.

## NOTE

1. Oracle link closure uses miniforge `libc++` via `@rpath` and MacPorts `libomp`. No effect on results.
2. The oracle harness contributes weak header-inline `Cont2D` destructor/vtable/typeinfo copies built from the
   same vendored headers. No effect.
3. With cheap evaluators, multiple workers are slower than one worker (1024², 0.037 s vs 0.093 s). This is
   expected, since the stated purpose is expensive sampling.
4. `ConfindConfig.h` version macros are hand-maintained; there is no generated `Version.hpp`.
5. Dead code remains: `Sort`, `Orientation`, `comp_Orient`. So do the 2019 `.vscode` configs and executable bits
   on text files.
6. Warnings: 18 from AppleClang and 4 from GCC, all cosmetic or historical. The `SetScanMode` tautology is a frozen
   historical defect.
7. Zaki inline headers still reference `LogManager::Emit`. Zaki is now the same generation, so the ODR layout
   mismatch is gone.
8. The implementation report labels disclosed limitations as "MATERIAL" while declaring disposition A.
9. The CompactStar migration record is accurate for CONFIND call sites. It omits:
   - the wider Zaki 2.0 plotting-removal footprint (`DataSet::Plot`/`LogLogPlot`/`SemiLog*`/`PlotParam` in EOS
     and Extensions);
   - TaskManager's own static-partition threads;
   - the long-path export change;
   - the M-1/N-1 build constraints.
10. The F19/C20 Plot-invariance fixture is vacuous in the modern build; the historical reference carries the
    evidence.
11. A local tool ref `refs/codex/turn-diffs/checkpoints/...` exists in the primary repo. It is a tree snapshot of
    master plus the untracked reconnaissance report; not a branch or tag, and not pushed.
12. The C15 non-dyadic sampled vs Fast/Normal differences are the intended, documented legacy-rounding
    distinction.

## Key evidence (all reproduced independently)

- **Identities.**
  - CONFIND master 89c5d9b equals the live remote.
  - The candidate is local-only; `ls-remote` shows only master.
  - Zaki c8c6813, CompactStar 812463a and all trees clean.
  - Recovered manifest ed76163c…4083: 18 of 18 files OK.
  - Vendored archives 09ed1a7c… and 3dd4789a… match.
  - No tags; state unchanged at exit.
- **Import commit 4a08260.** 18 of 18 bytes and sizes exact.
- **Reconstruction 6c68b51.** Exactly two changes: `ConvertToDataSet` declaration, definition and include
  removed; `Plot` body replaced with `{}`.
- **Oracle.** Rebuilt from the vendored headers and archives with `-ffp-contract=off`.
  - Two runs byte-identical to `tests/reference/serial.tsv` (c16766f0…).
  - Both export references reproduced.
  - Link map: every CONFIND function and all three `Coord3D` comparators come from vendored `libConfind.a`.
  - The harness defines no comparators.
- **Reconstruction.** Built with fresh canonical Zaki; byte-identical output, 0 FMA. The control build without
  the flag has 15 fused instructions.
- **Candidate test suites.**
  - Debug, Release, ASan+UBSan (instrumentation verified, 0 reports) and TSan (non-vacuous control, 0 reports):
    9/9 each.
  - Worker counts 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 64 and 1000, plain and perturbed: all byte-identical to the
    frozen reference.
  - 500 extra perturbed runs: 1 hash.
  - The perturbation changes the schedule on every run (20/20 distinct).
- **Exceptions.** Throws on the first, middle and last task, on multiple and all tasks, with mixed types, from
  the factory, and on thread-start failure. In every case the exception reaches the caller and no partial
  contour is published; the scenario matrix ran at 1, 2, 3 and 8 workers. Threads settle to baseline and the
  object recovers.
- **Package.** Relocatable (moved twice), no absolute paths. The interface carries `cxx_std_17`, `Zaki::Zaki`
  and `Threads::Threads` only, with no FP flag leak. An independent consumer passes.
- **Benchmark.** Release: 1 worker 0.830–0.849 s, 2 workers 0.427–0.429 s, 4 workers 0.217–0.218 s, 8 workers
  0.1107 s. Checksums are identical, and a 60× skewed-cost load balances at 7.4× on 8 workers.
