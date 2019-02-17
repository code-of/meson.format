#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <string>
#include <iostream>
#include <MesonTypes.hh>

namespace Meson {
using namespace std;

class Formatter {
    public:
        Formatter(void);
        virtual ~Formatter(void);

    protected:
        void read(basic_istream<utf8_char_t> *is);
        void read(basic_string<utf8_char_t> path);
        void write(basic_ostream<utf8_char_t>& os);
        void write(basic_string<utf8_char_t> path);

    private:
        pos_t indent;
        pos_t _count(utf8_char_t _c, basic_string<utf8_char_t> *_s);
        bool _match(basic_string<utf8_char_t> *target, utf8_cstring_t rexp);
        basic_string<utf8_char_t> *_format(basic_string<utf8_char_t> *s);
        basic_string<utf8_char_t> *_indent(utf8_cstring_t _append);
        void _format_ifcase(basic_string<utf8_char_t> *_s);
        void _format_foreach(basic_string<utf8_char_t> *_s);
        void _format_v(utf8_char_t _c, basic_string<utf8_char_t> *_s);
        void _write(basic_ostream<utf8_char_t>& _os, basic_string<utf8_char_t> *_s);
        void _replace(basic_string<utf8_char_t> *target, utf8_cstring_t rexp, utf8_cstring_t rfmt);
};
}
