#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

struct _str {
    char *heap;
    size_t size;
    size_t length;
};

typedef struct _str str_t[1];
typedef struct _str *str_ptr;
typedef struct _str const *const_str_ptr;

#define STR_PTR_FREE(s) {			\
    str_free (s);				\
    free (s);					\
    (s) = NULL;					\
  }


#define STR_SIZE_CHECK(s, len) { \
    if((len)+1 > (s)->size) { \
        str_resize(s, len); \
    } \
  }

#define STR_LENGTH(s) ((s)->length)
#define CSTR(s) ((const char *) (s)->heap)
#define str_set_null(s) str_trunc(s, 0);
#define str_is_null(s) ((s)->length == 0)


extern str_ptr  str_new(void);
extern void     str_init(str_ptr s, int len);
extern void     str_init_view(str_ptr s, const char *data);
extern void     str_free(str_ptr s);
extern void     str_resize(str_t s, size_t reqlen);
extern void     str_init_from_c(str_ptr s, const char *sf);
extern void     str_init_from_str(str_ptr s, const str_t sf);
extern void     str_copy(str_t d, const str_t s);
extern void     str_copy_c(str_t d, const char *s);
extern void     str_copy_c_substr(str_t d, const char *s, int len);
extern void     str_append_c(str_t to, const char *from, int sep);
extern void     str_append(str_t to, const str_t from, int sep);
extern void     str_append_char(str_t s, char ch);
extern void     str_trunc(str_t s, int len);
extern void     str_get_basename(str_t to, const str_t from, int dirsep);
extern void     str_dirname(str_t to, const str_t from, int dirsep);
extern int      str_getline(str_t d, FILE *f);
extern void     str_printf(str_t d, const char *fmt, ...);
extern void     str_printf_add(str_t d, const char *fmt, ...);
extern void     str_vprintf(str_t d, const char *fmt, int append, va_list ap);
extern void     str_pad(str_t d, int len, char sep);
