#!/usr/bin/env bash
set -euo pipefail
KEEBS_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
export PATH="$KEEBS_DIR/.venv/bin:$KEEBS_DIR/.tools/arm-gnu-toolchain-13.3.rel1-darwin-arm64-arm-none-eabi/bin:/opt/homebrew/opt/make/libexec/gnubin:$PATH"
export QMK_HOME="$KEEBS_DIR/vial-qmk"
cd "$QMK_HOME"
for source_file in keymap.c rules.mk config.h oled.c; do
    cp "$KEEBS_DIR/keymap/$source_file" "keyboards/sofle/rev1/keymaps/vial/$source_file"
done
qmk compile -kb sofle/rev1 -km vial -e CONVERT_TO=rp2040_ce -j 4 "$@"
mkdir -p "$KEEBS_DIR/dist"
cp "$QMK_HOME/sofle_rev1_vial_rp2040_ce.uf2" "$KEEBS_DIR/dist/sofle-v2.uf2"
"$KEEBS_DIR/.venv/bin/python" "$KEEBS_DIR/tools/verify.py"
"$KEEBS_DIR/.venv/bin/python" "$KEEBS_DIR/tools/render_layouts.py"
