#/* !iterator.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <formatter.hh>

namespace meson {
using namespace std;

StreamIterator::StreamIterator(stringstream *in)
{
    this->inStream = in;
}

StreamIterator::~StreamIterator()
{
}

string *StreamIterator::nextLine(void)
{
    string *obuf;
    utf8_t *tmp = new utf8_t [256];

    if (!this->inStream->eof()) {
        this->inStream->getline((char *)tmp, 255, '\n');
        obuf = new string((const char *)tmp);
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
