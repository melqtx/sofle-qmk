// SPDX-License-Identifier: GPL-2.0-or-later
#include "test_common.hpp"
#include "test_fixture.hpp"
#include "test_keymap_key.hpp"
#include "repeat_key.h"
#include "action_tapping.h"

using testing::InSequence;
using testing::_;
class SofleHomeRow : public TestFixture {};

TEST_F(SofleHomeRow, TapLettersWithoutCommand) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey s(0, 3, 2, LGUI_T(KC_D)), e(0, 3, 7, RGUI_T(KC_K));
    set_keymap({s, e});
    EXPECT_REPORT(driver, (KC_D)); EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_K)); EXPECT_EMPTY_REPORT(driver);
    tap_keys(s, e);
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, LeftCommandOppositeHandNestedChord) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey s(0, 3, 2, LGUI_T(KC_D)), l(0, 2, 7, KC_L);
    set_keymap({s, l});
    s.press(); run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_LGUI));
    EXPECT_REPORT(driver, (KC_LGUI, KC_L));
    EXPECT_REPORT(driver, (KC_LGUI));
    tap_key(l);
    EXPECT_EMPTY_REPORT(driver);
    s.release(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, RightCommandOppositeHandNestedChord) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey e(0, 3, 7, RGUI_T(KC_K)), c(0, 3, 3, KC_C);
    set_keymap({e, c});
    e.press(); run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_RGUI));
    EXPECT_REPORT(driver, (KC_RGUI, KC_C));
    EXPECT_REPORT(driver, (KC_RGUI));
    tap_key(c);
    EXPECT_EMPTY_REPORT(driver);
    e.release(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, SameHandRollStaysLetters) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey s(0, 3, 2, LGUI_T(KC_D)), a(0, 1, 2, KC_A);
    set_keymap({s, a});
    EXPECT_REPORT(driver, (KC_D));
    EXPECT_REPORT(driver, (KC_D, KC_A));
    EXPECT_REPORT(driver, (KC_A)); EXPECT_EMPTY_REPORT(driver);
    s.press(); run_one_scan_loop();
    a.press(); run_one_scan_loop();
    s.release(); run_one_scan_loop();
    a.release(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, OppositeHandRollStaysLetters) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey s(0, 3, 2, LGUI_T(KC_D)), l(0, 2, 7, KC_L);
    set_keymap({s, l});
    EXPECT_REPORT(driver, (KC_D));
    EXPECT_REPORT(driver, (KC_D, KC_L));
    EXPECT_REPORT(driver, (KC_L)); EXPECT_EMPTY_REPORT(driver);
    s.press(); run_one_scan_loop();
    l.press(); run_one_scan_loop();
    s.release(); run_one_scan_loop();
    l.release(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, FastTypingProtectsCommandLetter) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey a(0, 1, 2, KC_A), e(0, 3, 7, RGUI_T(KC_K)), c(0, 3, 3, KC_C);
    set_keymap({a, e, c});
    EXPECT_REPORT(driver, (KC_A)); EXPECT_EMPTY_REPORT(driver);
    tap_key(a); idle_for(20);
    EXPECT_REPORT(driver, (KC_K));
    e.press(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
    EXPECT_REPORT(driver, (KC_K, KC_C));
    EXPECT_REPORT(driver, (KC_K));
    tap_key(c);
    EXPECT_EMPTY_REPORT(driver);
    e.release(); run_one_scan_loop();
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, RepeatTapsNeverRememberTheirPlaceholder) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey o(0, 2, 6, KC_O), repeat(0, 3, 4, LT(1, KC_NO));
    set_keymap({o, repeat});
    EXPECT_REPORT(driver, (KC_O)); EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_O)); EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_O)); EXPECT_EMPTY_REPORT(driver);
    tap_keys(o, repeat, repeat);
    EXPECT_EQ(get_last_keycode(), KC_O);
    expect_layer_state(0);
}

TEST_F(SofleHomeRow, RepeatHomeRowTapProducesLetterWithoutCommand) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey s(0, 3, 2, LGUI_T(KC_D)), repeat(0, 3, 4, LT(1, KC_NO));
    set_keymap({s, repeat});
    EXPECT_REPORT(driver, (KC_D)); EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_D)); EXPECT_EMPTY_REPORT(driver);
    tap_keys(s, repeat);
    EXPECT_EQ(get_mods(), 0);
    EXPECT_EQ(get_last_keycode(), LGUI_T(KC_D));
}

TEST_F(SofleHomeRow, SymbolsQuickChordWorksOnEitherHand) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey repeat(0, 3, 4, LT(1, KC_NO));
    KeymapKey left(0, 1, 1, KC_Q), right(0, 5, 6, KC_Y);
    KeymapKey bang(1, 1, 1, KC_EXLM), amp(1, 5, 6, KC_AMPR);
    set_keymap({repeat, left, right, bang, amp});
    for (auto pair : {std::make_pair(left, KC_1), std::make_pair(right, KC_7)}) {
        idle_for(300);
        repeat.press(); run_one_scan_loop();
        EXPECT_REPORT(driver, (KC_LSFT));
        EXPECT_REPORT(driver, (KC_LSFT, pair.second));
        EXPECT_REPORT(driver, (KC_LSFT));
        EXPECT_EMPTY_REPORT(driver);
        tap_key(pair.first);
        expect_layer_state(1);
        repeat.release(); run_one_scan_loop();
        expect_layer_state(0);
    }
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, SymbolsHoldAfterRepeatTapAndNavPriority) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey o(0, 2, 6, KC_O), repeat(0, 3, 4, LT(1, KC_NO)), nav(0, 2, 9, LT(2, KC_BSPC));
    KeymapKey nav_sym(1, 2, 9, KC_TRNS), repeat_sym(1, 3, 4, KC_TRNS), repeat_nav(2, 3, 4, KC_TRNS), nav_nav(2, 2, 9, KC_TRNS);
    set_keymap({o, repeat, nav, nav_sym, repeat_sym, repeat_nav, nav_nav});
    EXPECT_REPORT(driver, (KC_O)); EXPECT_EMPTY_REPORT(driver);
    EXPECT_REPORT(driver, (KC_O)); EXPECT_EMPTY_REPORT(driver);
    tap_keys(o, repeat);
    repeat.press(); run_one_scan_loop(); idle_for(251);
    expect_layer_state(1);
    nav.press(); run_one_scan_loop(); idle_for(251);
    EXPECT_EQ(get_highest_layer(layer_state), 2);
    nav.release(); run_one_scan_loop();
    expect_layer_state(1);
    repeat.release(); run_one_scan_loop();
    expect_layer_state(0);
    EXPECT_EQ(get_mods(), 0);
}

TEST_F(SofleHomeRow, NavigationTapDeletesOnceAndDoubleTapDeletesTwice) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey nav(0, 2, 9, LT(2, KC_BSPC));
    set_keymap({nav});
    for (int i = 0; i < 3; ++i) {
        EXPECT_REPORT(driver, (KC_BSPC)); EXPECT_EMPTY_REPORT(driver);
        tap_key(nav);
        expect_layer_state(0);
    }
}

TEST_F(SofleHomeRow, NavigationHoldAfterTypingDoesNotDeleteOnRelease) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey a(0, 1, 2, KC_A), nav(0, 2, 9, LT(2, KC_BSPC));
    set_keymap({a, nav});
    EXPECT_REPORT(driver, (KC_A)); EXPECT_EMPTY_REPORT(driver);
    tap_key(a);
    EXPECT_NO_REPORT(driver);
    nav.press(); run_one_scan_loop(); idle_for(251);
    expect_layer_state(2);
    nav.release(); run_one_scan_loop();
    expect_layer_state(0);
}

TEST_F(SofleHomeRow, NavigationQuickChordMovesLeftWithoutBackspace) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey nav(0, 2, 9, LT(2, KC_BSPC)), a(0, 1, 2, KC_A), left(2, 1, 2, KC_LEFT);
    set_keymap({nav, a, left});
    nav.press(); run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_LEFT)); EXPECT_EMPTY_REPORT(driver);
    tap_key(a);
    expect_layer_state(2);
    nav.release(); run_one_scan_loop();
    expect_layer_state(0);
}

TEST_F(SofleHomeRow, NavigationQuickChordTypesNumberOnSameHand) {
    TestDriver driver;
    InSequence sequence;
    KeymapKey nav(0, 2, 9, LT(2, KC_BSPC)), h(0, 5, 7, KC_H), four(2, 5, 7, KC_4);
    set_keymap({nav, h, four});
    nav.press(); run_one_scan_loop();
    EXPECT_REPORT(driver, (KC_4)); EXPECT_EMPTY_REPORT(driver);
    tap_key(h);
    expect_layer_state(2);
    nav.release(); run_one_scan_loop();
    expect_layer_state(0);
}
