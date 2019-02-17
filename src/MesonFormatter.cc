#/* !formatter.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <regex>
#include <MesonFormatter.hh>
#include <MesonIterator.hh>


extern const char *__progname;

namespace Meson {
using namespace std;

static StreamIterator *iterator;
static basic_stringstream<utf8_char_t> *sstream;

Formatter::Formatter(void)
{
    this->indent = 0;
}

Formatter::~Formatter(void)
{
}

void Formatter::read(basic_istream<utf8_char_t> *_is)
{
    utf8_char_t _c = 0;

    sstream = new basic_stringstream<utf8_char_t>;

    _is->get(_c);
    while (!_is->eof()) {
        sstream->put(_c);
        _is->get(_c);
    }

    try {
        if (!sstream->good())
            throw it("Failure while reading from input.");
    } catch (const basic_string<utf8_char_t>& it) {
        delete sstream;
        error(it.c_str());
        exit(EXIT_FAILURE);
    }
}

void Formatter::read(basic_string<utf8_char_t> path)
{
    basic_ifstream<utf8_char_t> is(path, ios::in);

    try {
        if (!is.is_open())
            throw it("Could not open file :");
    } catch (const basic_string<utf8_char_t>& it) {
        error(it.c_str()) << path << endl;
        exit(EXIT_FAILURE);
    }

    this->read(&is);
    is.close();
}

void Formatter::write(basic_ostream<utf8_char_t>& _os)
{
    iterator = new StreamIterator(sstream);

    while (iterator->hasLine())
        this->_write(_os, this->_format(iterator->nextLine()));

    sstream->clear();

    delete sstream;
    delete iterator;
}

void Formatter::write(basic_string<utf8_char_t> path)
{
    basic_ofstream<utf8_char_t> os(path, ios::out | ios::trunc);

    try {
        if (!os.is_open())
            throw it("Could not open std::ios::ofstream for:");
    } catch (const basic_string<utf8_char_t>& it) {
        error(it.c_str()) << path << endl;
        exit(EXIT_FAILURE);
    }

    this->write(os);

    try {
        os.close();
        if (!os.good())
            throw it("Got std::ios::ios_base::failbit at std::ios::ofstream for:");
    } catch (const basic_string<utf8_char_t>& it) {
        error(it.c_str()) << path << endl;
        exit(EXIT_FAILURE);
    }
}

basic_string<utf8_char_t> *Formatter::_format(basic_string<utf8_char_t> *s)
{
    basic_string<utf8_char_t> *_s = new basic_string<utf8_char_t>(s->c_str());

    delete s;

    __HINT__("> break/continue");

    if (this->_match(_s, u8R"(^\s*(break|continue)\s*)")) {
        this->_replace(_s, u8R"(\s*(\w+)[\t ]*)", " $1");

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+(\w+))", ind->c_str());
        delete ind;

        return _s;
    }

    __HINT__("> endif/endforeach");

    if (this->_match(_s, u8R"(^\s*(endif|endforeach)\s*)")) {
        this->_replace(_s, u8R"(\s*(\w+)[\t ]*)", " $1");

        this->indent -= 2;

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+(\w+))", ind->c_str());
        delete ind;

        return _s;
    }

    __HINT__("> else");

    if (this->_match(_s, u8R"(^\s*(else)\s*)")) {
        this->_replace(_s, u8R"(^\s*(\w+)[\t ]*)", " $1");

        this->indent -= 2;

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+(\w+))", ind->c_str());
        delete ind;

        this->indent += 2;

        return _s;
    }

    __HINT__("> if/elif/foreach");

    if (this->_match(_s, u8R"(^\s*(if|elif|foreach)\s*)")) {
        this->_replace(_s, u8R"(^\s*(if|elif|foreach)[\t ]*)", " $1 ");

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+(if|elif|foreach))", ind->c_str());
        delete ind;

        __HINT__("> whitespace around (keyword-)comparison, increment, decrement...");
        if (this->_match(_s, u8R"(^\s*(if|elif))"))
            this->_format_ifcase(_s);
        else
            this->_format_foreach(_s);

        this->indent += 2;

        return _s;
    }

    if (this->_match(_s, u8R"(^\s*([_a-zA-Z][._0-9a-zA-Z]*\s*?\(\s*))")) {
        this->_replace(_s, u8R"(\s*([_a-zA-Z][._0-9a-zA-Z]*)\s*(\()\s*)", " $1$2");

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+([_a-zA-Z][._0-9a-zA-Z]*))", ind->c_str());
        delete ind;

        if (this->_match(_s, u8R"(\s*([_a-zA-Z][._0-9a-zA-Z]*\(\s*\)))")) {
            if (_s->at(_s->length() - 1) != '\n')
                _s->insert(_s->end(), 1, '\n');
        } else {
            while ((this->_count('(', _s)) != (this->_count(')', _s))) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw it("Missing closing parenthesis ')' !");
                } catch (const basic_string<utf8_char_t>& it) {
                    delete _s;
                    error(it.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            this->_format_v('(', _s);
        }
        return _s;
    }

    if (this->_match(_s, u8R"(^\s*([_a-zA-Z][_0-9a-zA-Z]*\s*=\s*))")) {
        this->_replace(_s, u8R"(\s*([_a-zA-Z][_0-9a-zA-Z]*)[\t ]*=[\t ]*)", " $1 = ");

        basic_string<utf8_char_t> *ind = this->_indent("$1");
        this->_replace(_s, u8R"(^\s+([_a-zA-Z][_0-9a-zA-Z]*))", ind->c_str());
        delete ind;

        if (this->_match(_s, u8R"(=\s+[_a-zA-Z][._0-9a-zA-Z]*\s*\(\s*\))")) {
            if (_s->at(_s->length() - 1) != '\n')
                _s->insert(_s->end(), 1, '\n');
        } else if (this->_match(_s, u8R"(=\s+[_a-zA-Z][._0-9a-zA-Z]*\s*\(\s*)")) {
            while ((this->_count('(', _s)) != (this->_count(')', _s))) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw it("Missing closing parenthesis ')' !");
                } catch (const basic_string<utf8_char_t>& it) {
                    delete _s;
                    error(it.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            this->_format_v('(', _s);
        } else if (this->_match(_s, u8R"(\s*[_a-zA-Z][_0-9a-zA-Z]*\s*=\s*\{\s*)")) {
            while ((this->_count('{', _s)) != (this->_count('}', _s))) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw it("Missing closing curly brace '}' !");
                } catch (const basic_string<utf8_char_t>& it) {
                    delete _s;
                    error(it.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            this->_format_v('{', _s);
        } else if (this->_match(_s, u8R"(\s*[_a-zA-Z][_0-9a-zA-Z]*\s*=\s*\[\s*)")) {
            while ((this->_count('[', _s)) != (this->_count(']', _s))) {
                try {
                    if (iterator->hasLine())
                        _s->append(iterator->nextLine()->c_str());
                    else
                        throw it("Missing closing square brace ']' !");
                } catch (const basic_string<utf8_char_t>& it) {
                    delete _s;
                    error(it.c_str());
                    exit(EXIT_FAILURE);
                }
            }

            this->_format_v('[', _s);
        }
        return _s;
    }

    return _s;
}

void Formatter::_format_ifcase(basic_string<utf8_char_t> *_s)
{
    pos_t i = 0;
    pos_t lit = 0;
    pos_t cstart = _s->find_first_of("if", 0UL, 2UL) + 3;
    basic_string<utf8_char_t> *_sub;
    basic_string<utf8_char_t> *sub = new basic_string<utf8_char_t>(_s->substr(cstart));

    for (i = (__s->length() - 1); i != 0; i--) {
        if (__s->at(i) == '\'')
            lit = !lit;
        if (!lit) {
            if (__s->at(i) != '\n')
                sub = new basic_string<utf8_char_t>(__s->substr(i, (__s->length() - i)));
            else
                continue;
        }
    }

    // if (this->_match(sub, u8R"(^[ ]?[_0-9a-zA-Z\.\)\(]+)")) {
    //     delete sub;
    // } else if (this->_match(sub, u8R"(^[\+=><!]=)")) {
    //     __s->insert(i, " ");
    //     if (!this->_match(sub, u8R"(^[^ ][\+=><!]=[ ])"))
    //         __s->insert(i + 3, " ");

    //     delete sub;
    // } else if (this->_match(sub, u8R"(^[-\+\*%/><:=])")) {
    //     __s->insert(i, " ");

    //     if (!this->_match(sub, u8R"(^[-\+\*%/><:=][ ])"))
    //         __s->insert(i + 2, " ");

    //     delete sub;
    // } else if (this->_match(sub, u8R"(^[ ]{2})")) {
    //     __s->erase(i, 1);
    //     delete sub;
    // } else if (this->_match(sub, u8R"(^\s+[,])")) {
    //     if (!this->_match(sub, u8R"(^\s+[,][ ])"))
    //         __s->insert(i + 2, " ");

    //     __s->erase(i, 1);
    //     delete sub;
    // } else {
    //     delete sub;
    // }


    _s->erase(_s->begin() + cstart, _s->end());
    _s->replace(cstart, sub->length(), sub->c_str());

    if (_s->at(_s->length() - 1) != '\n')
        _s->insert(_s->end(), 1, '\n');

    delete sub;
}

void Formatter::_format_foreach(basic_string<utf8_char_t> *_s)
{
    return;
}

void Formatter::_format_v(utf8_char_t _c, basic_string<utf8_char_t> *_s)
{
    pos_t i = 0;
    pos_t lit = 0;
    pos_t vstart = _s->find(_c);
    basic_string<utf8_char_t> *__s = new basic_string<utf8_char_t>(_s->substr(vstart));

    for (i = (__s->length() - 1); i != 0; i--) {
        if (__s->at(i) == '\'')
            lit = !lit;
        else if ((__s->at(i) == '\n') && !lit)
            __s->erase(i, 1);
        else if ((__s->at(i) == ' ') && !lit)
            __s->erase(i, 1);
    }

    lit = 0;

    if (_c == '{') {
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

        basic_string<utf8_char_t> *ind = this->_indent(NULL);

        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '\n') && !lit)
                __s->insert(i + 1, ind->c_str());
        }

        delete ind;

        this->indent -= 2;
    }
    if (_c == '[') {
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

        basic_string<utf8_char_t> *ind = this->_indent(NULL);

        for (i = (__s->length() - 1); i != 0; i--) {
            if (__s->at(i) == '\'')
                lit = !lit;
            else if ((__s->at(i) == '\n') && !lit)
                __s->insert(i + 1, ind->c_str());
        }

        delete ind;

        this->indent -= 2;
    }

    if (_c == '(') {
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

        basic_string<utf8_char_t> *ind = this->_indent(NULL);

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

    if (_s->at(_s->length() - 1) != '\n')
        _s->insert(_s->end(), 1, '\n');

    delete __s;
}

basic_string<utf8_char_t> *Formatter::_indent(utf8_cstring_t _append)
{
    basic_string<utf8_char_t> *_ind = new basic_string<utf8_char_t>();

    for (pos_t i = this->indent; i > 0; i--)
        _ind->append(1, ' ');

    if (NULL != _append)
        _ind->append(_append);

    return _ind;
}

pos_t Formatter::_count(utf8_char_t _c, basic_string<utf8_char_t> *_s)
{
    pos_t i = 0;
    pos_t n = 0;

    for (; i < _s->length(); i++)
        if (_s->at(i) == _c)
            n++;
    return n;
}

void Formatter::_write(basic_ostream<utf8_char_t>& _o, basic_string<utf8_char_t> *_s)
{
    if (_s->length())
        _o.write(_s->c_str(), _s->length());
    else
        _o.write("\n", 1);
    _o.flush();
    delete _s;
}

bool Formatter::_match(basic_string<utf8_char_t> *target, utf8_cstring_t rexp)
{
    match_results<utf8_cstring_t> m;
    basic_regex<utf8_char_t> expr(rexp);

    return regex_search(target->c_str(), m, expr);
}

void Formatter::_replace(basic_string<utf8_char_t> *target, utf8_cstring_t rexp, utf8_cstring_t rfmt)
{
    basic_regex<utf8_char_t> expr(rexp);

    target->assign(regex_replace(target->c_str(), expr, rfmt));
}

basic_ostream<utf8_char_t>& error(basic_string<utf8_char_t> it)
{
    return cerr << "\x1b[38:2:255:20:60m" << it << "\x1b(B\x1b[m" << endl;
}
}
