#include <stdio.h>
#include <zephyr/kernel.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include "lastkeys.h"
#include "../assets/custom_fonts.h"

#define MOD_SHIFT_MASK 0x22

static const char num_shift[10] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};

/* HID usage 0x2D..0x38 */
static const char sym_lower[] = "-=[]\\#;'`,./";
static const char sym_upper[] = "_+{}|~:\"~<>?";

static const char *keyboard_token(uint32_t kc, uint8_t mods, char single_buf[2]) {
    bool shift = (mods & MOD_SHIFT_MASK) != 0;

    if (kc >= 0x04 && kc <= 0x1D) { /* A..Z */
        single_buf[0] = (char)((shift ? 'A' : 'a') + (kc - 0x04));
        single_buf[1] = '\0';
        return single_buf;
    }
    if (kc >= 0x1E && kc <= 0x26) { /* 1..9 */
        single_buf[0] = shift ? num_shift[kc - 0x1D] : (char)('1' + (kc - 0x1E));
        single_buf[1] = '\0';
        return single_buf;
    }
    if (kc == 0x27) { /* 0 */
        single_buf[0] = shift ? num_shift[0] : '0';
        single_buf[1] = '\0';
        return single_buf;
    }
    if (kc >= 0x2D && kc <= 0x38) {
        single_buf[0] = shift ? sym_upper[kc - 0x2D] : sym_lower[kc - 0x2D];
        single_buf[1] = '\0';
        return single_buf;
    }
    if (kc == 0x2C) { /* Space */
        single_buf[0] = ' ';
        single_buf[1] = '\0';
        return single_buf;
    }

    switch (kc) {
    case 0x28: return "ENT";
    case 0x29: return "ESC";
    case 0x2A: return "BSP";
    case 0x2B: return "TAB";
    case 0x39: return "CAP";
    case 0x4C: return "DEL";
    case 0x4A: return "HOM";
    case 0x4D: return "END";
    case 0x4B: return "PUP";
    case 0x4E: return "PDN";
    case 0x4F: return ">";
    case 0x50: return "<";
    case 0x51: return "v";
    case 0x52: return "^";
    case 0x53: return "NUM";
    case 0x46: return "PSC";
    case 0x47: return "SLK";
    case 0x48: return "PAU";
    case 0x49: return "INS";
    /* Modifier keycodes */
    case 0xE0: return "LC";
    case 0xE1: return "LS";
    case 0xE2: return "LA";
    case 0xE3: return "LG";
    case 0xE4: return "RC";
    case 0xE5: return "RS";
    case 0xE6: return "RA";
    case 0xE7: return "RG";
    default: break;
    }

    if (kc >= 0x3A && kc <= 0x45) {
        static char fbuf[4];
        snprintf(fbuf, sizeof(fbuf), "F%d", (int)(kc - 0x39));
        return fbuf;
    }

    return NULL;
}

static const char *consumer_token(uint32_t kc) {
    switch (kc) {
    case 0xB0: return "PLY";
    case 0xB1: return "PAU";
    case 0xB2: return "REC";
    case 0xB3: return "FFW";
    case 0xB4: return "RWD";
    case 0xB5: return "NXT";
    case 0xB6: return "PRV";
    case 0xB7: return "STP";
    case 0xB8: return "EJT";
    case 0xCD: return "PLY";
    case 0xE2: return "MUT";
    case 0xE9: return "V+";
    case 0xEA: return "V-";
    case 0x6F: return "B+";
    case 0x70: return "B-";
    case 0x183: return "MED";
    case 0x18A: return "MAI";
    case 0x192: return "CAL";
    case 0x194: return "MYC";
    case 0x221: return "SRC";
    case 0x223: return "HOM";
    case 0x224: return "BCK";
    case 0x225: return "FWD";
    case 0x226: return "STP";
    case 0x227: return "RFR";
    case 0x22A: return "FAV";
    default: break;
    }
    return NULL;
}

const char *hid_to_token(uint16_t usage_page, uint32_t kc, uint8_t mods, char single_buf[2]) {
    if (usage_page == HID_USAGE_KEY) {
        return keyboard_token(kc, mods, single_buf);
    }
    if (usage_page == HID_USAGE_CONSUMER) {
        return consumer_token(kc);
    }
    return NULL;
}

static void draw_label(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_label_dsc_t hdr_dsc;
    init_label_dsc(&hdr_dsc, LVGL_FOREGROUND, &pixel_operator_mono, LV_TEXT_ALIGN_LEFT);
    lv_canvas_draw_text(canvas, 0, 50 + BUFFER_OFFSET_MIDDLE, BUFFER_SIZE, &hdr_dsc, "KEYS");

    lv_draw_label_dsc_t body_dsc;
    init_label_dsc(&body_dsc, LVGL_FOREGROUND, &pixel_operator_mono, LV_TEXT_ALIGN_LEFT);
    const char *txt = state->last_keys[0] ? state->last_keys : "";
    /* Large max_w disables wrapping; right-align anchors at x+max_w, so
       use LEFT align with manual right-edge placement. */
    int glyph_w = 8;
    int len = 0;
    for (const char *p = txt; *p; p++) {
        len++;
    }
    int text_w = len * glyph_w;
    int x = BUFFER_SIZE - text_w;
    if (x < 0) {
        x = 0;
    }
    lv_canvas_draw_text(canvas, x, 75 + BUFFER_OFFSET_MIDDLE, 500,
                        &body_dsc, txt);
}

void draw_lastkeys_status(lv_obj_t *canvas, const struct status_state *state) {
    draw_label(canvas, state);
}
