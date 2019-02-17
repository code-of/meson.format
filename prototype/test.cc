#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <boost/algorithm/string_regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
//
//  combined namespace.
namespace Boost {
//
//  scoped namespaces.
    using namespace boost::algorithm;
    using namespace boost::xpressive;
}

//
//  the internal inline Grammar namespace.
inline namespace Grammar  __attribute__((visibility("internal"))){
//
//  templated wrapper funcions as alias binding.
    template<typename BidiIter>
    inline typename boost::proto::terminal
    <boost::reference_wrapper
     <Boost::basic_regex<BidiIter> const> >::type
    const xref(Boost::basic_regex<BidiIter> const &rex)
    {
        return Boost::by_ref(rex);
    }

    template<typename Char>
    inline typename boost::proto::terminal
    <boost::xpressive::detail::range_placeholder
     <Char> >::type
    const xrange(Char begin, Char end)
    {
        return Boost::range(begin, end);
    }

    typedef struct boost::proto::functional::as_expr<> char_expr;
    char_expr const x = {};

    typedef boost::xpressive::detail::set_initializer_type xset_initializer;
    xset_initializer const xset = { {} };

    typedef struct boost::xpressive::mark_tag xmark;
//
//  protected inline namespace for the basic language semantic token.
    inline namespace  Token __attribute__((visibility("protected"))){
//
//  an alias for a basic_regex<std::string::const_iterator>
    typedef Boost::sregex Token_t;
//
//  constant language token
    static const Token_t _if =
        x('i') >> x('f');

    static const Token_t _else =
        x('e') >> x('l') >> x('s') >> x('e');

    static const Token_t _elif =
        x('e') >> x('l') >> _if;

    static const Token_t _endif =
        x('e') >> x('n') >> x('d') >> _if;

    static const Token_t _true =
        x('t') >> x('r') >> x('u') >> x('e');

    static const Token_t _false =
        x('f') >> x('a') >> x('l') >> x('s') >> x('e');

    static const Token_t _and =
        x('a') >> x('n') >> x('d');

    static const Token_t _of =
        x('o') >> x('r');

    static const Token_t _not =
        x('n') >> x('o') >> x('t');

    static const Token_t _foreach =
        x('f') >> x('o') >> x('r') >> x('e') >> x('a') >> x('c') >> x('h');

    static const Token_t _endforeach =
        x('e') >> x('n') >> x('d') >> _foreach;

    static const Token_t _in =
        x('i') >> x('n');

    static const Token_t _continue =
        x('c') >> x('o') >> x('n') >> x('t') >> _in >> x('u') >> x('e');

    static const Token_t _break =
        x('b') >> x('r') >> x('e') >> x('a') >> x('k');

    static const Token_t _assign =
        x('=');

    static const Token_t _colon =
        x(':');

    static const Token_t _plusassign =
        x('+') >> x('=');

    static const Token_t _equal =
        x('=') >> x('=');

    static const Token_t _nequal =
        x('!') >> x('=');

    static const Token_t _le =
        x('<') >> x('=');

    static const Token_t _ge =
        x('>') >> x('=');

    static const Token_t _allowed_space =
        *Boost::space >> -!Boost::_ln >> *Boost::space;

    static const Token_t comment =
        x('#') >> *~Boost::_ln;

    static const Token_t identifier =
        xset[x('_') | Boost::alpha] >> *xset[x('_') | Boost::digit | Boost::alpha];

    static const Token_t number =
        x('0') >> xset[x('b') | x('B')] >> +xset[x('0') | x('1')] |
        x('0') >> xset[x('o') | x('O')] >> +xrange('0', '7') |
        x('0') >> xset[x('x') | x('X')] >> +Boost::xdigit |
        xrange('1', '9') >> *Boost::digit |
        x('0');

    static const Token_t boolean =
        _true | _false;

    static const Token_t string =
        x('\'')
        >> *(~xset[x('\'') | x('\\')]
             | x('\\'))
        >> x('\'');

    static const Token_t multiline_string =
        Boost::repeat<3, 3>(x('\''))
        >> *(~Boost::_ln | Boost::_ln)
        >> Boost::repeat<3, 3>(x('\''));

    static const Token_t constants =
        boolean
        | number
        | string
        | multiline_string;

    static const Token_t value =
        identifier
        | constants;
}

    static Token_t kwarg,
                   kvar, kvars, dict,
                   element, elements, array,
                   param, params, plist,
                   function;

    template<typename Tp = void>
    struct argsof_func {
        typedef Tp result_type;

        template<typename Match>
        void operator()(Match const &cname, Match const &cargs) const
        {
            std::cout << "Call -> " << cname << std::endl;
            std::cout << "Arguments ->" << cargs << std::endl;
        }
    };
    Boost::function<argsof_func<> >::type const argsof = { {} };

    struct load_grammar {
        void operator()(void)
        {
            kwarg =
                identifier
                >> _allowed_space
                >> _colon
                >> _allowed_space
                >> (xref(dict) | xref(array) | value);
            kvar =
                string
                >> _allowed_space
                >> _colon
                >> _allowed_space
                >> (xref(array) | value);
            kvars =
                _allowed_space
                >> !(kvar)
                >> *(*Boost::space
                     >> x(',')
                     >> xref(kvars));
            dict =
                x('{')
                >> kvars
                >> x('}');
            element =
                dict | value;
            elements =
                _allowed_space
                >> !(element)
                >> *(*Boost::space
                     >> x(',')
                     >> xref(elements));
            array =
                x('[')
                >> elements
                >> x(']');
            param =
                xref(function) | kwarg | array | value;
            params =
                _allowed_space
                >> !(param)
                >> *(*Boost::space
                     >> x(',')
                     >> xref(params));
            plist =
                *(Boost::space)
                >> x('(')
                >> params
                >> x(')');
            function =
                !(string | identifier)
                >> !x('.')
                >> identifier
                >> plist | xref(function);
        }
    };

    void match(std::string *it, Token_t with)
    {
        Boost::smatch m;
        std::string _it = std::string(it->c_str());

        if (Boost::regex_search(_it, m, with)) {
            std::cout << "------(results " << 0 << "-" << m.size() << ")-------" << std::endl;

            for (size_t n = 0; n < m.size(); n++)
                std::cout << "match[" << n << "] " << m[n] << std::endl;
        }
    }
}
int main(int, const char **argv)
{
    argv++;
    load_grammar()();
    std::string *s = new std::string(*argv);
    match(s, Grammar::function);
    return 0;
}
