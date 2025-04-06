/* Copyright 2024 @ Keychron (https://www.keychron.com)
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
#include "keychron_common.h"


enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    // Emacs Ctrl binding
    WIN_FN,
    // Emacs C-x binding
    WIN_CX,
    // Emacs set-mark mode
    MAC_CX
};

enum custom_keycodes {
    CUT_LINE = SAFE_RANGE,  // cutline as Emacs
    SET_MARK,   
    ABORT,
    COPY_TEXT,  
    CUT_TEXT,
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
        case COPY_TEXT:
            if (record->event.pressed) {
                tap_code16(C(KC_C));
                set_mark_active = false;
            }
            break;
        case CUT_TEXT:
            if (record->event.pressed) {
                tap_code16(C(KC_X));
                set_mark_active = false;
            }
            break;
        case KC_LEFT: case KC_RIGHT: case KC_UP: case KC_DOWN: 
        case KC_HOME: case KC_END: case KC_PGDN: case KC_PGUP:
            if (set_mark_active) {
                if (record->event.pressed) {
                    register_code(KC_LSFT);
                } else {
                    unregister_code(KC_LSFT);
                }
            }
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
    // Keychron original macros
    // true: Process all other keycodes normally
    // false: Keychron original keycodes
    return process_record_keychron_common(keycode, record);
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

// clang-format off
// see https://docs.qmk.fm/feature_layers
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    //  0         1         2         3         4         5         6         7         8         9         10        11        12          13        13.5      14        15        16        17        18        19        (20)
    [MAC_BASE] = LAYOUT_113_jis(
        KC_ESC,    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,     KC_MUTE,            KC_SNAP,  KC_SIRI,  RGB_MOD,  KC_F13,   KC_F14,   KC_F15,   KC_F16,
        KC_GRV,    KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_INT3,  KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,    KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_ENT,             KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_BSLS,                                                      KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,                                  KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,   KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD, KC_ROPTN, MO(MAC_FN), KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT          ),
    [MAC_FN] = LAYOUT_113_jis(
        _______, KC_BRID,      KC_BRIU,     KC_MCTRL, KC_LNPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU, RGB_TOG,           _______,    _______,    RGB_TOG,      _______,  _______,  _______,  _______,
        _______, BT_HST1,      BT_HST2,     BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,    _______,    _______,      _______,  _______,  _______,  _______,
        RGB_TOG, RGB_MOD,      RGB_VAI,     RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,           _______,    _______,    _______,      _______,  _______,  _______,  _______,
        _______, KC_HOME,      RGB_VAD,     RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                                                           _______,  _______,  _______,
        _______, _______,      _______,     _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,                                 _______,                  _______,  _______,  _______,  _______,
        _______, _______,      _______,     _______,                      _______,                      _______,  _______,  _______,  _______,  _______,           _______,    _______,    _______,      _______,            _______          ),
    [WIN_BASE] = LAYOUT_113_jis(
        KC_ESC,    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,     KC_MUTE,            KC_PSCR,  CHATGPT,  RGB_MOD,  RGB_HUI,  RGB_SAI,  RGB_VAI,  RGB_SPI,
        KC_GRV,    KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_INT3,  KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,    KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_ENT,             KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        MO(WIN_FN),KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_BSLS,                                                      KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,                                  KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,   KC_LGUI,  KC_LALT,  KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RGUI,   KC_RALT, MO(MAC_FN), KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT          ),
    [WIN_FN] = LAYOUT_113_jis(
        _______, KC_BRID,      KC_BRIU,     KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV, KC_MPLY,    KC_MNXT,   KC_MUTE,     KC_VOLD,     KC_VOLU,    RGB_TOG,            G(KC_G),    COPILOT,    RGB_RMOD,    RGB_HUD,    RGB_SAD,  RGB_VAD,  RGB_SPD,
        _______, BT_HST1,      BT_HST2,     BT_HST3,  P2P4G,    _______,  _______,  _______, _______,    _______,   _______,     _______,     _______,    _______,   _______, _______,    _______,    _______,     _______,    _______,  _______,  _______,
        RGB_TOG, G(S(KC_F23)), CUT_TEXT,    KC_END,   C(KC_R),  C(KC_T),  C(KC_V),  C(KC_Z), KC_TAB,     C(KC_O),   KC_UP,       SET_MARK,    C(KC_RBRC), C(KC_ENT),          _______,    _______,    _______,     _______,    _______,  _______,  _______,
        _______, KC_HOME,      C(KC_F),     KC_DEL,   KC_RGHT,  ABORT,    KC_BSPC,  KC_ENT,  CUT_LINE,   C(KC_L),   C(KC_SCLN),  C(KC_QUOT),  C(KC_BSLS),                                                          _______,    _______,  _______,
     C(KC_LSFT), G(KC_DOWN),   OSL(WIN_CX), C(KC_C),  KC_PGDN,  KC_LEFT,  KC_DOWN,  KC_ENT,  C(KC_COMM), C(KC_DOT), C(KC_Z),     _______,     C(KC_RSFT),                                 KC_MS_UP,                _______,    _______,  _______,  _______,
        _______, C(KC_LGUI),   C(KC_LALT),  _______,                      SET_MARK,                      _______,   C(KC_RCMMD), C(KC_ROPTN), _______,    KC_MS_BTN1,         KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, KC_MS_BTN2,           _______          ),
    [MAC_CX] = LAYOUT_113_jis(
        _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  S(C(KC_S)), _______,  _______,  _______,  _______,  C(KC_X),  _______,A(KC_TAB),  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  C(KC_S),    KC_FILE,  C(KC_O),  _______,  C(KC_A),  _______,  C(KC_W),  _______,  _______,  _______,  _______,                                                      _______,  _______,  _______,
        _______,  _______,  _______,    A(KC_F4), _______,  KC_TASK,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                                  _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,    _______,                      _______,                      _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,            _______          ),
    [WIN_CX] = LAYOUT_113_jis(
        _______,  _______,  _______,    _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  CHATGPT,     PERP,     GGL_EN,     SCLR,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  S(C(KC_S)), _______,  _______,  _______,   _______,  C(KC_X),  _______,A(KC_TAB),  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  C(KC_S),    G(KC_E),  C(KC_O),  _______,   C(KC_A),  _______,  C(KC_W),  _______,  _______,  _______,  _______,                                                      _______,  _______,  _______,
        _______,  _______,  _______,    A(KC_F4), _______,  G(KC_TAB), _______,  _______,  _______,  _______,  _______,  _______,  _______,                                  _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,    _______,                       _______,                      _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,            _______          )
    // [] = LAYOUT_113_jis(
    //     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
    //     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    //     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,
    //     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                                                      _______,  _______,  _______,
    //     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                                  _______,            _______,  _______,  _______,  _______,
    //     _______,  _______,  _______,  _______,                      _______,                      _______,  _______,  _______,  _______,    _______,            _______,  _______,  _______,  _______,            _______          )        
};

// clang-format on
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [WIN_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [MAC_CX]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [WIN_CX]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)}
};
#endif // ENCODER_MAP_ENABLE
