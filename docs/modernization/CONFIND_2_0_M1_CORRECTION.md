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
