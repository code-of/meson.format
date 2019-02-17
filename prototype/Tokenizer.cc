#/* !Tokenizer.cc */
#// (c) 2019 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <map>
#include <string>
#include <iostream>
#include <boost/phoenix/phoenix.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

using namespace std::string_literals;
namespace phx = boost::phoenix;
namespace xpr = boost::xpressive;

#define x(c) xpr::as_xpr(c)
#define xref(ref) xpr::by_ref(ref)
#define xset(...) xpr::set[__VA_ARGS__]
#define xmark(tag) xpr::mark_tag tag
#define xrange(begin, end) xpr::range(begin, end)

inline namespace Grammar {
static const xpr::sregex _if =
    x('i') >> x('f');

static const xpr::sregex _else =
    x('e') >> x('l') >> x('s') >> x('e');

static const xpr::sregex _elif =
    x('e') >> x('l') >> _if;

static const xpr::sregex _endif =
    x('e') >> x('n') >> x('d') >> _if;

static const xpr::sregex _true =
    x('t') >> x('r') >> x('u') >> x('e');

static const xpr::sregex _false =
    x('f') >> x('a') >> x('l') >> x('s') >> x('e');

static const xpr::sregex _and =
    x('a') >> x('n') >> x('d');

static const xpr::sregex _of =
    x('o') >> x('r');

static const xpr::sregex _not =
    x('n') >> x('o') >> x('t');

static const xpr::sregex _foreach =
    x('f') >> x('o') >> x('r') >> x('e') >> x('a') >> x('c') >> x('h');

static const xpr::sregex _endforeach =
    x('e') >> x('n') >> x('d') >> _foreach;

static const xpr::sregex _in =
    x('i') >> x('n');

static const xpr::sregex _continue =
    x('c') >> x('o') >> x('n') >> x('t') >> _in >> x('u') >> x('e');

static const xpr::sregex _break =
    x('b') >> x('r') >> x('e') >> x('a') >> x('k');

static const xpr::sregex _assign =
    x('=');

static const xpr::sregex _plusassign =
    x('+') >> x('=');

static const xpr::sregex _equal =
    x('=') >> x('=');

static const xpr::sregex _nequal =
    x('!') >> x('=');
static const xpr::sregex _le =
    x('<') >> x('=');

static const xpr::sregex _ge =
    x('>') >> x('=');

static const xpr::sregex identifier =
    xset(x('_') | xpr::alpha) >> *xset(x('_') | xrange('0', '9') | xpr::alpha);

static const xpr::sregex number =
    x('0') >> xset(x('b') | x('B')) >> +xset(x('0') | x('1')) |
    x('0') >> xset(x('o') | x('O')) >> +xrange('0', '7') |
    x('0') >> xset(x('x') | x('X')) >> +xpr::xdigit |
    xrange('1', '9') >> *xpr::digit |
    x('0');

static const xpr::sregex string =
    x('\'') >> *(~xset(x('\'') | x('\\')) | x('\\') >> xpr::_) >> x('\'');

static const xpr::sregex multiline_string =
    xpr::repeat<3, 3>(x('\'')) >> *(~xpr::_ln | xpr::_ln) >> xpr::repeat<3, 3>(x('\''));

static const xpr::sregex comment =
    x('#') >> *~xpr::_ln;

static const xpr::sregex function =
    *xpr::space >> identifier >> *xpr::space >> x('(');

static const xpr::sregex assignment =
    *xpr::space >> identifier >> *xpr::space >> (_plusassign | _assign)
    >> *xpr::space >> (function | number | string | x('{') | x('[')) >> *xpr::space;
}

struct distance_func {
    template <typename Iterator1, typename Iterator2>
    struct result : boost::iterator_difference<Iterator1> {};

    template <typename Iterator1, typename Iterator2>
    typename result<Iterator1, Iterator2>::type
    operator()(Iterator1 const& begin, Iterator2 const& end) const
    {
        return std::distance(begin, end);
    }
};
phx::function<distance_func> const distance = distance_func();
