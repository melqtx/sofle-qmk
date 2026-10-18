// SPDX-License-Identifier: GPL-2.0-or-later
// OLED coordinates are 32 x 128 after rotation.
#ifdef OLED_ENABLE
static uint8_t cat_pixels[512];
static void cat_pixel(int x, int y) {
    if (x >= 0 && x < 32 && y >= 0 && y < 128) cat_pixels[x + (y / 8) * 32] |= 1U << (y % 8);
}
static void cat_line(int x0, int y0, int x1, int y1) {
    int dx = x1 > x0 ? x1 - x0 : x0 - x1, sx = x0 < x1 ? 1 : -1;
    int dy = -(y1 > y0 ? y1 - y0 : y0 - y1), sy = y0 < y1 ? 1 : -1, err = dx + dy;
    for (;;) {
        cat_pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
static const uint8_t handle_widths[] = {5, 5, 3, 1, 3, 3, 3};
static const uint8_t handle_pixels[][5] = {
    {62, 65, 93, 85, 30}, // @
    {124, 4, 120, 4, 120}, // m
    {56, 84, 92}, // e
    {127}, // l
    {24, 36, 124}, // q
    {4, 63, 68}, // t
    {108, 16, 108}, // x
};

static void cat_handle(void) {
    int x = 1; // 29 pixels wide; one-pixel left margin, two-pixel right margin.
    for (unsigned g = 0; g < sizeof(handle_widths); ++g) {
        for (int c = 0; c < handle_widths[g]; ++c)
            for (int y = 0; y < 7; ++y)
                if (handle_pixels[g][c] & (1U << y)) cat_pixel(x + c, 120 + y);
        x += handle_widths[g] + 1;
    }
}
static void cat_head(int y, bool asleep) {
    cat_line(4,y+13,4,y+3); cat_line(4,y+3,11,y+9); cat_line(11,y+9,20,y+9);
    cat_line(20,y+9,27,y+3); cat_line(27,y+3,27,y+13);
    cat_line(4,y+13,2,y+18); cat_line(2,y+18,4,y+24); cat_line(4,y+24,10,y+27);
    cat_line(10,y+27,21,y+27); cat_line(21,y+27,27,y+24); cat_line(27,y+24,29,y+18); cat_line(29,y+18,27,y+13);
    cat_line(6,y+8,6,y+12); cat_line(25,y+8,25,y+12);
    if (asleep) {
        cat_line(7,y+17,11,y+17); cat_line(20,y+17,24,y+17);
    } else {
        cat_line(9,y+15,9,y+18); cat_line(22,y+15,22,y+18);
        cat_pixel(8,y+15); cat_pixel(21,y+15);
    }
    cat_pixel(15,y+20); cat_pixel(16,y+20);
    cat_line(15,y+21,13,y+23); cat_line(16,y+21,18,y+23);
    cat_line(1,y+20,5,y+21); cat_line(26,y+21,30,y+20);
}
static void cat_sitting(bool asleep) {
    const int y = 34;
    cat_head(y, asleep);
    cat_line(7,y+27,5,y+35); cat_line(5,y+35,5,y+47);
    cat_line(5,y+47,8,y+51); cat_line(8,y+51,23,y+51);
    cat_line(23,y+51,26,y+47); cat_line(26,y+47,26,y+35); cat_line(26,y+35,24,y+27);
    cat_line(10,y+37,10,y+48); cat_line(10,y+48,14,y+48);
    cat_line(21,y+37,21,y+48); cat_line(21,y+48,17,y+48);
    cat_line(26,y+45,30,y+41); cat_line(30,y+41,30,y+34);
    cat_line(30,y+34,28,y+31); cat_line(28,y+31,26,y+31);
}
static void cat_sprite(uint8_t frame, bool asleep) {
    const int y = 44;
    cat_head(y, asleep);
    for (int x = 1; x < 31; ++x) cat_pixel(x,y+39);
    cat_line(1,y+40,3,y+47); cat_line(3,y+47,13,y+47); cat_line(13,y+47,15,y+40);
    cat_line(16,y+40,18,y+47); cat_line(18,y+47,28,y+47); cat_line(28,y+47,30,y+40);
    cat_line(2,y+36,12,y+36); cat_line(17,y+36,29,y+36);
    cat_line(2,y+36,1,y+39); cat_line(12,y+36,15,y+39); cat_line(17,y+36,16,y+39); cat_line(29,y+36,30,y+39);
    int ly = (frame == 1 && !asleep) ? 30 : 35;
    int ry = (frame == 2 && !asleep) ? 30 : 35;
    cat_line(6,y+27,5,y+ly); cat_line(5,y+ly,10,y+ly); cat_line(10,y+ly,11,y+28);
    cat_line(20,y+28,21,y+ry); cat_line(21,y+ry,26,y+ry); cat_line(26,y+ry,25,y+27);
    if (!asleep && frame == 1) { cat_line(20,y+32,20,y+30); cat_line(28,y+32,30,y+30); }
    if (!asleep && frame == 2) { cat_line(3,y+32,1,y+30); cat_line(11,y+32,12,y+30); }
}
static void cat_render(bool left, uint8_t frame, bool asleep) {
    memset(cat_pixels, 0, sizeof(cat_pixels));
    if (left) {
        cat_sitting(asleep);
        cat_handle();
    } else {
        cat_sprite(frame, asleep);
    }
}
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    (void)rotation;
    return OLED_ROTATION_270;
}
bool oled_task_user(void) {
    static uint32_t last_active, last_frame;
    static uint8_t frame;
    uint32_t now = timer_read32();
    uint8_t wpm = get_current_wpm();
    if (wpm) last_active = now;
    bool asleep = timer_elapsed32(last_active) > 8000;
    if (wpm && timer_elapsed32(last_frame) > (uint32_t)(wpm > 100 ? 100 : 300 - wpm * 2)) {
        frame = frame == 1 ? 2 : 1;
        last_frame = now;
    }
    cat_render(is_keyboard_left(), wpm ? frame : 0, asleep);
    oled_set_cursor(0,0);
    oled_write_raw((const char *)cat_pixels,sizeof(cat_pixels));
    return false;
}
#endif
