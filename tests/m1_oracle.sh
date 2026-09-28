#!/bin/bash
# Local Darwin arm64 authority capture. OUTPUT must be scratch, never reference/.
set -euo pipefail
: "${COMPACTSTAR:?Set to the authenticated CompactStar checkout}"
: "${OUTPUT:?Set to a fresh scratch directory}"
root=$(cd "$(dirname "$0")/.." && pwd)
sdk=$(xcrun --show-sdk-path)
dep="$COMPACTSTAR/dependencies"
mkdir -p "$OUTPUT"
test "$(shasum -a 256 "$dep/lib/Confind/Darwin/arm64/libConfind.a" | cut -d ' ' -f1)" = 09ed1a7c43a83b42f64ee8e0bda3b879af970126ee75159a802179a4d0a49eb2
test "$(shasum -a 256 "$dep/lib/Zaki/Darwin/arm64/libZaki.a" | cut -d ' ' -f1)" = 3dd4789a20c35064b3133bb863c54c4f64e7df31c94b83201d68f5463902dfef
for name in characterize differential nodes_oracle; do
  xcrun clang++ -std=c++17 -O0 -ffp-contract=off -isysroot "$sdk" -arch arm64 \
    -I"$dep/include" -I/opt/local/include -I/opt/homebrew/opt/libomp/include \
    -c "$root/tests/m1_$name.cpp" -o "$OUTPUT/$name.o"
  xcrun clang++ -isysroot "$sdk" -arch arm64 "$OUTPUT/$name.o" \
    "$dep/lib/Confind/Darwin/arm64/libConfind.a" "$dep/lib/Zaki/Darwin/arm64/libZaki.a" \
    -L/opt/local/lib -lgsl -lgslcblas -lz -L/opt/homebrew/opt/libomp/lib -lomp \
    -L/Users/keeper/miniforge3/lib -lpython3.12 \
    -Wl,-rpath,/opt/local/lib -Wl,-rpath,/opt/homebrew/opt/libomp/lib \
    -Wl,-rpath,/Users/keeper/miniforge3/lib -o "$OUTPUT/$name"
done
for mode in small stellar; do
  "$OUTPUT/characterize" "$OUTPUT/m1-$mode.tsv" "$mode" 0 0
  "$OUTPUT/characterize" "$OUTPUT/m1-$mode-repeat.tsv" "$mode" 0 0
  cmp "$OUTPUT/m1-$mode.tsv" "$OUTPUT/m1-$mode-repeat.tsv"
  cmp "$OUTPUT/m1-$mode.tsv" "$root/tests/reference/m1-$mode.tsv"
done
for spec in log:1 linear:2; do
  name=${spec%:*}; mask=${spec#*:}
  "$OUTPUT/differential" "$OUTPUT/m1-$name.tsv" 600 "$mask"
  "$OUTPUT/differential" "$OUTPUT/m1-$name-repeat.tsv" 600 "$mask"
  cmp "$OUTPUT/m1-$name.tsv" "$OUTPUT/m1-$name-repeat.tsv"
  cmp "$OUTPUT/m1-$name.tsv" "$root/tests/reference/m1-$name.tsv"
done
"$OUTPUT/nodes_oracle" "$OUTPUT/m1-nodes.tsv"
"$OUTPUT/nodes_oracle" "$OUTPUT/m1-nodes-repeat.tsv"
cmp "$OUTPUT/m1-nodes.tsv" "$OUTPUT/m1-nodes-repeat.tsv"
cmp "$OUTPUT/m1-nodes.tsv" "$root/tests/reference/m1-nodes.tsv"
