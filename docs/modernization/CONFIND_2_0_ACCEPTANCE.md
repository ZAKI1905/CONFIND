# CONFIND 2.0 owner acceptance

Status: **OWNER-ACCEPTED**

Integration status: **CANONICAL INTEGRATION AUTHORIZED**

Date: 2026-09-28. Human owner acceptance applies to the exact candidate below.
This record establishes the current acceptance boundary; the earlier
predeclaration, qualification reports and README retain their chronological
pre-acceptance wording. This acceptance does not authorize a release tag or
consumer migration.

## Accepted identities and review

| Identity | SHA |
|---|---|
| Canonical entry master (local / origin / live) | `89c5d9b731534e4289d9f686549d9f0ac178e567` |
| Accepted candidate | `8c6731ee0ff8cb6e63303af8118607c12a582f51` |
| Original modernization candidate | `b243851be6e0acc2e5f12ee6786934603965353a` |
| Canonical Zaki (unchanged) | `c8c68131b04e9d216673725075bef38df81e6041` |
| CompactStar (unchanged) | `812463ac9ed374f64ac9cadd500066ab723d3a6c` |

Accepted branch: `modernize/confind-2.0`. Entry candidate HEAD equals the accepted
SHA, its worktree is clean, and it is local-only with no upstream. Entry master
is the exact merge base and an ancestor of the accepted candidate. The primary
checkout's previously documented untracked reconnaissance report is byte-identical
to the copy already committed in the accepted candidate; its bytes are preserved.

The M-1 correction history is accepted without rewriting:

1. `f2c12a2a5c8bfe2187f0655ce4f0691270e8747e` — correction predeclaration.
2. `dbc51f9959f845c5357d7cbab78d3c935ffa9cd0` — discriminating pre-fix tests.
3. `e4309f3f5205f01a2d565756f5e6bc42e23f16b9` — historical pow production fix.
4. `ba1fc368b15403323b654bcd821937443ee8a1d6` — threading test discrimination.
5. `fe5cf02b9fd3e7b633e68f310a2eb74cb6d1ffe0` — strict C++17 build policy.
6. `8c6731ee0ff8cb6e63303af8118607c12a582f51` — correction documentation and
   accepted candidate.

The owner accepts the focused independent re-review at this candidate:

- **BLOCKING: 0**.
- **MATERIAL: 0**.
- **Disposition B — READY FOR OWNER ACCEPTANCE WITH NONBLOCKING FINDINGS**.

The complete focused report was read locally from
`/private/tmp/claude-501/-Users-keeper-Documents-CompactStar-external-CONFIND/3fd3589b-47e7-4de2-afe8-dd50e2be652e/scratchpad/CONFIND_2_0_FOCUSED_REREVIEW.md`.
Its SHA-256 is
`19d57d49ab8c460694869c2e8ea9bdd6c9757ce05696665c371b39c26090a864`.
The report's full disposition is B — CONFIND 2.0 FOCUSED RE-REVIEW PASS WITH
NONBLOCKING FINDINGS — READY FOR OWNER ACCEPTANCE WITH CAVEATS. The local scratch
path is provenance, not a maintained package dependency. The owner-supplied
acceptance instruction is the authority for this acceptance.

## Accepted architecture and numerical contract

The owner accepts:

- Recovered 2023 source basis and historical arm64 reconstruction.
- Historical serial CONREC numerics and historical Coord3D/radius
  de-duplication semantics preserved.
- Historical saddle, NaN, grid-index, raw/curve-order and exact-comparison
  semantics preserved; no numerical semantics are repaired by acceptance.
- OpenMP completely removed from the maintained library and build.
- Deterministic C++17 multithreading replacing OpenMP, with dynamically
  scheduled indexed work and deterministic result assembly.
- Worker-local evaluator construction and explicit exception propagation.
- ROOT, Python, plotting and vendored Zaki removed from the maintained library,
  package and consumer dependency closure.
- Canonical Zaki consumed through `Zaki::Zaki`; installable CONFIND consumed
  through `CONFIND::CONFIND`.
- `ExportContour` retained.
- CONFIND **2.0.0** accepted as the code line and intended next major version.
- Private `-ffp-contract=off` accepted for the numerical translation units.
- Optimized historical `pow(10.0,x)` preservation through private
  `HistoricalMath` accepted, retaining the qualified noinline and source-local
  `-fno-builtin;-fno-lto` boundary without leaking private flags to consumers.

## Accepted numerical and threading evidence

The owner accepts the existing qualification and focused independent review:

- Debug equals the historical oracle exactly.
- Release equals the historical oracle exactly.
- Release+LTO equals the historical oracle exactly.
- Randomized linear differential differences: **0**.
- Randomized log differential differences: **0**.
- CompactStar-shaped 99×150 log-log fixture: raw differences **0**, curve
  differences **0**. This is a synthetic contour fixture, not a stellar solve.
- **1, 2, 3, 4, 6 and 8 workers** reproduce frozen results.
- Perturbed scheduling remains deterministic.
- ASan, UBSan and TSan pass.
- The TSan deliberate-race control is non-vacuous.
- Package relocation passes and consumer flags remain clean.

These are accepted existing results, not newly rerun numerical campaigns.
Acceptance integrity checks authenticate identities, ancestry, artifact hashes,
the documentation delta and final byte preservation. No oracle regeneration or
qualification reinterpretation is authorized or performed here.

Evidence read in full: [modernization predeclaration](CONFIND_2_0_MODERNIZATION.md),
[qualification report](CONFIND_2_0_REPORT.md),
[M-1 correction](CONFIND_2_0_M1_CORRECTION.md), the repository README, and the
focused independent re-review identified above.

## Accepted deferred nonblocking findings

### R-N1 — test-only regression gap

Status: **ACCEPTED / DEFERRED TEST-ONLY FOLLOW-UP**.

The maintained implementation is correct at all six historical inverse-log
call-site groups: `Cell::case36`, `case5`, `case48`, `EvalSimpleFunc`,
`EvalMemFunc`, and `ContourFinder::Evaluate`. The committed regression suite
directly discriminates only the `case36` group. Independent review separately
verified all six groups against the oracle and showed that one-site regressions
are detectable with its per-site probe. This finding limits future regression
protection and does not reopen M-1. A separately authorized test-only follow-up
may add per-site historical discrimination or an artifact gate. **No production
change or test change is authorized by this acceptance task.**

### N-1 — weak-symbol Coord3D interposition

Status: **ACCEPTED / DEFERRED TO COMPACTSTAR INTEGRATION QUALIFICATION**.

A consumer translation unit may alter weak Coord3D comparator/XYDist2 resolution
under some build configurations and thereby change historical radius
de-duplication. Qualify the actual consumer translation units, floating-point
policy and link resolution in the future CompactStar integration. Do not change
`Coord3D`, `operator<`, `XYDist2`, `RMDuplicates` or `SortNew` here.

### N-4 — Evaluate operational limitations

Status: **ACCEPTED / DEFERRED FUTURE PERFORMANCE AND OPERATIONAL FOLLOW-UP**.

- Requested nodes are resampled, including when contour levels are already found.
- There is no sample cache.
- Exceptions have all-or-nothing semantics.
- Already-running work is allowed to complete; there is no cooperative
  interruption of in-flight evaluations.
- There is no small explicit upper worker-count ceiling beyond the sample count.

These limitations are future performance/operational improvements and are not
CONFIND 2.0 acceptance blockers. Deterministic execution remains accepted.

### N-6 / N-7 — historical evidence and intermediate commits

Status: **ACCEPTED / DEFERRED**, as documented in the correction record.

N-6: the earlier full frozen-source audit normalizer is uncommitted and its
coverage is incomplete; the narrow helper-substitution audit is not a substitute
for a full source audit. Independent review found no corresponding product defect.

N-7: historical intermediate commits are not all green. `cd7de4d` does not link;
`0e4f6b4` and `297c8a7` fail historical export with sufficiently long paths. The
predeclared `dbc51f9` pre-fix gate intentionally fails Release. These limitations
do not describe the accepted final candidate, and history remains unchanged.

## Mandatory future CompactStar qualification

The separately governed migration and TaskManager stellar-equivalence work must
carry the following requirements before scientific authority is granted:

1. Qualify Coord3D weak-symbol/interposition behavior at the actual consumer/link
   boundary (N-1), preserving the historical comparator and de-duplication contract
   until a separately authorized scientific decision permits change.
2. Characterize and resolve optimized log-axis `pow`/`exp10` behavior **outside
   CONFIND**. Independent review found that canonical Zaki Release may use
   `__exp10` in `Axis::operator[]`, affecting paths including
   `GridVals_2D::Interpolate`, while historical vendored Zaki used `pow`.
   Compare these log-axis paths against the historical authority.
3. Characterize CompactStar TaskManager's own `pow(10,...)` sites, including log
   task-range construction, and its use of Zaki interpolation. CONFIND's private
   HistoricalMath correction does not qualify these external paths.
4. Preserve `FindCriticalCurve` raw contour ordering and point sequence,
   `mass_curve[0]`, and `Bisect` index behavior, including intersection/cut order.
5. Require full TaskManager stellar-output regression and equivalence evidence
   on the declared sequence/EOS inputs. Bounded C23 and the synthetic 99×150
   fixture do not establish TaskManager stellar equivalence.
6. Remove old plotting calls, replace vendored Zaki/CONFIND and binaries with
   external `Zaki::Zaki` / `CONFIND::CONFIND` package targets, and carry the wider
   Zaki 2.0 plotting-removal/Python cleanup into the governed migration.
7. Keep Linux and EKU cluster qualification later and separate. Local Mac results
   do not establish those authorities or cross-platform bitwise equivalence.

These requirements are also carried by the documentation-only
[CompactStar migration record](COMPACTSTAR_MIGRATION.md). No CompactStar or Zaki
implementation is edited by this task.

## Scope, release and authorized integration

This acceptance establishes CONFIND 2.0 library-level authority on the qualified
local Mac configuration only. It does **not** establish:

- CompactStar TaskManager stellar equivalence.
- Linux qualification.
- EKU cluster qualification.
- Cross-platform bitwise authority.

**Next version: 2.0.0. Release tag: NOT AUTHORIZED.**

The only authorized descendant changes from the accepted candidate are this
acceptance document and `docs/modernization/COMPACTSTAR_MIGRATION.md`. Production,
headers, tests, numerical references, CMake/package implementation, threading,
Coord3D, RMDuplicates, SortNew, NaN and saddle behavior remain frozen at the
accepted candidate. README is unchanged.

The owner authorizes committing this documentation with the exact message
`docs: accept confind 2.0 modernization`, pushing `modernize/confind-2.0`
non-force, verifying local/origin/live equality, and fast-forwarding master only
from the authenticated entry SHA to that acceptance commit. Master must then be
pushed non-force and its local/origin/live equality and product/build/test byte
identity with the accepted candidate verified. No merge commit, squash,
cherry-pick, rebase, force push or release tag is authorized.

After canonical integration, **STOP and return to the owner**. The exact next
action is a separate CompactStar migration and TaskManager stellar-equivalence
predeclaration carrying every requirement above. Do not begin it automatically.
