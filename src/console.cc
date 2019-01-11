#/* !Console.cc */
#// (c) 2018 MIT License
#// Marcel Bobolz
#// <ergotamin.source@gmail.com>

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <console.hh>

extern const char *__progname;

namespace meson {
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
    this->Ifile = new string;
    this->Ofile = new string;
    this->Rfile = new string;
    this->init(argc, argv);
}

Console::~Console(void)
{
    delete this->Ifile;
    delete this->Ofile;
    delete this->Rfile;
}

void Console::init(int argc, char **argv)
{
    int ch;

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
                this->Ifile->assign(string(optarg));
            else
                throw;
            break;

        case 'o':
            this->Ofile->assign(string(optarg));
            break;

        case 'r':
            if (0 == access(optarg, F_OK))
                this->Rfile->assign(string(optarg));
            else
                throw;
            break;

        default:
            exit(EXIT_FAILURE);
        }
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
    cerr << "[by: ergotamin (c) 2018 MIT License]\n";
    exit(EXIT_SUCCESS);
}

int Console::run(void)
{
    if (!(this->hasIfile || this->hasRfile || this->hasOfile)) {
        this->read_from_stream(&cin);
        this->write_to_stream(cout);
    } else if (this->hasOfile && !this->hasIfile) {
        this->read_from_stream(&cin);
        this->write_to_file(this->Ofile->c_str());
    } else if (this->hasIfile && !this->hasOfile) {
        this->read_from_file(this->Ifile->c_str());
        this->write_to_stream(cout);
    } else if (this->hasOfile && this->hasIfile) {
        this->read_from_file(this->Ifile->c_str());
        this->write_to_file(this->Ofile->c_str());
    } else if (this->hasRfile) {
        this->read_from_file(this->Rfile->c_str());
        if (bflag) {
            string *backup = new string(this->Rfile->c_str());
            backup->append(".orig");
            rename(this->Rfile->c_str(), backup->c_str());
            delete backup;
        }
        this->write_to_file(this->Rfile->c_str());
    } else {
        throw;
    }
    return EXIT_SUCCESS;
}
}
