# CONFIND 2.0 M-1 correction

## Immutable predeclaration (commit before production changes)

Local Mac arm64, 2026-09-28. Starting candidate
`b243851be6e0acc2e5f12ee6786934603965353a`, branch
`modernize/confind-2.0`, worktree `/Users/keeper/.codex/worktrees/confind-2-0/CONFIND`.
Tracked candidate is clean. Protected HEADs: CONFIND master
`89c5d9b731534e4289d9f686549d9f0ac178e567`, canonical Zaki
`c8c68131b04e9d216673725075bef38df81e6041`, CompactStar
`812463ac9ed374f64ac9cadd500066ab723d3a6c`. No protected edits, merge, tag,
cluster access or release. Existing untracked canonical CONFIND docs are preserved.

The complete independent review was read from
`/private/tmp/claude-501/-Users-keeper-Documents-CompactStar-external-CONFIND/0a6dfb68-387e-4dfe-9a31-6419cc9cede0/scratchpad/CONFIND_2_0_INDEPENDENT_REVIEW.md`.
M-1: AppleClang optimization substitutes __exp10 for historical pow(10,x).
Review evidence: Debug exact; Release 840/3621 differing records in 243/600
log fixtures; 99x150 log-log geometry, 52 levels: 258 raw and 184 curve lines
differ. Linear fixtures exact. Contraction-off does not prevent this rewrite;
broad no-builtin did in review scratch. Existing C22 does not discriminate.
These are review observations pending reproduction, not new qualification claims.

Owner requires optimized builds for expensive neutron-star evaluators. Debug-only
qualification is rejected. Exact ordered raw xyz and curve xy, IEEE bits, hex
floats, level/found metadata and historical arithmetic are frozen; no tolerance.

Plan: one private Source/Internal/HistoricalMath.cpp implementing
HistoricalPow10(x) as libm pow(10.0,x), compiled alone with -fno-builtin on the
supported GNU/Clang family, with noinline and an LTO boundary if required.
All other numerical TUs retain existing optimization and private contraction-off.
Route only historical inverse-log calls: Cell case36, case5, case48,
EvalSimpleFunc, EvalMemFunc and ContourFinder::Evaluate. Preserve log10, operands,
interpolation, indexing, topology, order, Coord3D, RMDuplicates, SortNew and NaN.
Audit complete source call inventory before substitution.

First freeze a small deterministic discriminating log fixture and the review's
TaskManager-shaped 99x150 log-log grid (8e14..5e15, 5e13..5e16), smooth synthetic
surface, 52 levels accumulated from 0.70 by 0.005 below 0.96. Use authenticated
vendored arm64 archives only to generate expected raw/curve/hex/IEEE output and
SHA-256 ledger, reproduce twice. Require oracle PASS, starting Debug PASS and
starting Release FAIL before production edits. Retain a reproducible seeded
600-fixture log and 600-fixture linear differential harness (sampled/Fast/Normal).

Then qualify Debug/Release, 52 existing variants, new fixtures, seeded differential,
1/2/3/4/6/8 workers and >=20 perturbed runs at each multiworker count. Inspect
symbols/disassembly: control exp10, corrected libm pow, no exp10 and zero FMA.
Tests only: synchronized thread-ID concurrency proof, non-dyadic Evaluate node
coordinate contract, cancellation discrimination without fixing a race count.
Fresh Debug/Release/ASan+UBSan/functional TSan, relocated package/consumer flag
checks, and CPU-heavy benchmark at 1/2/4/8 workers. CMP0128 NEW is conditional on
AppleClang/available Clang/GCC compile-mode evidence and numerical neutrality.

STOP on oracle reproduction failure, non-discriminating starting Release,
corrected Debug/Release mismatch, threaded mismatch, class-C semantic change,
need for broad no-builtin, dialect numerical change, post-hoc tolerance, or a
required CompactStar edit. Do not amend this predeclaration after results.

Deferred: N-1 weak Coord3D interposition, prominently for integration qualification;
N-4 repeated sampling, no cache, all-or-nothing exceptions, no in-flight cooperative
interruption, no explicit sane worker cap; N-6 incomplete/uncommitted source-audit
normalizer; N-7 historically non-green intermediate commits including long paths.
N-3 existing invalid-input/non-numerical changes will be documented only.

## Results

To be appended in a separate final documentation commit. The immutable
predeclaration above must remain byte-for-byte unchanged.

## Completed correction and qualification — 2026-09-28

**Disposition A — CONFIND 2.0 M-1 CORRECTION PASS — DEBUG AND OPTIMIZED NUMERICS MATCH HISTORICAL AUTHORITY — DETERMINISTIC THREADING PRESERVED — READY FOR INDEPENDENT RE-REVIEW**

The immutable predeclaration above is preserved in full. This record supersedes
the original report's insufficient optimized-log qualification. Optimized builds
remain supported: expensive neutron-star sampling makes Debug-only restriction
unacceptable to the owner. No fitted tolerance or rebaselining was used.

| # | Item | Result |
|---|---|---|
| 1 | Starting SHA | `b243851be6e0acc2e5f12ee6786934603965353a` |
| 2 | Branch / worktree | `modernize/confind-2.0`; `/Users/keeper/.codex/worktrees/confind-2-0/CONFIND` |
| 3 | Predeclaration SHA | `f2c12a2a5c8bfe2187f0655ce4f0691270e8747e` |
| 4 | Discriminating-test SHA | `dbc51f9959f845c5357d7cbab78d3c935ffa9cd0` |
| 5 | M-1 production-fix SHA | `e4309f3f5205f01a2d565756f5e6bc42e23f16b9` |
| 6 | Threading-test SHA | `ba1fc368b15403323b654bcd821937443ee8a1d6` |
| 7 | CMake-policy SHA | `fe5cf02b9fd3e7b633e68f310a2eb74cb6d1ffe0` |
| 8 | Documentation SHA | The commit containing this completed record (`docs: record confind m1 correction`); exact SHA is returned in the final chat. No self-referential commit hash is embedded. |
| 9 | Final candidate SHA | Same final documentation commit; implementation qualified at `fe5cf02b9fd3e7b633e68f310a2eb74cb6d1ffe0`. Later changes are documentation/evidence and repository-relative ledger paths only. |
| 10 | Historical oracle unchanged? | YES. CONFIND archive `09ed1a7c43a83b42f64ee8e0bda3b879af970126ee75159a802179a4d0a49eb2`; Zaki archive `3dd4789a20c35064b3133bb863c54c4f64e7df31c94b83201d68f5463902dfef`. Original reference bytes unchanged. |
| 11 | Original Release fixture fails? | YES, both fixtures. Small: 2 raw-point differences and curve length 4→6. Stellar: 258 raw and 184 curve-point differences. Original Debug exact. Gate frozen before production edit. |
| 12 | Discriminating fixture | One cell: x=1..100 Log, y=0..1 Linear, scalar log10(x), level `0x1.22fad6cb53501p-10`. Historical 6 raw / 4 curve points; raw point 4 distinguishes pow `0x1.00a7b7380b14p+0` from exp10 `0x1.00a7b7380b13fp+0`. |
| 13 | CompactStar-shaped fixture | 99×150 cells, x=8e14..5e15 Log, y=5e13..5e16 Log, 52 levels accumulated 0.70 to <0.96 by 0.005; smooth synthetic surface. 52,632 raw and 28,720 curve points. Geometry/ladder trace to TaskManager.cpp lines 270–271 and 293; no stellar solve claimed. |
| 14 | Historical pow mechanism | Private out-of-line `CONFIND::detail::HistoricalPow10(double)` returns `std::pow(10.0,x)`; no approximation or arithmetic rearrangement; noinline plus native non-LTO object. |
| 15 | Files using helper | `source/Cell.cpp`: case36/case5/case48 (12 calls), EvalSimpleFunc/EvalMemFunc (8); `source/ContourFinder.cpp`: Evaluate (2). 22 total. |
| 16 | Compile flag scope | Only `source/Internal/HistoricalMath.cpp`: `-fno-builtin;-fno-lto`; existing CONFIND-private `-ffp-contract=off` retained. No optimization-level reduction. |
| 17 | Broad -fno-builtin? | NO. |
| 18 | exp10 in frozen path? | NO in corrected Debug/Release archives; original Release and separately compiled optimized control import `___exp10`. |
| 19 | libm pow reference? | YES, `_pow`; helper disassembly has ARM64_RELOC_BRANCH26 to `_pow`. |
| 20 | FMA count | 0 in each complete corrected AppleClang Debug/Release numerical archive. |
| 21 | Debug vs oracle | EXACT / byte-identical. |
| 22 | Release vs oracle | EXACT / byte-identical. |
| 23 | Randomized linear differences | 0/3,621 records in 600 fixtures in both Debug and Release. |
| 24 | Randomized log differences | 0/3,621 records in 600 fixtures in both Debug and Release. Original Release reproduced 840 differing records / 243 fixtures. |
| 25 | Stellar raw differences | 0/52,632 in Debug and Release. |
| 26 | Stellar curve differences | 0/28,720 in Debug and Release. |
| 27 | Existing 52 variants | PASS; 84 contour records / 5,844 lines. Historical oracle recaptured twice exactly; both historical exports reproduced. |
| 28 | New fixture count | 2 contour fixtures / 53 contour records, plus 1,200 seeded differential fixtures / 7,242 records and 112 pinned node coordinates across two grids. |
| 29 | Actual concurrency test | PASS at requested 2/3/4/6/8. Mutex-protected thread IDs and rendezvous demonstrate ≥2 workers execute evaluators. M14 silently-serial mutation detected. |
| 30 | Evaluate coordinate test | PASS, 56 non-dyadic linear and 56 log nodes, IEEE-identical to vendored GetDeltas plus frozen min+i*delta arithmetic. M15 multiply-before-divide mutation detected. |
| 31 | Cancellation test | PASS at 2/3/4/6/8. Hold already-claimed tasks until throwing thread exits after cancellation store; TLS notification makes cancellation visible. No new unclaimed tasks then start. M13 removal detected. |
| 32 | 1-worker equivalence | EXACT, existing + small + stellar references. |
| 33 | 2-worker equivalence | EXACT, existing + small + stellar references. |
| 34 | 3-worker equivalence | EXACT, existing + small + stellar references. |
| 35 | 4-worker equivalence | EXACT, existing + small + stellar references. |
| 36 | 6-worker equivalence | EXACT, existing + small + stellar references. |
| 37 | 8-worker equivalence | EXACT, existing + small + stellar references. |
| 38 | Perturbed-run hashes | Existing `c16766f02711212f5d7dc502a284ea4275c2bf981b57b2ecfeaec28b7544ad83`; small `1fe565f1d7ac944ec2e41df8953494850c8526b276016bd119c890de1e691af7`; stellar `927acf72b7a5bf8bd50488484dc58176d3cb252cc39460bfc06692f7849e2ee0`. Every recorded run matches; per-run manifests retained. |
| 39 | Debug tests | 29/29 PASS, fresh build. |
| 40 | Release tests | 29/29 PASS, fresh build. Release LTO additionally 29/29 PASS. |
| 41 | ASan | 29/29 PASS with ASan+UBSan; macOS leak detection disabled. CONFIND and external canonical Zaki instrumented. |
| 42 | UBSan | 29/29 PASS with halt_on_error; no reports. |
| 43 | TSan | 29/29 PASS, both libraries instrumented; no product reports. Intentional-race control emits data-race diagnostic (exit 134), proving active detector. |
| 44 | Performance 1 worker(s) | 0.846176375 s; 4993.04888 tasks/s; 4,225 tasks, cost 40,000; checksum `cbe68cd13d73af38`. |
| 45 | Performance 2 worker(s) | 0.426504667 s; 9906.10497 tasks/s; 4,225 tasks, cost 40,000; checksum `cbe68cd13d73af38`. |
| 46 | Performance 4 worker(s) | 0.217563875 s; 19419.5842 tasks/s; 4,225 tasks, cost 40,000; checksum `cbe68cd13d73af38`. |
| 47 | Performance 8 worker(s) | 0.1143265 s; 36955.5615 tasks/s; 4,225 tasks, cost 40,000; checksum `cbe68cd13d73af38`. |
| 48 | N-1 | DEFERRED prominently to CompactStar integration qualification. Weak Coord3D interposition in Debug consumers can change de-duplication. No comparator/Coord3D/RMDuplicates fix here. |
| 49 | N-3 documentation | COMPLETE below and in README: copy/name/work-directory, recursive throwing SetWrkDir, empty levels, longer labels, invalid res/scale, full long paths. |
| 50 | N-4 | DEFERRED: repeated sampling even when levels found; no sample cache; all-or-nothing exceptions; no cooperative in-flight interruption; no explicit sane upper worker bound. Scheduler unchanged. |
| 51 | CMP0128 / C++17 | Minimum CMake 3.22 selects CMP0128 NEW. AppleClang 21, Clang 21.1.8, GCC 15.2 verbose commands all `-std=c++17`, no GNU dialect; all 29 tests pass on each. No frozen output changes. |
| 52 | Package relocation | PASS: fresh Zaki + CONFIND installation moved from package-original to package-relocated; external find_package(CONFIND CONFIG REQUIRED), CONFIND::CONFIND consumer builds/runs. |
| 53 | Consumer flags clean? | YES: no -fno-builtin, -ffp-contract=off or -fno-lto in consumer compile commands or exported interface. |
| 54 | OpenMP absent? | YES, maintained package and consumer; historical oracle only retains old link closure. |
| 55 | ROOT absent? | YES. |
| 56 | Python absent? | YES from maintained library/package/consumer. Historical oracle requires its original Python link closure; local evidence scripts are not runtime dependencies. |
| 57 | Plotting absent? | YES; no restored plotting API or implementation. |
| 58 | Coord3D unchanged? | YES; canonical Zaki untouched. |
| 59 | RMDuplicates unchanged? | YES, Cont2D.cpp byte-identical to starting candidate. |
| 60 | SortNew unchanged? | YES, Cont2D.cpp byte-identical. |
| 61 | Saddle semantics unchanged? | YES; surrounding Cell arithmetic is token-identical after reversing only helper substitutions. |
| 62 | NaN semantics unchanged? | YES; historical fixtures remain exact; no NaN-policy edit. |
| 63 | CompactStar unchanged? | YES, HEAD/master `812463ac9ed374f64ac9cadd500066ab723d3a6c`, tracked clean. |
| 64 | Zaki unchanged? | YES, HEAD/master `c8c68131b04e9d216673725075bef38df81e6041`, tracked clean. |
| 65 | Cluster accessed? | NO. Local Mac only. |
| 66 | Release tag? | NO. No tag, merge or canonical CONFIND edit; master remains `89c5d9b731534e4289d9f686549d9f0ac178e567`. |
| 67 | Branch pushed? | NO; local candidate only. |
| 68 | git diff --check | PASS, staged and unstaged checks; final committed state checked in final chat. |
| 69 | BLOCKING | 0. |
| 70 | MATERIAL | 0 remaining in this correction; M-1 fixed. Existing frozen scientific defects and integration qualification limits remain disclosed, not silently reclassified as corrected. |
| 71 | NONBLOCKING | N-1/N-4/N-6/N-7 deferred; N-2 strengthened; N-3 documented; N-5 corrected. No additional issue found by this bounded qualification. |
| 72 | Final disposition | A — CONFIND 2.0 M-1 CORRECTION PASS — DEBUG AND OPTIMIZED NUMERICS MATCH HISTORICAL AUTHORITY — DETERMINISTIC THREADING PRESERVED — READY FOR INDEPENDENT RE-REVIEW |
| 73 | Exact next action | STOP. Independent re-review of M-1, new log discrimination, Release equivalence, threading tests and C++17 policy, then separate owner acceptance. Do not merge. |

### Numerical evidence and reproducibility

The small one-cell fixture includes two raw differences under original Release;
the unchanged radius-based de-duplication amplifies these to four versus six curve
points. This is evidence for preserving exact coordinates, not permission to
repair de-duplication. The stellar fixture independently reproduces the review's
258 raw / 184 curve differences. Both expected files were produced twice from the
authenticated vendored oracle, then committed before the production fix.
`tests/m1_oracle.sh` reproduces references using only that authority. It also
reproduces 600 log and 600 linear differential fixtures (fixed seed
`0xC0FFEE1234567`, sampled/Fast/Normal), and the 112 node-coordinate records.
The reference manifest is verified from repository root. New manifest paths were
normalized to its existing repository-relative convention; no reference bytes or
hash values changed during final documentation.

The M-1 test harnesses compile at -O0 with contraction-off and use a volatile libm
function pointer for their sample generation, independently of optimized library
code. That keeps identical inputs across the oracle and candidate and prevents a
harness rewrite from concealing library differences. Full small/stellar raw xyz,
ordered curve xy, hex-float and IEEE representations are committed. Differential
records include counts, levels/found flags and order-sensitive bit hashes.

The helper's noinline attribute and source-specific -fno-lto form an optimization
boundary. No optimization is disabled elsewhere. Debug and Release object/archive
inspection proves `_pow` and zero exp10/FMA; the separately compiled -O3 control
imports `___exp10`. An additional Release interprocedural-optimization build passes
all 29 tests. `log10`, operand order and all surrounding arithmetic remain intact.

The original 52-variant stress suite is retained and expanded to 1/2/3/4/6/8:
20 perturbed runs each, in every fresh matrix build. The new small fixture has the
same 120-run schedule per build (its four tasks cap actual workers at four).
Separate large-fixture worker checks cover all six requested worker counts, and
100 additional Release stellar perturbations cover 20 each at 2/3/4/6/8. These
four configurations contain 960 original/small perturbed executions plus 100
stellar executions; policy/LTO checks add further passing runs. Actual concurrency
is independently proven on a 4,225-task workload, including six and eight workers.

The cancellation test deliberately holds every nonthrowing worker inside its
already-claimed task. The throwing worker's TLS destructor runs after the
executor's catch stores cancellation, then synchronizes with the held workers.
Thus their next cancellation check must observe the store. The exact count in
this controlled schedule is not an assertion about arbitrary production races.
The executor itself is unchanged. Separate scratch mutations confirm that the
three previously surviving defects now fail the new test.

The benchmark uses the unchanged deterministic integer-heavy linear-grid evaluator,
so it qualifies scheduler regression, not a real stellar campaign or log-conversion
microbenchmark. All checksums and output artifacts match. The measured 8-worker
speedup is 7.40×; no major regression versus the independent
review's 0.830–0.849 s serial and 0.1107 s at eight workers. No speedup threshold
was used to weaken historical arithmetic.

### Existing compatibility changes (N-3; documentation only)

- Copies and assignments carry the working directory and name.
- `SetWrkDir` recursively creates parent directories and can throw on failure.
- An empty level set throws on the GridVals evaluation path; historically this
  was a no-op.
- `SetContVal(values, labels)` rejects labels longer than the value list.
- `SetGrid` rejects zero resolution and unknown scale names.
- Export paths longer than the historical 149-character payload are preserved,
  so filenames can differ from historical silently truncated paths.

No behavior above was changed by this correction.

### Deferred review issues and limits

**N-1 remains an integration qualification constraint.** Debug consumers that emit
competing weak Zaki Coord3D comparator/XYDist2 definitions under different FP
contraction policy can change radius de-duplication (review F17 curve counts
368→367 and 300→299). No Coord3D, comparator, RMDuplicates or SortNew change is
made here. CompactStar integration must separately qualify its actual consumer
translation units and link; library qualification does not resolve this latent
issue or authorize a semantic fix.

**N-4 remains deferred.** Evaluate re-samples even if all levels are already found,
discards samples, provides all-or-nothing exceptions, cannot cooperatively stop
in-flight expensive evaluations, and caps workers by task count without a sane
explicit upper limit. Current deterministic scheduling and demonstrated scaling
remain acceptable for this bounded qualification. No performance redesign occurred.

**N-6 remains deferred.** The earlier comprehensive frozen-source normalizer was
not committed and its coverage was incomplete. The narrow current-delta check
only proves that reversing the 22 helper substitutions restores the starting
Cell/ContourFinder tokens; it is not a replacement full source audit.

**N-7 remains deferred.** Historical commit cd7de4d does not link; 0e4f6b4 and
297c8a7 fail historical_export under paths exceeding the old truncation limit.
These are historical intermediate-commit limitations, not final-candidate
failures. Prior history has not been rewritten.

This evidence is local Darwin arm64. GCC/Clang checks are local Mac builds, not
Linux or cluster qualification. No full neutron-star or CompactStar integration
campaign is claimed. No new downstream numerical semantics are authorized.

Evidence: [inventory and reproduction](evidence/m1/REPRODUCE.md),
[pre-fix gate](evidence/m1/pre-fix-gate.json),
[original randomized reproduction](evidence/m1/baseline-randomized.json),
[final equivalence](evidence/m1/final-equivalence.json),
[artifact codegen](evidence/m1/final-codegen.json), and
[benchmark](evidence/m1/benchmark.tsv). All evidence files are individually
SHA-256 recorded in `evidence/m1/SHA256SUMS`.
