// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tap_hold.h"
#include "oled.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = { // Base
        {KC_ESCAPE, KC_1, KC_2, KC_3, KC_4, KC_5},
        {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T},
        {KC_LALT, KC_A, KC_S, CMD_S, KC_F, KC_G},
        {KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B},
        {KC_LGUI, KC_LCTL, KC_BSPC, REP_SYM, KC_LSFT, KC_MUTE},
        {KC_GRAVE, KC_0, KC_9, KC_8, KC_7, KC_6},
        {KC_BSPC, KC_P, KC_O, KC_I, KC_U, KC_Y},
        {KC_QUOTE, KC_SCLN, KC_L, CMD_E, KC_J, KC_H},
        {KC_RSFT, KC_SLASH, KC_DOT, KC_COMMA, KC_M, KC_N},
        {KC_RGUI, KC_RCTL, LT(2, KC_BSPC), KC_ENTER, KC_SPACE, KC_MPLY},
    },
    [1] = { // Symbols
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
        {KC_TRNS, LSFT(KC_1), LSFT(KC_2), LSFT(KC_3), LSFT(KC_4), LSFT(KC_5)},
        {KC_TRNS, KC_BSLS, KC_SLASH, KC_MINUS, LSFT(KC_MINUS), KC_EQUAL},
        {KC_TRNS, KC_GRAVE, LSFT(KC_GRAVE), LSFT(KC_6), LSFT(KC_8), LSFT(KC_EQUAL)},
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
        {KC_TRNS, LSFT(KC_SLASH), LSFT(KC_DOT), LSFT(KC_COMMA), LSFT(KC_BSLS), LSFT(KC_7)},
        {KC_TRNS, KC_P, KC_RBRC, KC_LBRC, LSFT(KC_0), LSFT(KC_9)},
        {KC_TRNS, LSFT(KC_QUOTE), KC_QUOTE, LSFT(KC_P), LSFT(KC_RBRC), LSFT(KC_LBRC)},
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
    },
    [2] = { // Navigation, numbers and F-keys
        {KC_F11, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5},
        {KC_TRNS, KC_PGUP, KC_UP, KC_PGDN, KC_HOME, KC_END},
        {KC_TRNS, KC_LEFT, KC_DOWN, KC_RIGHT, KC_BSPC, KC_DELETE},
        {KC_TRNS, LGUI(KC_Z), LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), LGUI(LSFT(KC_Z))},
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
        {KC_F12, KC_F10, KC_F9, KC_F8, KC_F7, KC_F6},
        {KC_BSPC, KC_EQUAL, KC_MINUS, KC_9, KC_8, KC_7},
        {KC_DELETE, LSFT(KC_8), LSFT(KC_EQUAL), KC_6, KC_5, KC_4},
        {KC_RSFT, KC_DOT, KC_0, KC_3, KC_2, KC_1},
        {KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS},
    },
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLD, MS_WHLU)},
    [1] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [2] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
};
#endif

void keyboard_post_init_user(void) {
    default_layer_set(1UL << 0);
}
