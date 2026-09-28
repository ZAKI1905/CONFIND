# CONFIND 2.0 qualification report

**CONFIND 2.0 MODERNIZATION PASS — HISTORICAL SERIAL NUMERICS PRESERVED —
OPENMP REPLACED BY DETERMINISTIC C++ THREADING — PLOTTING REMOVED —
READY FOR INDEPENDENT REVIEW**

Disposition **A**. Local Mac arm64 only, 2026-09-28. This is a candidate, not owner
acceptance, integration, or release. No merge, push, tag or CompactStar migration
was performed. The original Claude report remains unchanged on the primary
checkout and is preserved byte-for-byte in this branch.

## Requested final record

| # | Item | Result |
|---|---|---|
| 1 | Entry CONFIND SHA | `89c5d9b731534e4289d9f686549d9f0ac178e567` |
| 2 | Canonical Zaki SHA | `c8c68131b04e9d216673725075bef38df81e6041` |
| 3 | Recovered manifest | `ed76163c22e0a1f8ba3f71f62f5a56527528850650bbf7127da9c0c908d14083`; all 18 imported file bytes rechecked at source-basis commit |
| 4 | Branch/worktree | `modernize/confind-2.0`; `/Users/keeper/.codex/worktrees/confind-2-0/CONFIND` (fresh desktop-managed location) |
| 5 | Predeclaration SHA | `b51c4c6c6d31e99d09da48f41272473c9ad11944` |
| 6 | Source-basis SHA | `4a082603cb6bfb8ac062ee318a2b0fe8658f82c7` |
| 7 | Reconstruction SHA | `6c68b51c1105da34910c36530b9e2bd6bfeb3023` |
| 8 | Characterization SHA | `5d6ca542ddd25b3804b7ab93ef12ee3777bf1d63` |
| 9 | Zaki-package commit | `cd7de4d473e52800b74d0afee734fc00197cc173` |
| 10 | Threading commit | `0e4f6b4921033e63d877d9fb5b1bb08cc85b8785` |
| 11 | Plotting-removal commit | `297c8a79f8ee2ef0336939ef6db191ad677aed9b` |
| 12 | Safety-fix commit | `47e7525501c1985b0af5e694d350c0c335c5276b` |
| 13 | Package commit | `1c52a792c07d73e6545de12cc5e1f9a9f740be50` |
| 14 | Final branch SHA | The final chat supplies this report's documentation commit SHA; qualified implementation is `1c52a792c07d73e6545de12cc5e1f9a9f740be50`. Resolve `git rev-parse modernize/confind-2.0` for the final documentation tip. |
| 15 | Historical oracle artifact count | 5: one complete serial TSV, two export TSVs, two historical OpenMP evidence TSVs; 3 safe-path reference files |
| 16 | Historical serial equivalence | PASS: 52 fixture variants / 84 contour records / 5,844 lines; exact full raw xyz and curve xy output |
| 17 | Reconstructed source equivalence | PASS byte-for-byte against the oracle; two oracle runs identical; two export files identical |
| 18 | FMA policy | Private `-ffp-contract=off` on AppleClang/Clang/GNU numerical TUs; no global consumer FP flags |
| 19 | FMA count with policy | 0 in reconstructed, modern Debug, and modern Release archives (fmadd/fmsub/fnmadd/fnmsub/fmla/fmls scan) |
| 20 | Serial reference | PASS; SHA-256 `c16766f02711212f5d7dc502a284ea4275c2bf981b57b2ecfeaec28b7544ad83` |
| 21 | Thread architecture | Standard C++17 indexed worker executor for expensive sampling; extraction remains the historical serial scan |
| 22 | Pool lifetime | One bounded pool per Evaluate operation; one caller execution for worker count 1 |
| 23 | Scheduler | Atomic next-task index; variable-cost tasks dynamically distributed |
| 24 | Task indexing | `k=i+(nx+1)*j`; predetermined `values[k]`, then level/j/i/triangle/point extraction order |
| 25 | Evaluator strategy | Caller supplies per-worker factory; independent mutable state required; factories and destruction serialized on caller; wrapper Clone alone is not independence |
| 26 | Exceptions | First observed exception_ptr, cancellation of pending tasks, all threads joined, caller rethrow; thread-start failure also cancels and joins; no partial sampled contours published |
| 27 | Worker API/default | `Evaluate(EvaluatorFactory, ThreadingOptions{worker_count})`; default 1, zero falls back to 1, capped by task count, no global state |
| 28 | 1-thread equality | PASS exact frozen comparison |
| 29 | 2-thread equality | PASS exact frozen comparison |
| 30 | 4-thread equality | PASS exact frozen comparison |
| 31 | Maximum tested equality | PASS at 8 workers on a 10-CPU local Mac |
| 32 | Repeated hashes | Every archived run equals `c16766f02711212f5d7dc502a284ea4275c2bf981b57b2ecfeaec28b7544ad83`; 20 runs/configuration at 1/2/4/8, separately in Debug, Release, and sanitizer builds |
| 33 | Scheduler perturbation | PASS with test-only yields and coordinate-dependent short sleeps |
| 34 | CPU fixture, 1 worker | 0.854682708 s; 4,225 tasks; 4,943.35496 tasks/s |
| 35 | CPU fixture, 2 workers | 0.430656625 s; 9,810.60027 tasks/s |
| 36 | CPU fixture, 4 workers | 0.218832125 s; 19,307.0373 tasks/s |
| 37 | Maximum local timing | 8 workers: 0.110907333 s; 38,094.8661 tasks/s; base cost 40,000 integer-mixing rounds with deterministic variable work; identical checksums/results |
| 38 | OpenMP symbols remaining | NO in maintained archive/installed consumer; historical evidence retained as documentation |
| 39 | OpenMP CMake remaining | NO active discovery/link flags |
| 40 | Python remaining | NO active dependency, source runtime, or installed-consumer linkage; historical oracle recipes mention its old runtime |
| 41 | ROOT remaining | NO active code, dependency, or linkage |
| 42 | Plotting APIs remaining | NO; no compatibility stubs; frozen fixture names/documentation retain historical names |
| 43 | Zaki vendoring remaining | NO; old copied headers/library removed |
| 44 | Zaki public API only | YES; installed package headers and target used |
| 45 | DataColumn private access | NO |
| 46 | Coord3D semantics changed | NO; historical/canonical full definition token-identical; canonical Zaki unchanged |
| 47 | RMDuplicates changed | NO numerical change; diagnostic removal only |
| 48 | SortNew changed | NO valid-path numerical change; diagnostic removal plus authorized empty-contour UB guard |
| 49 | Saddle behavior changed | NO; frozen topology/status/centre and output comparisons pass |
| 50 | Interpolation arithmetic changed | NO; token audit of case36/case5/case48 and centre calculations passes |
| 51 | Grid indexing changed | NO; historical extraction and callable paths retained. New factory path follows the separately specified canonical sampled-node coordinates. |
| 52 | NaN semantics changed | NO; NaN samples remain accepted and historical defective curves preserved |
| 53 | Empty-contour UB fixed | YES; empty found contour converts to empty Curve2D and SortNew returns safely |
| 54 | Ownership UB fixed | YES: array storage uses RAII, copy replaces stale evaluator ownership, move ownership/clear reuse tested, SetMemFunc takes unique_ptr, custom diagnostic allocation removed, self-append made safe |
| 55 | GridVals validation | Null object/value buffer, dimension mismatch, zero/overflow/storage-limit dimensions refused before access; public metadata and raw buffer remain the API boundary |
| 56 | Export retained | YES; two valid historical TSV outputs byte-identical; long paths no longer truncated |
| 57 | SetWrkDir retained | YES; recursive safe directory creation and empty-directory handling |
| 58 | C++ standard | C++17; extensions OFF (verified `-std=c++17`) |
| 59 | Install target | `CONFIND::CONFIND`; CONFINDConfig and SameMajorVersion version package |
| 60 | External consumer | PASS in Debug, Release, sanitizer builds; only find_package and exported target; private FP flags not leaked |
| 61 | ASan | PASS all 9 tests; CONFIND and canonical Zaki compiled with address instrumentation; leak detector disabled on macOS |
| 62 | UBSan | PASS all 9 tests with halt_on_error; no reports |
| 63 | Characterization test count | 52 serial fixture variants, 84 contour records, all C1–C23 categories covered (C16 historical evidence only); 9 final CTest entries per configuration |
| 64 | Full tests | PASS 9/9 Debug, 9/9 Release, 9/9 ASan+UBSan; final matrix includes 240 scheduler-stress executions plus earlier development checks |
| 65 | CompactStar modified | NO |
| 66 | Canonical Zaki modified | NO; builds/installations were out of tree under temporary task storage |
| 67 | Cluster accessed | NO |
| 68 | Version | 2.0.0 candidate |
| 69 | Release tag | NO |
| 70 | git diff --check | PASS; final branch clean after documentation commit |
| 71 | BLOCKING findings | None for the requested bounded qualification |
| 72 | MATERIAL findings | Frozen radius collapse/NaN defects; full TaskManager/stellar campaign gap; new canonical sampling versus legacy callable rounding distinction; independent evaluator-state contract; local arm64 qualification only |
| 73 | NONBLOCKING findings | Benchmark is one synthetic local measurement, not promised scaling. Future GridVals accessor improvement remains separate. Intermediate package-only commit retained old OpenMP imports until the next threading commit. |
| 74 | Disposition | A — exact disposition at the top of this report |
| 75 | Exact next action | Independently review this branch and its frozen evidence before owner acceptance. STOP here; do not merge, tag, migrate CompactStar, or run a cluster campaign. |

## Evidence and limits

The authority and reproduction details are in [CHARACTERIZATION](evidence/CHARACTERIZATION.md)
and the unchanged reconnaissance report. Frozen oracle artifacts are enumerated by
`tests/reference/SHA256SUMS`; modern evidence is enumerated by
`evidence/QUALIFICATION_SHA256SUMS`. No modern result was used to change a historical
target, and no comparison uses a fitted tolerance.

The 14-entry source audit compares whitespace/comment/diagnostic-stripped function
bodies with reconstruction commit 6c68b51. It covers the vertex/triangle classifiers,
cell origin/centre/vertex/triangle/status/interpolation methods, RMDuplicates,
SortNew, FindContourFast and FindNextContours. The only numerical-boundary exception
is the preauthorized empty guard in SortNew. Diagnostic-only snprintf buffers are
excluded for FindNextContours. The actual arithmetic/topology and valid-path
ordering tokens match. The complete Coord3D definition also matches between
historical and canonical Zaki headers; no Zaki code was copied into CONFIND.

The scalar factory's own results must be independent of task order and worker ID.
The library cannot make a nondeterministic external stellar solver deterministic.
A copied wrapper may still share a mutable target; ownership independence is the
factory caller's explicit obligation. Existing SetFunc/SetMemFunc Fast/Normal calls
remain serial, including non-dyadic rounding. The new factory path is a separate
indexed sampled-grid interface. Contour extraction is intentionally serial.

C23 contains bounded synthetic consumer operations; real TaskManager, stellar/EOS,
Linux, and cluster qualification are not claimed. The historical oracle inherits
vendored diagnostic layout defects and the callable array-deletion defect. Empty
historical conversion and known overflowing threaded dimensions were not invoked.
The owner explicitly authorized preserving the numerically defective radius/NaN
semantics; these are not quietly repaired or used as acceptance exclusions.

The initial package-consumption build compiled the library but failed test linkage
on inherited OpenMP imports. That was resolved by removal/replacement in the next
commit, after which all frozen comparisons passed. No failed numerical comparison
was bypassed or rebaselined. The implementation tested in the final matrix is the
package commit recorded above; subsequent changes are documentation/evidence only.

## Exit authentication

CONFIND primary HEAD/master/origin/master and live master still equal the entry
SHA. Its only untracked file is the owner's reconnaissance report, unchanged at
SHA-256 `0b1b0f17841283c4d7de9ab440559c039299c10043bd9d1335febb7e4dc0882e`.
Canonical Zaki HEAD/master/origin/master/live master still equal c8c68131 and its
checkout is clean. CompactStar HEAD/master/origin/master still equal 812463ac and
its checkout is clean. Both historical arm64 archive hashes still match entry.
All five oracle ledger entries and all modern qualification evidence hashes verify.
There are no release tags. The final worktree is committed and clean.
