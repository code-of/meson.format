#/* !main.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <formatter.hh>

#define PROJECT "meson.format"
#define VERSION "1.0.0a"
#define AUTHOR  "Marcel Bobolz <ergotamin.source@gmail.com>"

int main(int argc, char **argv)
{
    meson::Formatter formatter(argc, argv);

    return formatter.run();
}
