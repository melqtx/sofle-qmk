// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "quantum.h"
#include "repeat_key.h"

// macOS Colemak translates USB D/K into S/E.
#define CMD_S LGUI_T(KC_D)
#define CMD_E RGUI_T(KC_K)
// Repeat is not a basic keycode; replace this layer-tap's tap in the callback.
#define REP_SYM LT(1, KC_NO)

const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = {
    {'L', 'L', 'L', 'L', 'L', 'L'},
    {'L', 'L', 'L', 'L', 'L', 'L'},
    {'L', 'L', 'L', 'L', 'L', 'L'},
    {'L', 'L', 'L', 'L', 'L', 'L'},
    {'*', '*', '*', '*', '*', '*'},
    {'R', 'R', 'R', 'R', 'R', 'R'},
    {'R', 'R', 'R', 'R', 'R', 'R'},
    {'R', 'R', 'R', 'R', 'R', 'R'},
    {'R', 'R', 'R', 'R', 'R', 'R'},
    {'*', '*', '*', '*', '*', '*'},
};

bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    // Remembering the Repeat placeholder would overwrite the key to repeat.
    return keycode != REP_SYM;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == REP_SYM && record->tap.count) {
        repeat_key_invoke(&record->event);
        return false;
    }
    return true;
}
