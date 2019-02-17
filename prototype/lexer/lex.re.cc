#/* !lex.re.cc */
#// (c) 2019 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>
#include <header>

/*!re2c
*/
int main(int argc, char **argv)
{
    argv++;
    const char *cstr = "This and that";
    printf("\n%s\n", mem::strcat(cstr, "is", "new"));
    return 0;
}
