#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace meson {
using namespace std;

typedef char utf8_t;
typedef string::size_type align_t;

class StreamIterator
{
    public:
        explicit StreamIterator(stringstream *in);
        virtual ~StreamIterator();
        string *nextLine(void);

    private:
        StreamIterator(const StreamIterator& copy);
        StreamIterator& operator=(StreamIterator&);
        stringstream *inStream;

    public:
        bool hasLine(void) const
        {
            return !this->inStream->eof();
        }
};

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
        align_t indent;
        string *format_(string *s);
        void format_v(utf8_t __c, string *_s);
        string *indent_(const utf8_t *_append);
        align_t count_(utf8_t __c, string *_s);
        void write_(ostream& _os, string *_s);
        bool match_(string *target, const utf8_t *rexp);
        void replace_(string *target, const utf8_t *rexp, const utf8_t *rfmt);
};
}
