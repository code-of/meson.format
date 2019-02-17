#/* !iterator.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <MesonIterator.hh>
#include <MesonTypes.hh>

namespace Meson {
using namespace std;

StreamIterator::StreamIterator(basic_stringstream<utf8_char_t> *in)
{
    this->inStream = in;
}

StreamIterator::~StreamIterator()
{
}

basic_string<utf8_char_t> *StreamIterator::nextLine(void)
{
    basic_string<utf8_char_t> *obuf;
    utf8_string_t tmp = new utf8_char_t [256];

    if (!this->inStream->eof()) {
        this->inStream->getline(static_cast<utf8_string_t>(tmp), 255, '\n');
        obuf = new basic_string<utf8_char_t>(static_cast<utf8_cstring_t>(tmp));
        delete[] tmp;
    }
    this->inStream->peek();
    if (false == this->inStream->good()) {
        return obuf;
    } else {
        obuf->append("\n");
        return obuf;
    }
}
}
