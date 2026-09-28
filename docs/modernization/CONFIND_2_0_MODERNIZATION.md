# CONFIND 2.0 modernization

Status: **PREDECLARED / OWNER-AUTHORIZED IMPLEMENTATION**

Date: 2026-09-28. Local Mac arm64 only. Owner request: historical reconstruction,
characterization, deterministic C++17 threading, plotting removal and packaging.
The owner corrected the requested model to Astra 6. No merge, tag, CompactStar
migration, canonical Zaki edit, or cluster access is authorized.

## Authenticated entry and provenance

- CONFIND HEAD/master/origin/master/live master: `89c5d9b731534e4289d9f686549d9f0ac178e567`.
- Canonical Zaki HEAD/master/origin/master/live master: `c8c68131b04e9d216673725075bef38df81e6041`.
- CompactStar HEAD/master/origin/master: `812463ac9ed374f64ac9cadd500066ab723d3a6c`.
- All three tracked trees were clean. After initial authentication Claude added
  only `docs/modernization/CONFIND_RECONNAISSANCE_PREFLIGHT.md`, untracked on CONFIND
  master. The owner's follow-up explicitly identified that expected document.
  Its exact bytes are preserved here and left untouched in the primary checkout.
  SHA-256: `0b1b0f17841283c4d7de9ab440559c039299c10043bd9d1335febb7e4dc0882e`.
- Recovered non-Git source manifest: `ed76163c22e0a1f8ba3f71f62f5a56527528850650bbf7127da9c0c908d14083`.
  All 18 paths, sizes and content hashes verified, with no extra files.
- Historical arm64 CONFIND archive SHA-256: `09ed1a7c43a83b42f64ee8e0bda3b879af970126ee75159a802179a4d0a49eb2`.
- Historical arm64 Zaki archive SHA-256: `3dd4789a20c35064b3133bb863c54c4f64e7df31c94b83201d68f5463902dfef`.
- Branch: `modernize/confind-2.0`, exact entry base above; fresh managed worktree
  `/Users/keeper/.codex/worktrees/confind-2-0/CONFIND`. The desktop's managed
  worktree location substitutes for the owner's preferred worktree path.

The recovered snapshot is the owner-selected source basis, not an attributable
historical Git revision. Import its 18 files byte-for-byte, preserving their
logical paths, and remove the obsolete Git implementation from the maintained
layout rather than mixing implementations. Record the import as **SOURCE BASIS —
NOT YET HISTORICAL ARM64 RECONSTRUCTION**. Preserve snapshot and provenance files
under docs/provenance. Historical Git ancestry remains available.

Authority order: (1) authenticated CompactStar arm64 archives and consumed headers;
(2) reconstructed source only after exact serial oracle equivalence; (3) modern
source only after the frozen comparison. The reconnaissance report is supporting
evidence, not a replacement oracle. Its probe hashes have been checked.

Reconstruction changes ONLY removal of Cont2D::ConvertToDataSet and its declaration,
and replacement of the ContourFinder::Plot body by an empty body. Retain Plot's
public declaration until characterization. Build against a fresh out-of-tree
canonical Zaki 2.0 build. No Python/ROOT plotting is executed. Historical oracle
link dependencies are isolated from the maintained package.

## Frozen numerical and execution contracts

Apply `-ffp-contract=off` to CONFIND numerical translation units on AppleClang,
Clang and GNU, without imposing global flags on consumers. Historical arm64
uses separate multiply/add operations. Check emitted fused instructions.

Freeze CONREC triangle/case topology, centre mean, saddle handling, comparisons,
edge interpolation expression/order, cell origins, log-axis arithmetic, Coord3D
ordering and exact equality, RMDuplicates, SortNew, nearest-neighbour chaining,
start-point rule, raw/curve ordering, grid indexing, default values, NaN and
case-(g) semantics. No tolerance is fitted after observing a failure.

Remove OpenMP completely from maintained code/build. Replace it with bounded
standard C++17 indexed execution: workers created once per operation, atomic next
index for dynamic load balancing, predetermined result slots, deterministic join,
first observed exception capture and cancellation followed by rethrow on caller.
No worker appends to shared contour output. Sampling is the preferred expensive
parallel boundary; canonical node index is k=i+(nx+1)*j. Extraction uses the frozen
serial scan after all samples complete. Explicit per-worker evaluator factories
must provide independent state; wrapper cloning alone is not thread-safety proof.
Factory construction and destruction are serialized on the caller to avoid the
historical MemFuncWrapper registry race. A shared evaluator is allowed only under
an explicit caller-declared thread-safe contract. Default worker count is 1;
hardware concurrency is only an optional caller hint. Worker count 1 is reference.

Remove Plot/SetPlot*, colour, legend, dimensions/ranges, plotting adapters,
banners and hot-path diagnostics. Preserve numerical export and SetWrkDir.
Consume canonical Zaki through find_package(Zaki 2.0 CONFIG REQUIRED), Zaki::Zaki;
DataColumn fields remain private. No new Zaki API or copied Zaki code.

## Predeclared characterization and qualification

Before cleanup freeze machine-readable full raw x/y/z sequences, multiplicity,
level order, found flags, curve sequence, raw/exact-distinct/radius-class counts.
Use hex floats and explicit bit encodings, with no approximate comparator.

C1 constant; C2 vertical; C3 centre-on-level; C4 horizontal; C5 diagonal/vertex;
C6 origin and off-centre circles; C7 disconnected; C8 boundary; C9 vertex;
C10 grid-edge; C11 saddles and mean-decider levels; C12 nearby branches;
C13 synthetic exact/ulp/radius stress; C14 reversed axes; C15 sampled/callable;
C16 historical OpenMP evidence only; C17 NaN; C18 missing/default sample;
C19 CompactStar-scale; C20 Plot no-op/raw invariance; C21 level order/found flags;
C22 x/y/both log axes; C23 bounded CompactStar consumers: raw critical sequence,
maximum excluding last point, mass_curve[0], intersection and Bisect ordering/index.
Full stellar TaskManager campaigns are outside scope; record that remaining gap.

Historical undefined operations (empty found curve conversion, mismatched array
delete and unsafe threaded dimensions) are HISTORICAL DEFECT, not correctness
expectations. Do not invoke empty conversion in the oracle. NaN remains separately
identified historical numerical defect and is preserved, not rejected.

Gate: all safe reconstructed serial fixtures must equal the historical oracle
byte-for-byte under contraction-off before ANY modernization. Freeze references
before comparing reconstruction, with SHA-256 ledger. Then require unchanged
references after each semantic-risk-bearing change.

Final tests: serial, 1/2/4/min(8,hardware hint) workers; at least 20 repetitions per
representative threaded configuration with yields/deterministic scheduling delays;
identical hashes, sampled/callable equivalence, open/closed/saddle/radius/log/multi-
level/large grids; safe empty conversion, invalid grid dimensions/null, copy/move,
resource ownership, evaluator exceptions and export bytes; external installed
find_package consumer. ASan/UBSan where supported. Synthetic deterministic CPU
work with configurable cost: report time/tasks/workers/throughput, not a speedup gate.

## Stop conditions and audit history

STOP on unexpected identity/source drift; reconstruction mismatch after contraction
off; threaded/serial or repeat mismatch; requirement to alter frozen semantics;
missing public Zaki API; valid-path changes from a UB fix or plotting removal;
need to modify CompactStar; proposed NaN rejection; or post-failure tolerance choice.

Logical commits: this predeclaration; byte-exact source import; restricted
reconstruction; oracle harness/frozen characterization; Zaki package consumption;
deterministic threading; plotting/presentation removal; ownership/input safety;
CONFIND package; final evidence/docs. Do not squash referenced SHAs.

Candidate version 2.0.0. Desired final disposition A requires completed independent-
review-ready evidence; acceptance/integration are later owner actions.

## Completed implementation record

The predeclaration above remains the chronological contract. Implementation and
qualification are complete with disposition A; see CONFIND_2_0_REPORT.md for all
75 requested fields, commit identities, preserved defects, timing and test evidence.
The final implementation package commit is 1c52a792c07d73e6545de12cc5e1f9a9f740be50.
Independent review and owner acceptance are the next boundary; no merge or tag.
