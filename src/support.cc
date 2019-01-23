#/* !types.cc */
#// (c) 2019 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <cstring>
#include <iostream>
#include <MesonTypes.hh>

namespace Meson {
using namespace std;

const string it(string s)
{
    const string it = s.c_str();

    return it;
}

ostream& error(const char *it)
{
    cerr << "\x1b[38:2:255:20:60m" << it << "\x1b(B\x1b[m" << endl;
    return cerr;
}
}
