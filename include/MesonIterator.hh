#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <iostream>
#include <fstream>
#include <sstream>
#include <MesonTypes.hh>

namespace Meson {
using namespace std;

class StreamIterator
{
    public:
        explicit StreamIterator(basic_stringstream<utf8_char_t> *in);
        virtual ~StreamIterator();
        basic_string<utf8_char_t> *nextLine(void);

    private:
        StreamIterator(const StreamIterator& copy);
        StreamIterator& operator=(StreamIterator&);
        basic_stringstream<utf8_char_t> *inStream;

    public:
        bool hasLine(void) const
        {
            return !this->inStream->eof();
        }
};
}
