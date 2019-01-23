#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <MesonFormatter.hh>

namespace Meson {
using namespace std;

class Console : public Formatter {
    public:
        Console(int argc, char **argv);
        ~Console(void);
        int run(void);

    private:
        bool hasIfile;
        bool hasOfile;
        bool hasRfile;
        string *Ifile;
        string *Ofile;
        string *Rfile;
        void init(int argc, char **argv);
        void usage(void);
};
}
