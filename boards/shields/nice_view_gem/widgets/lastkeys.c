#include <stdio.h>
#include <zephyr/kernel.h>
#include "lastkeys.h"
#include "../assets/custom_fonts.h"

#define MOD_SHIFT_MASK 0x22

static const char num_shift[10] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};

/* HID usage 0x2D..0x38 */
static const char sym_lower[] = "-=[]\\#;'`,./";
static const char sym_upper[] = "_+{}|~:\"~<>?";

static const char *fkey_name(uint32_t kc, char buf[4]) {
    int n = (int)(kc - 0x39);
    if (n >= 1 && n <= 12) {
        snprintf(buf, 4, "F%d", n);
        return buf;
    }
    return NULL;
}

const char *hid_to_token(uint32_t kc, uint8_t mods, char single_buf[2]) {
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
        return "SPC";
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
    default: break;
    }

    if (kc >= 0x3A && kc <= 0x45) {
        static char fbuf[4];
        return fkey_name(kc, fbuf);
    }

    return NULL;
}

static void draw_label(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_label_dsc_t hdr_dsc;
    init_label_dsc(&hdr_dsc, LVGL_FOREGROUND, &pixel_operator_mono, LV_TEXT_ALIGN_LEFT);
    lv_canvas_draw_text(canvas, 0, 50 + BUFFER_OFFSET_MIDDLE, BUFFER_SIZE, &hdr_dsc, "KEYS");

    lv_draw_label_dsc_t body_dsc;
    init_label_dsc(&body_dsc, LVGL_FOREGROUND, &pixel_operator_mono, LV_TEXT_ALIGN_RIGHT);
    const char *txt = state->last_keys[0] ? state->last_keys : "";
    lv_canvas_draw_text(canvas, 0, 75 + BUFFER_OFFSET_MIDDLE, BUFFER_SIZE, &body_dsc, txt);
}

void draw_lastkeys_status(lv_obj_t *canvas, const struct status_state *state) {
    draw_label(canvas, state);
}
