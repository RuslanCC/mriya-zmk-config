#pragma once

#include <lvgl.h>
#include "util.h"

struct lastkeys_status_state {
    uint32_t keycode;
    uint16_t usage_page;
    uint8_t mods;
    bool pressed;
};

const char *hid_to_token(uint16_t usage_page, uint32_t keycode, uint8_t mods, char single_buf[2]);

void draw_lastkeys_status(lv_obj_t *canvas, const struct status_state *state);
