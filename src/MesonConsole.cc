#/* !Console.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include <getopt.h>
#include <MesonConsole.hh>

extern const char *__progname;

namespace Meson {
using namespace std;

static int bflag;

static constexpr struct option lopts[] =
{
    { "backup",	 no_argument,		&bflag, 1	},
    { "help",	 no_argument,		0,		'h' },
    { "replace", required_argument, 0,		'r' },
    { "input",	 required_argument, 0,		'i' },
    { "output",	 required_argument, 0,		'o' },
    { 0,		 0,					0,		0	},
};

Console::Console(int argc, char **argv)
{
    this->hasIfile = false;
    this->hasOfile = false;
    this->hasRfile = false;
    this->Ifile = new basic_string<utf8_char_t>;
    this->Ofile = new basic_string<utf8_char_t>;
    this->Rfile = new basic_string<utf8_char_t>;
    this->init(argc, argv);
}

Console::~Console(void)
{
    delete this->Ifile;
    delete this->Ofile;
    delete this->Rfile;
}

void Console::init(int argc, utf8_string_t *argv)
{
    utf8_char_t ch;

    try {
        while (true) {
            int optidx = 0;

            ch = getopt_long(argc, argv, "i:o:r:", lopts, &optidx);

            if (-1 == ch)
                break;

            switch (ch) {
            case 0:
                break;

            case 'h':
                usage();
                break;

            case 'i':
                if (0 == access(optarg, F_OK))
                    this->Ifile->assign(optarg);
                else
                    throw it("Could not access :");
                break;

            case 'o':
                this->Ofile->assign(optarg);
                break;

            case 'r':
                if (0 == access(optarg, F_OK))
                    this->Rfile->assign(optarg);
                else
                    throw it("Could not access :");
                break;

            default:
                error("No such option :") << ch << endl;
                exit(EXIT_FAILURE);
            }
        }
    } catch (const basic_string<utf8_char_t>& it) {
        error(it.c_str()) << optarg << endl;
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        cerr << "Unknown parameter(s):" << endl;
        while (optind < argc)
            cerr << "[" << argv[optind++] << "] ";
        cerr << endl;
    }

    if (bflag)
        cerr << "Original file-state will be preserved !" << endl;

    if (0 < this->Rfile->length())
        this->hasRfile = true;

    if (0 < this->Ifile->length())
        this->hasIfile = true;

    if (0 < this->Ofile->length())
        this->hasOfile = true;
}

void Console::usage(void)
{
    cerr << "    Usage:\n\n";
    cerr << "        " << __progname << "[OPTION(S)] [FILE(S)]\t\n\n";
    cerr << "    Options:\n\n";
    cerr << "      " << "--input, -i FILE    - reads from file and writes to stdout.\n\n";
    cerr << "      " << "--output, -o FILE   - redirects output to FILE.\n\n";
    cerr << "      " << "--replace, -r FILE  - buffered inplace overwrite of FILE.\n\n";
    cerr << "      " << "--backup            - preserves the file with '.orig'-extension.\n";
    cerr << "      " << "                      (only useable during -r|--replace)\n\n";
    cerr << "[ ergotamin (c) 2018 MIT License]\n";
    exit(EXIT_SUCCESS);
}

int Console::run(void)
{
    try {
        if (!(this->hasIfile || this->hasRfile || this->hasOfile)) {
            this->read(&cin);
            this->write(cout);
        } else if (this->hasOfile && !this->hasIfile) {
            this->read(&cin);
            this->write(this->Ofile->c_str());
        } else if (this->hasIfile && !this->hasOfile) {
            this->read(this->Ifile->c_str());
            this->write(cout);
        } else if (this->hasOfile && this->hasIfile) {
            this->read(this->Ifile->c_str());
            this->write(this->Ofile->c_str());
        } else if (this->hasRfile) {
            this->read(this->Rfile->c_str());
            if (bflag) {
                basic_string<utf8_char_t> *backup = new basic_string<utf8_char_t>(this->Rfile->c_str());
                backup->append(".orig");
                rename(this->Rfile->c_str(), backup->c_str());
                delete backup;
            }
            this->write(this->Rfile->c_str());
        } else {
            throw it("An unknown error occured :");
        }
    } catch (const basic_string<utf8_char_t>& it) {
        error(it.c_str()) << __FILE__ << "@" << __LINE__ << endl;
    }
    return EXIT_SUCCESS;
}
}
