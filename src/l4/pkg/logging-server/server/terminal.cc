#include <cstdio>
#include "terminal.h"

void Terminal::clear() {
    gfxbitmap_fill((l4_uint8_t*)fb, (l4re_video_view_info_t*)&view_info,
            0, 0, view_info.width, view_info.height, bg);

    x = 0;
    y = 0;
}

void Terminal::insert_newline() {
    x = 0;

    while (y >= term_height() - 1) {
        scroll();
        y--;
    }

    y++;
}

void Terminal::scroll() {
    gfxbitmap_copy((l4_uint8_t*)fb, (l4_uint8_t*)fb, (l4re_video_view_info_t*)&view_info,
            0, char_height(),
            view_info.width, (term_height() - 1) * char_height(),
            0, 0);

    // clear the last row
    gfxbitmap_fill((l4_uint8_t*)fb, (l4re_video_view_info_t*)&view_info,
            0, char_y_pos(term_height() - 1), view_info.width, char_height(), bg);
}

void Terminal::print_char(const char character) {
    if (character == '\n') {
        insert_newline();
        return;
    }

    if (y == term_height()) {
        y -= 1;
        scroll();
    }

    gfxbitmap_font_text(fb, (l4re_video_view_info_t *)&view_info,
            GFXBITMAP_DEFAULT_FONT, (const char[1]){character}, 1,
            char_x_pos(x),
            char_y_pos(y),
            fg, bg);

    x = (x + 1) % term_width();
    if (x == 0) y++;
}

void Terminal::print_str(const char *str, unsigned length) {
    while (*str && --length)
        print_char(*str++);
}

int Terminal::init(Terminal **terminal) {
    auto fb = new Goos_fb("fb");
    if (!fb) {
        printf("Could not get frame buffer\n");
        return 1;
    }

    auto base = fb->attach_buffer();
    View::Info view_info;

    if (fb->view_info(&view_info)) {
        printf("Could not get view info\n");
        return 1;
    }

    printf("frame buffer: dimensions = %ldx%ld\n", view_info.width, view_info.height);

    if (gfxbitmap_font_init()) {
        printf("Error initializing font library\n");
        return 1;
    }

    *terminal = new Terminal(base, view_info);
    (*terminal)->clear();

    return 0;
}

