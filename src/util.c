#include "util.h"
#include <string.h>

static int is_continuation_byte(unsigned char c) {
    return (c & 0xC0) == 0x80;
}

size_t utf8_display_width(const char *s) {
    size_t width = 0;
    for (const unsigned char *p = (const unsigned char *)s; *p != '\0'; p++) {
        if (!is_continuation_byte(*p)) {
            width++;
        }
    }
    return width;
}

void utf8_truncate(char *dst, size_t dst_size, const char *src, int max_width) {
    size_t out = 0;
    int width = 0;
    const unsigned char *p = (const unsigned char *)src;

    while (*p != '\0' && width < max_width) {
        size_t char_len = 1;
        if ((*p & 0x80) == 0x00) {
            char_len = 1;
        } else if ((*p & 0xE0) == 0xC0) {
            char_len = 2;
        } else if ((*p & 0xF0) == 0xE0) {
            char_len = 3;
        } else if ((*p & 0xF8) == 0xF0) {
            char_len = 4;
        }

        if (out + char_len >= dst_size) {
            break;
        }
        memcpy(dst + out, p, char_len);
        out += char_len;
        p += char_len;
        width++;
    }

    dst[out] = '\0';
}
