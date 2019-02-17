#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <string>
#include <ostream>

namespace Meson {
using namespace std;

// pseudo-implementation of utf8-encoded char
typedef __typeof__ (u8'T') utf8_t;
template <typename Tp>
struct __attribute__ ((visibility("protected"), aligned)) def {
    typedef Tp char_type;
    typedef Tp *string_type;
    typedef const Tp *cstring_type;
};
typedef def<utf8_t>::char_type utf8_char_t;
typedef def<utf8_t>::string_type utf8_string_t;
typedef def<utf8_t>::cstring_type utf8_cstring_t;

// typeof size_t
typedef unsigned long pos_t;

// For regular-expression descriptions
template <typename T>
struct _HINT_ {
    _HINT_(T)
    {
    };
};

typedef _HINT_<const char *> __HINT__;

typedef const basic_string<utf8_char_t> it;

extern basic_ostream<utf8_char_t>& error(basic_string<utf8_char_t> it);
}
