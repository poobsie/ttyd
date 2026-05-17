#!/bin/bash
# Usage: ./diff.sh <src_path> [unit_name]
# Example: ./diff.sh src/effect/eff_hit.c
# Example: ./diff.sh src/effect/eff_hit.c MarioSt/effect/eff_hit
#
# Compiles the file and shows match % for each function.
# Does NOT relink the full ELF — fast iteration loop.

set -e

SRC="$1"
if [ -z "$SRC" ]; then
    echo "Usage: $0 <src_path> [unit_name]"
    echo "  e.g. $0 src/effect/eff_hit.c"
    exit 1
fi

# Derive the .o path and unit name from the src path if not provided
# src path format: src/effect/eff_hit.c
# .o format:       build/G8MJ01/src/effect/eff_hit.o
# unit name:       MarioSt/effect/eff_hit  (strip leading "src/", strip ".c")
NOEXT="${SRC%.c}"
OBJ="build/G8MJ01/${NOEXT}.o"
STRIPPED="${NOEXT#src/}"
UNIT="${2:-MarioSt/${STRIPPED}}"

echo "==> Compiling $SRC..."
ninja "$OBJ" 2>&1

echo ""
echo "==> Diffing unit: $UNIT"
build/tools/objdiff-cli diff -p . -u "$UNIT" -o - --format json 2>/dev/null | python3 -c "
import json, sys
d = json.load(sys.stdin)
all_match = True
for sec in d['right']['sections']:
    for sym in sec.get('symbols', []):
        name = (sym.get('symbol') or {}).get('name', '?')
        pct = sym.get('match_percent', 0)
        bar = '#' * int(pct / 5) + '-' * (20 - int(pct / 5))
        status = 'MATCH' if pct == 100.0 else '     '
        print(f'  {status}  [{bar}] {pct:6.1f}%  {name}')
        if pct != 100.0:
            all_match = False
if all_match:
    print()
    print('  *** All functions match! Mark as Matching in configure.py and run ninja. ***')
"
