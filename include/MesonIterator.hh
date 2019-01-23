#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace Meson {
using namespace std;

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
}
