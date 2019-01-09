#/* !formeson.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <iostream>

#define PROJECT_NAME "formeson"

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cout << argv[0] << "takes no arguments.\n";
        return 1;
    }
    std::cout << "This is project " << PROJECT_NAME << ".\n";
    return 0;
}
