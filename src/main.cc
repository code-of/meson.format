#/* !main.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <console.hh>

#define PROJECT "meson.format"
#define VERSION "1.0.3"
#define LICENSE "MIT"
#define AUTHOR  "Marcel Bobolz <ergotamin.source@gmail.com>"

int main(int argc, char **argv)
{
    meson::Console console(argc, argv);

    return console.run();
}
