#ifndef TERMINAL_H
#define TERMINAL_H

#include <l4/re/video/goos>
#include <l4/re/util/video/goos_fb>
#include <l4/libgfxbitmap/font.h>

using namespace L4Re::Util::Video;
using namespace L4Re::Video;

struct Terminal {
    Terminal(void *fb, View::Info info): fb(fb), view_info(info) {
        fg = white();
        bg = black();
    }

    void clear();
    static int init(Terminal **terminal);

    inline gfxbitmap_color_pix_t white() {
        return gfxbitmap_convert_color((l4re_video_view_info_t *)&view_info, 0xffffff);
    }

    inline gfxbitmap_color_pix_t black() {
        return gfxbitmap_convert_color((l4re_video_view_info_t *)&view_info, 0x00000000);
    }

    static inline unsigned char_width() {
        return gfxbitmap_font_width(GFXBITMAP_DEFAULT_FONT);
    }

    static inline unsigned char_height() {
        return gfxbitmap_font_height(GFXBITMAP_DEFAULT_FONT);
    }

    inline unsigned term_width() const {
        return view_info.width / char_width();
    }

    inline unsigned term_height() const {
        return view_info.height / char_height();
    }

    unsigned char_x_pos(unsigned x) {
        return x * char_width();
    }

    unsigned char_y_pos(unsigned y) {
        return y * char_height();
    }

    void print_char(const char input);
    void print_str(const char *str, unsigned length);
    void insert_newline();
    void scroll();

private:
    unsigned x = 0, y = 0;
    void* fb;
    View::Info view_info;
    
    gfxbitmap_color_pix_t fg, bg;
};

#endif
