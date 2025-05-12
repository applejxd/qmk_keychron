/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

// clang-format off
enum layers {
    UBU_BASE,
    UBU_FN,
    WIN_BASE,
    WIN_FN, // Emacs Ctrl binding
    WIN_CX,  // Emacs C-x binding
    UBU_CX
};


enum custom_keycodes {
    CUT_LINE = SAFE_RANGE,  // cutline as Emacs
    SET_MARK,   
    ABORT,
    // for Ubuntu
    NAUTILUS,
    UBU_GPT,
    // for Windows
    DEEPL,
    COPILOT,
    CHATGPT,
    PERP,
    GGL_EN,
    SCLR,
};

/* ------ */
/* Macros */
/* ------ */

// see https://docs.qmk.fm/reference_keymap_extras#header-files
#include <sendstring_japanese.h>

void run_on_windows(const char *url) {
    // open command dialog
    tap_code16(G(KC_R));
    // wait for moving active window and clear inputs
    SEND_STRING(SS_DELAY(300));
    tap_code(KC_DEL);
    // open URL
    SEND_STRING(SS_DELAY(100));
    send_string(url);
    SEND_STRING(SS_DELAY(100));
    tap_code(KC_ENT);
}


bool set_mark_active = false;  // マーク状態を保持
uint8_t mod_state;
// see https://docs.qmk.fm/feature_macros#using-macros-in-c-keymaps
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    mod_state = get_mods();
    switch (keycode) {
        case CUT_LINE:
            if (record->event.pressed) {
                tap_code16(S(KC_END));
                SEND_STRING(SS_DELAY(10));
                tap_code16(C(KC_X));
            }
            break;
        case SET_MARK:
            if (record->event.pressed) set_mark_active = !set_mark_active;
            break;
        case ABORT:
            if (record->event.pressed) {
                if (set_mark_active) {
                    // マーク解除時は ESC を送信しない
                    set_mark_active = false;
                } else {
                    tap_code(KC_ESC);
                }
            }
            break;
        case KC_LEFT: case KC_RIGHT: case KC_UP: case KC_DOWN: 
        case KC_HOME: case KC_END: case KC_PGDN: case KC_PGUP:
            // 範囲選択
            if (set_mark_active) {
                if (record->event.pressed) {
                    register_code(KC_LSFT);
                } else {
                    unregister_code(KC_LSFT);
                }
            }
            break;
        case C(KC_C): case C(KC_X): case C(KC_V): case C(KC_K): case KC_DEL:
            // 選択範囲を用いたアクションの後は選択解除
            if (record->event.pressed) { set_mark_active = false; }
            break;
        case KC_W:
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_ALT) {
                    del_mods(MOD_MASK_ALT);
                    tap_code16(C(KC_C));    // w/o alt key
                    set_mods(mod_state);
                    set_mark_active = false;
                    return false;
                }
            }
            return true;
        case NAUTILUS:
            if (record->event.pressed) {
                tap_code16(A(KC_F2));
                SEND_STRING(SS_DELAY(10)"nautilus");
                tap_code(KC_ENT);
            }
            break;
        case UBU_GPT:
            if (record->event.pressed) {
                tap_code16(A(KC_F2));
                SEND_STRING(SS_DELAY(10)"xdg-open https://chatgpt.com/");
                tap_code(KC_ENT);
            }
            break;
        case DEEPL:
            if (record->event.pressed) {
                tap_code16(G(KC_R));
                // wait for moving active window and clear inputs
                SEND_STRING(SS_DELAY(300));
                tap_code(KC_DEL);
                SEND_STRING(
                    SS_DELAY(100)
                    // run command
                    // "\%UserProfile\%/src/windows-setup/bin/deepl.bat"
                    "powershell -Command "
                    SS_DELAY(100)
                    "\"Start-Process (\'https://www.deepl.com/translator#en/ja/\'"
                    SS_DELAY(100)
                    " + [uri]::EscapeDataString((Get-Clipboard)))\""
                    SS_DELAY(100)
                );
                tap_code(KC_ENT);
            }
            break;
        case COPILOT:
            if (record->event.pressed) { run_on_windows("https://m365.cloud.microsoft/chat?auth=2"); }
            break;
        case CHATGPT:
            if (record->event.pressed) { run_on_windows("https://chatgpt.com/"); }
            break;
        case PERP:
            if (record->event.pressed) { run_on_windows("https://www.perplexity.ai/"); }
            break;
        case GGL_EN:
            if (record->event.pressed) { run_on_windows("https://www.google.com/webhp?gl=us&hl=en&gws_rd=cr&pws=0"); }
            break;
        case SCLR:
            if (record->event.pressed) { run_on_windows("https://scholar.google.com/"); }
            break;
    }
    return true;
}

/* -------- */
/* Override */
/* -------- */

// see https://docs.qmk.fm/features/key_overrides
// see https://docs.qmk.fm/feature_advanced_keycodes
#if defined(KEY_OVERRIDE_ENABLE) 
const key_override_t alt_v_to_pageup = ko_make_basic(MOD_MASK_ALT, KC_V, KC_PGUP);

// ALT+B -> CTRL+LEFT
const key_override_t alt_b_to_ctrl_left = ko_make_basic(MOD_MASK_ALT, KC_B, C(KC_LEFT));
// ALT+F -> CTRL+RIGHT
const key_override_t alt_f_to_ctrl_right = ko_make_basic(MOD_MASK_ALT, KC_F, C(KC_RIGHT));
// ALT+Y -> Win(GUI)+V
const key_override_t alt_w_to_gui_v = ko_make_basic(MOD_MASK_ALT, KC_Y, LGUI(KC_V));

const key_override_t **key_overrides = (const key_override_t *[]){
    &alt_v_to_pageup,
    &alt_b_to_ctrl_left,
    &alt_f_to_ctrl_right,
    &alt_w_to_gui_v,
    NULL
};
#endif // KEY_OVERRIDE_ENABLE

/* ------ */
/* Layers */
/* ------ */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [UBU_BASE] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP, // 32
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN, // 46
    MO(UBU_FN),   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME, // 61
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_INT5,                      KC_SPC,                       KC_INT4,  KC_RALT, MO(UBU_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [UBU_FN] = LAYOUT(
        KC_TRNS,  KC_BRID,  KC_BRIU,  KC_LGUI,    NAUTILUS, BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,   KC_TRNS,  KC_TRNS,    BL_BRTG,
        KC_TRNS,  BT_HST1,  BT_HST2,  BT_HST3,    KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,  KC_TRNS,    BL_UP,
        BL_TOGG,  UBU_GPT,  C(KC_X),  KC_END,     KC_TRNS,  KC_TRNS,  C(KC_V),  C(KC_Z),  KC_TAB,   KC_TRNS,    KC_UP,  SET_MARK, KC_TRNS,                         BL_DOWN,
        KC_TRNS,  KC_HOME,  C(KC_F),  KC_DEL,     KC_RGHT,  ABORT,    KC_BSPC,  KC_ENT,   CUT_LINE, KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,              KC_TRNS, 
        KC_TRNS,            G(KC_H),  OSL(UBU_CX),KC_TRNS,  KC_PGDN,  KC_LEFT,  KC_DOWN,  KC_ENT,   KC_TRNS,  KC_TRNS,  C(KC_Z),  KC_TRNS,   KC_TRNS,              KC_TRNS,
        KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,                        SET_MARK,                     KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_MS_LEFT,KC_MS_UP, KC_MS_DOWN, KC_MS_RIGHT),

    [WIN_BASE] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
    MO(WIN_FN),   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_INT5,                      KC_SPC,                       KC_INT4,  KC_RALT, MO(WIN_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT(
        KC_TRNS,  KC_BRID,     KC_BRIU,   KC_TASK,    KC_FILE,  BL_DOWN, BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,   KC_MUTE,   KC_VOLD,   KC_VOLU,   KC_TRNS,    KC_TRNS,    BL_BRTG,
        KC_TRNS,  BT_HST1,     BT_HST2,   BT_HST3,    DEEPL,    COPILOT, KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,    KC_TRNS,    BL_UP,
        BL_TOGG,  G(S(KC_F23)),C(KC_X),   KC_END,     C(KC_R),  C(KC_T), C(KC_V),  C(KC_Z),  KC_TAB,   C(KC_O),   KC_UP,     SET_MARK,  KC_ESC,                            BL_DOWN,
        KC_TRNS,  KC_HOME,     C(KC_F),   KC_DEL,     KC_RGHT,  ABORT,   KC_BSPC,  KC_ENT,   CUT_LINE, C(KC_L),   C(KC_SCLN),C(KC_QUOT),C(KC_NUHS),KC_TRNS,                KC_TRNS,
     C(KC_LSFT),               G(KC_DOWN),OSL(WIN_CX),C(KC_C),  KC_PGDN, KC_LEFT,  KC_DOWN,  KC_ENT,   C(KC_COMM),C(KC_DOT), C(KC_Z),   KC_TRNS,   C(KC_RSFT),             KC_TRNS,
        KC_TRNS,  C(KC_LGUI),  C(KC_LALT),KC_TRNS,                       SET_MARK,                     KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_MS_LEFT,KC_MS_UP,   KC_MS_DOWN, KC_MS_RIGHT),
    
    [WIN_CX] = LAYOUT(
        KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,
        KC_TRNS,  CHATGPT,     PERP,   GGL_EN,     SCLR,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,
        KC_TRNS,  KC_TRNS,S(C(KC_S)), KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  C(KC_X),  KC_TRNS,A(KC_TAB),  KC_TRNS,  KC_TRNS,  KC_TRNS,                      KC_TRNS,
        KC_TRNS,  KC_TRNS,  C(KC_S),  KC_FILE,  C(KC_O),  KC_TRNS,  C(KC_A),  KC_TRNS,  C(KC_W),  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
        KC_TRNS,            KC_TRNS,  KC_TRNS, A(KC_F4),  KC_TRNS,  KC_TASK,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,            KC_TRNS,
        KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,                      KC_TRNS,                      KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS),
};