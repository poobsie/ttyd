#!/bin/bash
# Usage: ./gen_m2c_ctx.sh [extra_headers...]
# Generates ctx.c for m2c decompilation of a specific file.
# Focuses on core types + provided headers.
set -e

INCDIRS="-I include/ -I libs/dolsdk2004/include/ -I libs/"

{
  # Always include base types
  echo '#include "dolphin/types.h"'
  echo '#include "dolphin/os.h"'
  echo '#include "dolphin/gx.h"'
  echo '#include "system.h"'
  echo '#include "memory.h"'
  # Include any extra headers passed as arguments
  for hdr in "$@"; do
    echo "#include \"$hdr\""
  done
} | gcc -E -x c - $INCDIRS 2>/dev/null \
  | grep -v "^#" \
  | grep -v "__declspec" \
  | grep -v "sizeof" \
  > ctx.c

echo "Generated ctx.c: $(wc -l < ctx.c) lines"
