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
        StreamIterator(const StreamIterator& copy);       // copy constructor not to be implemented
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
        void write_to_file(string path);
        void read_from_file(string path);
        void write_to_stream(ostream& os);
        void read_from_stream(istream *is);

    private:
        string *to_format(string *s);
        void write_to(ostream& _os, string *_s);
        void replace(string *target, const char *rexp, const char *rfmt);
};
}
