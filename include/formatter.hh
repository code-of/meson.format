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
typedef unsigned long align_t;

class StreamIterator
{
    public:
        explicit StreamIterator(stringstream *in);
        virtual ~StreamIterator();
        int getStreamLength(void) const;
        string *nextLine(void);
        streamoff tellg(void);

    private:
        StreamIterator(const StreamIterator& copy);       // copy constructor not to be implemented
        StreamIterator& operator=(StreamIterator&);
        stringstream *inStream;
        string *iBuf;
        streamoff streamLength;

    public:
        bool hasLine(void) const
        {
            return !this->inStream->eof();
        }
};

class Formatter {
    public:
        Formatter(int argc, char **argv);
        ~Formatter(void);
        int run(void);

    private:
        bool hasOfile;
        bool hasIfile;
        bool hasRfile;
        bool fScope;
        align_t indent;
        ofstream *oFile;
        ifstream *iFile;
        stringstream buf;
        string *format(string *in);
        void writeLine(string *out);
        void printLine(string *out);
        void parseCmdLine(int argc, char **argv);
};
}
