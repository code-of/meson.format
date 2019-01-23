#pragma once
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
static_assert(__GNUG__, "Your compiler is not supporting GnuExtensions !");

#include <string>
#include <ostream>

namespace Meson {
using namespace std;

typedef char utf8_t;

typedef unsigned long align_t;

typedef unsigned long pos_t;

const string it(string s);

ostream& error(const char *it);
}
