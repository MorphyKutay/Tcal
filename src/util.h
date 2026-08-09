#ifndef TCAL_UTIL_H
#define TCAL_UTIL_H

#include <stddef.h>

/* Number of Unicode codepoints in a NUL-terminated UTF-8 string. Since none
   of this app's text uses wide (double-column) codepoints, this equals the
   string's on-screen column width. */
size_t utf8_display_width(const char *s);

/* Copies up to `max_width` codepoints (not bytes) from `src` into `dst`
   (which must be at least `dst_size` bytes), NUL-terminated. Never splits a
   multi-byte codepoint. */
void utf8_truncate(char *dst, size_t dst_size, const char *src, int max_width);

#endif
