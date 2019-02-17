#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <MesonFormatter.hh>

namespace Meson {
using namespace std;

class Console : protected Formatter {
    public:
        explicit Console(int argc, char **argv);
        virtual ~Console(void);
        int run(void);

    private:
        bool hasIfile;
        bool hasOfile;
        bool hasRfile;
        basic_string<utf8_char_t> *Ifile;
        basic_string<utf8_char_t> *Ofile;
        basic_string<utf8_char_t> *Rfile;
        void init(int argc, utf8_string_t *argv);
        void usage(void);
};
}
