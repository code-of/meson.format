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

static void replace(string *target, const char *rexp, const char *rfmt);

Formatter::Formatter(void)
{
    this->scope = false;
    this->stage2 = false;
    this->indent = 0;
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

    while (iterator.hasLine())
        this->write_to(ofs, this->format_from(iterator.nextLine()));

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

    while (iterator.hasLine())
        this->write_to(os, this->format_from(iterator.nextLine()));

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

void Formatter::write_to(ostream& _os, string *_s)
{
    if (_s->length())
        _os.write(_s->c_str(), _s->length());
    else
        _os.write("\n", 1);
    _os.flush();
    delete _s;
}

string *Formatter::format_from(string *s)
{
    string *out = new string;

    out->assign(s->c_str());
    delete s;
    // TAB remove
    replace(out, "^\t*", "");
    // SPACE prefixed/suffixed Colons (2x to merge )
    replace(out, "\\s+(:)(\\s(?!\n))*", " $1 ");
    // Remove trailing Whitespaces and align Commas
    replace(out, "\\s+(,)\\s+$", "$1\n");
    // Remove prepended and trailing Whitespaces inside square-brackets
    replace(out, "(\\[)\\s*", "$1");
    replace(out, "\\s*(\\],?)", "$1");
    // SPACE around Variable Assignment
    replace(out, "^\\s*(\\w+)\\s*(=)\\s*(\\w+)\\s*(\\()\\s*", "$1 $2 $3$4");
    // 2 Args on the same line as Function
    replace(out, "\\s*(\\()\\s*('?.+?'?)\\s*(,)\\s*('?.+?'?)\\s*(,|\\))\\s*$", "$1$2$3 $4$5\n");
    if (true == this->stage2) {
        this->stage2 = false;
        return out;
    }
    if (false == this->stage2) {
        this->stage2 = true;
        out = this->format_from(out);
    }
    if (this->scope) {
        align_t sc = 0;
        do
            if (out->at(sc++) != ' ') {
                sc--;
                break;
            }
        while (true);

        if (sc < this->indent)
            out->insert(0, this->indent - (sc - 1), ' ');

        if (sc > this->indent) {
            out->erase(0, sc);
            out->insert(0, this->indent, ' ');
        }

        if (out->npos != out->find(')')) {
            this->scope = false;
            this->indent = 0;
        }
    }

    if (out->npos != out->find('(')) {
        if (out->npos == out->find(')')) {
            this->indent = out->find('(') + 1UL;
            this->scope = true;
        }
    }

    return out;
}

static void replace(string *target, const char *rexp, const char *rfmt)
{
    regex expr(rexp);

    target->assign(regex_replace(target->c_str(), expr, rfmt));
}
}
