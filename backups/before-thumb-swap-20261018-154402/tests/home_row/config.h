// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "test_common.h"
#undef MATRIX_ROWS
#undef MATRIX_COLS
#define MATRIX_ROWS 10
// QMK's native dummy keymap requires ten columns; tests use Sofle columns 0–5.
#define MATRIX_COLS 10
#include "../../../keymap/config.h"
