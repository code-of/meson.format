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
static StreamIterator *iterator;


static void error(string what)
{
    cerr << "\x1b[38:2:255:20:60m" << what << "\x1b(B\x1b[m" << endl;
    exit(EXIT_FAILURE);
}


Formatter::Formatter(void)
{
    this->indent = 0;
}

Formatter::~Formatter(void)
{
}

void Formatter::read(istream *_is)
{
    _c = 0;
    sstream.clear();

    _is->get(_c);
    while (!_is->eof()) {
        sstream.put(_c);
        _is->get(_c);
    }
    try {
        if (sstream.good())
            return;
        else
            throw "Failure while reading from input";
    } catch (const utf8_t *e) {
        error(e);
    }
}

void Formatter::read(string path)
{
    ifstream is(path, ios::in);

    if (!is.is_open())
        throw;

    this->read(&is);

    is.close();
}

void Formatter::write(ostream& _os)
{
    iterator = new StreamIterator(&sstream);

    while (iterator->hasLine())
        this->write_(_os, this->format_(iterator->nextLine()));

    delete iterator;
    sstream.clear();
}

void Formatter::write(string path)
{
    ofstream os(path, ios::out | ios::trunc);

    try {
        if (!os.is_open())
            throw "Could not open std::ofstream";
    } catch (const utf8_t *e) {
        error(e);
    }

    this->write(os);

    try {
        os.close();
        if (!os.good())
            throw "std::ofstream got failbit";
    } catch (const utf8_t *e) {
        error(e);
    }
}

string *Formatter::format_(string *s)
{
    string *_s = new string(s->c_str());

    delete s;

    if (this->match_(_s, "^\\s*(if|elif|else|foreach)\\s*")) {
        this->replace_(_s, "\\s*(\\S+)[\t\\ ]*", " $1");

        string *ind = this->indent_("$1");
        this->replace_(_s, "^\\s+(\\S+)", ind->c_str());
        delete ind;

        this->indent += 2;

        return _s;
    }

    if (this->match_(_s, "^\\s*(break|continue)")) {
        this->replace_(_s, "\\s*(\\w+)[\t\\ ]*", " $1");

        string *ind = this->indent_("$1");
        this->replace_(_s, "^\\s*(\\w+)", ind->c_str());
        delete ind;

        return _s;
    }

    if (this->match_(_s, "^\\s*(endif|endforeach)\\s*")) {
        this->replace_(_s, "\\s*(\\w+)[\t\\ ]*", " $1");

        this->indent -= 2;

        string *ind = this->indent_("$1");
        this->replace_(_s, "^\\s*(\\w+)", ind->c_str());
        delete ind;

        return _s;
    }

    if (this->match_(_s, "^\\s*(\\w+\\s*\\(\\s*)")) {
        this->replace_(_s, "\\s*(\\w+)\\s*(\\()\\s*", "$1$2");

        string *ind = this->indent_("$1");
        this->replace_(_s, "^\\s+(\\S*)", ind->c_str());
        delete ind;

        if (this->match_(_s, "\\s*\\w+\\s*\\(\\s*\\)")) {
            if (_s->at(_s->length() - 1) != '\n')
                _s->insert(_s->end(), 1, '\n');
        } else {
            while (_s->find(')') == string::npos) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw "Missing parenthesis ')' !";
                } catch (const utf8_t *e) {
                    error(e);
                }
            }
            this->format_v('(', _s);
        }
        return _s;
    }

    if (this->match_(_s, "^\\s*(\\w+\\s*=\\s*)")) {
        this->replace_(_s, "\\s*(\\w+)[\t\\ ]*=[\t\\ ]*", " $1 = ");

        string *ind = this->indent_("$1");
        this->replace_(_s, "^\\s+(\\S*)", ind->c_str());
        delete ind;

        if (this->match_(_s, "=\\s*\\w+\\s*\\(\\s*\\)")) {
            if (_s->at(_s->length() - 1) != '\n')
                _s->insert(_s->end(), 1, '\n');
        } else if (this->match_(_s, "\\s*\\w+\\s*\\(\\s*")) {
            while (_s->find(')') == string::npos) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw "Missing parenthesis ')' !";
                } catch (const utf8_t *e) {
                    error(e);
                }
            }
            this->format_v('(', _s);
        } else if (this->match_(_s, "\\s*\\w+\\s*=\\s*\\{\\s*")) {
            while (_s->find('}') == string::npos) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw "Missing parenthesis '}' !";
                } catch (const utf8_t *e) {
                    error(e);
                }
            }
            this->format_v('{', _s);
        } else if (this->match_(_s, "\\s*\\w+\\s*=\\s*\\[\\s*")) {
            while ((this->count_('[', _s)) != (this->count_(']', _s))) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw "Missing parenthesis ']' !";
                } catch (const utf8_t *e) {
                    error(e);
                }
            }

            this->format_v('[', _s);
        }
        return _s;
    }

    return _s;
}

void Formatter::format_v(utf8_t __c, string *_s)
{
    align_t i = 0;
    unsigned int lit = 0;
    align_t vstart = _s->find(__c);
    string *__s = new string(_s->substr(vstart));

    for (i = (__s->length() - 1); i != 0; i--) {
        if (__s->at(i) == '\'')
            lit = !lit;
        else if ((__s->at(i) == '\n') && !lit)
            __s->erase(i, 1);
        else if ((__s->at(i) == ' ') && !lit)
            __s->erase(i, 1);
    }

    lit = 0;

    if (__c == '{') {
        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '}') && !lit)
                __s->insert(i + 1, "\n");
            else if ((__s->at(i) == '{') && !lit)
                __s->insert(i - 1, "\n");
            else if ((__s->at(i) == ',') && !lit)
                __s->insert(i + 1, "\n");
        }
        lit = 0;

        this->indent += 2;

        string *ind = this->indent_(NULL);

        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '\n') && !lit)
                __s->insert(i + 1, ind->c_str());
        }

        delete ind;

        this->indent -= 2;
    }
    if (__c == '[') {
        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'') {
                lit = !lit;
            } else if ((__s->at(i) == '[') && !lit) {
                if ((__s->at(i - 1)) == ',')
                    __s->insert(i, "\n");
                else
                    continue;
            }
        }
        lit = 0;

        this->indent += 2;

        string *ind = this->indent_(NULL);

        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '\n') && !lit)
                __s->insert(i + 1, ind->c_str());
        }

        delete ind;

        this->indent -= 2;
    }

    if (__c == '(') {
        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'') {
                lit = !lit;
            } else if ((__s->at(i) == ',') && !lit) {
                __s->insert(i + 1, "\n");
            } else if ((__s->at(i) == ':') && !lit) {
                __s->insert(i, " ");
                __s->insert(i + 2, " ");
            }
        }

        lit = 0;

        this->indent += 2;

        string *ind = this->indent_(NULL);

        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '\n') && !lit)
                __s->insert(i + 1, ind->c_str());
        }

        delete ind;

        this->indent -= 2;
    }

    _s->erase(_s->begin() + vstart, _s->end());
    _s->replace(vstart, __s->length(), __s->c_str());
    this->replace_(_s, "(\\w+)[\t\\ ]*(\\(|\\{|\\[)[\t\\ ]*", "$1$2");

    if (_s->at(_s->length() - 1) != '\n')
        _s->insert(_s->end(), 1, '\n');

    delete __s;
}

string *Formatter::indent_(const utf8_t *_append)
{
    string *_ind = new string();

    for (align_t i = this->indent; i > 0; i--)
        _ind->append(1, ' ');

    if (NULL != _append)
        _ind->append(_append);

    return _ind;
}

align_t Formatter::count_(utf8_t __c, string *_s)
{
    align_t i = 0;
    align_t n = 0;

    for (; i < _s->length(); i++)
        if (_s->at(i) == __c)
            n++;
    return n;
}

void Formatter::write_(ostream& _o, string *_s)
{
    if (_s->length())
        _o.write(_s->c_str(), _s->length());
    else
        _o.write("\n", 1);
    _o.flush();
    delete _s;
}

bool Formatter::match_(string *target, const utf8_t *rexp)
{
    cmatch m;
    regex expr(rexp);

    return regex_search(target->c_str(), m, expr);
}

void Formatter::replace_(string *target, const utf8_t *rexp, const utf8_t *rfmt)
{
    regex expr(rexp);

    target->assign(regex_replace(target->c_str(), expr, rfmt));
}
}
