#/* !formatter.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <regex>
#include <unistd.h>
#include <getopt.h>
#include <formatter.hh>

extern const char *__progname;

namespace meson {
using namespace std;

static utf8_t _c;
static stringstream sstream;

Formatter::Formatter(void)
{
}

Formatter::~Formatter(void)
{
}

void Formatter::write_to_file(string path)
{
    ofstream ofs(path, ios::out | ios::trunc);

    if (!ofs.is_open())
        throw;

    StreamIterator iterator(&sstream);

    do {
        string *codeBlock = new string;

        while (iterator.hasLine()) {
            codeBlock->append(iterator.nextLine()->c_str());
            if (codeBlock->find(')') != codeBlock->npos)
                break;
        }

        this->write_to(ofs, this->to_format(codeBlock));

        if (!iterator.hasLine())
            break;
    } while (true);

    sstream.clear();
    ofs.close();

    if (ofs.good())
        return;
    else
        throw;
}

void Formatter::read_from_file(string path)
{
    _c = 0;
    sstream.clear();
    ifstream ifs(path, ios::in);

    if (!ifs.is_open())
        throw;

    ifs.get(_c);
    while (!ifs.eof()) {
        sstream.put(_c);
        ifs.get(_c);
    }
    ifs.close();

    if (sstream.good())
        return;
    else
        throw;
}

void Formatter::write_to_stream(ostream& os)
{
    StreamIterator iterator(&sstream);

    do {
        string *codeBlock = new string;
        while (iterator.hasLine()) {
            codeBlock->append(iterator.nextLine()->c_str());
            if (codeBlock->find(')') != codeBlock->npos)
                break;
        }

        this->write_to(os, this->to_format(codeBlock));

        if (!iterator.hasLine())
            break;
    } while (true);

    sstream.clear();
}

void Formatter::read_from_stream(istream *is)
{
    _c = 0;
    sstream.clear();

    is->get(_c);
    while (!is->eof()) {
        sstream.put(_c);
        is->get(_c);
    }

    if (sstream.good())
        return;
    else
        throw;
}

string *Formatter::to_format(string *s)
{
    align_t pos1 = 0;
    align_t pos2 = 0;
    align_t indent = 0;
    string *_s = new string(s->c_str());

    delete s;
    this->replace(_s, "\\s+", "");
    this->replace(_s, "^(\\w+)(=)(\\w+)(\\()", "$1 $2 $3$4");
    this->replace(_s, "(',|'[^']+',)", "$1\n");
    this->replace(_s, "(:(?=\\['))", " $1\n");
    this->replace(_s, "(:(?=')|:(?=\\[)|:(?=\\w))", " $1 ");
    this->replace(_s, "\\](,(?=\\w+[^\\)]))", "]$1\n");
    this->replace(_s, "( : [A-Za-z0-9_]+)(,)([A-Za-z0-9_]+ : )", "$1$2\n$3");

    if (string::npos == (indent = _s->find("(")))
        throw;

    indent++;
    do {
        pos2 = _s->find('\n', pos1);
        if (string::npos == pos2)
            break;
        _s->insert(pos2 + 1, indent, ' ');
        pos1 = pos2 + 1;
    } while (true);

    _s->append("\n\n");

    return _s;
}

void Formatter::write_to(ostream& _os, string *_s)
{
    if (_s->length())
        _os.write(_s->c_str(), _s->length());
    else
        _os.write("\n", 1);
    _os.flush();
    delete _s;
}

void Formatter::replace(string *target, const char *rexp, const char *rfmt)
{
    regex expr(rexp);

    target->assign(regex_replace(target->c_str(), expr, rfmt));
}
}
