#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <string>
#include <iostream>

namespace Meson {
using namespace std;

class Formatter {
    public:
        Formatter(void);
        ~Formatter(void);
        void read(istream *is);
        void read(string path);
        void write(ostream& os);
        void write(string path);

    private:
        // bool param;
        size_t indent;
        string *format_(string *s);
        void format_v(char __c, string *_s);
        string *indent_(const char *_append);
        size_t count_(char __c, string *_s);
        void write_(ostream& _os, string *_s);
        bool match_(string *target, const char *rexp);
        void replace_(string *target, const char *rexp, const char *rfmt);
};
}
