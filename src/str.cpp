#include <assert.h>
#include <string.h>

#include "str.h"
#include "str_priv.h"

void
str_init(str_ptr s, int len)
{
    if(len < 0) {
        len = 0;
    }
    str_init_raw(s, (size_t)len);
    s->heap[0] = 0;
    s->length = 0;
}

void
str_init_view(str_ptr s, const char *data)
{
    s->heap = (char *) data;
    s->length = strlen(data);
    s->size = 0;
}

str_ptr
str_new(void)
{
    str_ptr s = (str_ptr) str_malloc(sizeof(str_t));
    str_init_raw(s, 15);
    s->heap[0] = 0;
    s->length = 0;
    return s;
}

void
str_free(str_ptr s)
{
    if(s->size > 0) {
        free(s->heap);
    }
    s->heap = 0;
}

static void
str_init_from_c_raw(str_ptr s, const char *sf, size_t len)
{
    str_init_raw(s, len);
    memcpy(s->heap, sf, len+1);
    s->length = len;
}

void
str_init_from_c(str_ptr s, const char *sf)
{
    size_t len = strlen(sf);
    str_init_from_c_raw(s, sf, len);
}

void
str_init_from_str(str_ptr s, const str_t sf)
{
    str_init_from_c_raw(s, CSTR(sf), STR_LENGTH(sf));
}

void
str_resize(str_t s, size_t len)
{
    char *old_heap = (s->size > 0 ? s->heap : nullptr);
    str_init_raw(s, ALLOC_SIZE_INCREASE(len));

    if(old_heap) {
        memcpy(s->heap, old_heap, s->length+1);
        free(old_heap);
    }
}

void
str_copy(str_t d, const str_t s)
{
    const size_t len = s->length;
    STR_SIZE_CHECK(d, len);
    memcpy(d->heap, s->heap, len+1);
    d->length = len;
}

void
str_copy_c(str_t d, const char *s)
{
    const size_t len = strlen(s);
    STR_SIZE_CHECK(d, len);
    memcpy(d->heap, s, len+1);
    d->length = len;
}

void
str_copy_c_substr(str_t d, const char *s, int len)
{
    len = (len >= 0 ? len : 0);
    STR_SIZE_CHECK(d, (size_t)len);
    memcpy(d->heap, s, (size_t)len);
    d->heap[len] = 0;
    d->length = len;
}

void
str_append_c(str_t to, const char *from, int sep)
{
    size_t flen = strlen(from);
    size_t newlen = STR_LENGTH(to) + flen + (sep == 0 ? 0 : 1);
    int idx = STR_LENGTH(to);

    STR_SIZE_CHECK(to, newlen);

    if(sep != 0) {
        to->heap[idx++] = sep;
    }

    memcpy(to->heap + idx, from, flen+1);
    to->length = newlen;
}

void
str_append(str_t to, const str_t from, int sep)
{
    str_append_c(to, CSTR(from), sep);
}

void
str_append_char(str_t s, char ch)
{
    STR_SIZE_CHECK(s, s->length + 1);
    s->heap[s->length] = ch;
    s->heap[s->length + 1] = '\0';
    s->length ++;
}

void
str_trunc(str_t s, int len)
{
    if(len < 0 || (size_t)len >= STR_LENGTH(s)) {
        return;
    }

    s->heap[len] = 0;
    s->length = len;
}

void
str_get_basename(str_t to, const str_t from, int dirsep)
{
    const char * ptr = strrchr(CSTR(from), dirsep);

    if(ptr == nullptr) {
        str_copy_c(to, CSTR(from));
    } else {
        str_copy_c(to, ptr + 1);
    }
}

void
str_dirname(str_t to, const str_t from, int dirsep)
{
    const char * ptr = strrchr(CSTR(from), dirsep);

    if(ptr == nullptr) {
        str_copy_c(to, CSTR(from));
    } else {
        str_copy_c_substr(to, CSTR(from), ptr - CSTR(from));
    }
}

int
str_getline(str_t d, FILE *f)
{
    char *res, *ptr;
    int szres, pending = 0;

    STR_SIZE_CHECK(d, 0);
    str_trunc(d, 0);

    for(;;) {
        int j;

        ptr = d->heap + d->length;
        szres = d->size - d->length;

        res = fgets(ptr, szres, f);

        if(res == nullptr) {
            if(feof(f) && pending) {
                return 0;
            }
            return (-1);
        }

        for(j = 0; j < szres; j++) {
            if(ptr[j] == '\n' || ptr[j] == 0) {
                ptr[j] = 0;
                d->length += j;
                break;
            }
        }

        if(j < szres - 1) {
            break;
        }

        pending = 1;

        STR_SIZE_CHECK(d, 2 * d->length);
    }

    if(d->length > 0) {
        if(d->heap[d->length - 1] == '\015') {
            d->heap[d->length - 1] = 0;
            d->length --;
        }
    }

    return 0;
}

void
str_vprintf(str_t d, const char *fmt, int append, va_list ap)
{
#define STR_BUFSIZE 64
    char buffer[STR_BUFSIZE];
    char *xbuf;
    int xbuf_size;
    int ns;

    ns = vsnprintf(buffer, STR_BUFSIZE, fmt, ap);

    if(ns >= STR_BUFSIZE) {
        xbuf_size = ns+1;
        xbuf = (char *) str_malloc(xbuf_size);
        vsnprintf(xbuf, xbuf_size, fmt, ap);
    } else {
        xbuf = buffer;
        xbuf_size = 0;
    }

    if(append) {
        str_append_c(d, xbuf, 0);
        if(xbuf_size > 0) {
            free(xbuf);
        }
    } else {
        if(xbuf_size > 0) {
            free(d->heap);
            d->heap = xbuf;
            d->size = xbuf_size;
            d->length = ns;
        } else {
            str_copy_c_substr(d, xbuf, ns);
        }
    }
#undef STR_BUFSIZE
}

void
str_printf(str_t d, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    str_vprintf(d, fmt, 0, ap);
    va_end(ap);
}

void
str_printf_add(str_t d, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    str_vprintf(d, fmt, 1, ap);
    va_end(ap);
}

void
str_pad(str_t s, int len, char sep)
{
    int diff = len - s->length;

    if(diff <= 0) {
        return;
    }

    STR_SIZE_CHECK(s, (size_t)len);

    memmove(s->heap + diff, s->heap, (s->length + 1) * sizeof(char));
    memset(s->heap, sep, diff * sizeof(char));

    s->length += diff;
}
