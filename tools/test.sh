#!/usr/bin/env bash
set -euo pipefail
KEEBS_DIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
export PATH="$KEEBS_DIR/.venv/bin:/opt/homebrew/opt/make/libexec/gnubin:$PATH"
cd "$KEEBS_DIR/vial-qmk"
if ! test -f lib/googletest/googletest/src/gtest-all.cc; then
    git submodule update --init --depth 1 lib/googletest
fi
mkdir -p tests/melqtx_home_row
cp "$KEEBS_DIR"/tests/home_row/* tests/melqtx_home_row/
make test:melqtx_home_row -j4
