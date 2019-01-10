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

vector<string> *inp_v;
vector<string> *out_v;
vector<string> *rep_v;

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

static void replace(string *target, const char *rexp, const char *rfmt);

static void usage(void)
{
    cerr << "Usage:\n\t" << __progname << "[OPTION(S)] [FILE(S)]\t- reads input from stdin and writes to stdout.\n";
    cerr << "Options:\n\t" << "--input, -i FILE\t- reads from file and writes to stdout.\n";
    cerr << "\t" << "--output, -o FILE\t- redirects output to FILE.\n";
    cerr << "\t" << "--replace, -r FILE\t- buffered inplace overwrite of FILE.\n";
    cerr << "\t" << "--backup\t- preserves the original file with .orig-extension. (only useable with -r|--replace)\n";
    cerr << "\t" << "\t\tergotamin (c) 2018 MIT License\n";
    exit(EXIT_SUCCESS);
}

StreamIterator::StreamIterator(stringstream *in)
{
    this->inStream = in;
    this->inStream->seekg(0, this->inStream->end);
    this->streamLength = this->inStream->tellg();
    this->inStream->seekg(0, this->inStream->beg);
}

StreamIterator::~StreamIterator()
{
}

int StreamIterator::getStreamLength(void) const
{
    return static_cast<int>(this->streamLength);
}

string *StreamIterator::nextLine(void)
{
    utf8_t *tBuf = new utf8_t [256];

    if (!this->inStream->eof()) {
        this->inStream->getline((char *)tBuf, 255, '\n');
        this->iBuf = new string((const char *)tBuf);
        delete[] tBuf;
    }
    this->inStream->peek();
    if (false == this->inStream->good()) {
        return this->iBuf;
    } else {
        this->iBuf->append("\n");
        return this->iBuf;
    }
}

streamoff StreamIterator::tellg(void)
{
    return this->inStream->tellg();
}

Formatter::Formatter(int argc, char **argv)
{
    this->iFile = NULL;
    this->oFile = NULL;
    this->hasIfile = false;
    this->hasOfile = false;
    this->hasRfile = false;
    this->fScope = false;
    this->indent = 0;
    this->parseCmdLine(argc, argv);
}

Formatter::~Formatter(void)
{
}

int Formatter::run(void)
{
    utf8_t ch = 0;

    this->buf.clear();

    if (!(this->hasIfile || this->hasRfile || this->hasOfile)) {
        istream *input = &cin;

        input->get(ch);
        while (!input->eof()) {
            this->buf.put(ch);
            input->get(ch);
        }

        StreamIterator iterator(&(this->buf));
        while (iterator.hasLine())
            this->printLine(this->format(iterator.nextLine()));

        this->buf.clear();
        return EXIT_SUCCESS;
    }

    if (this->hasOfile && !this->hasIfile) {
        for (size_t n = 0; n < out_v->size(); n++) {
            istream *input = &cin;

            input->get(ch);
            while (!input->eof()) {
                this->buf.put(ch);
                input->get(ch);
            }

            this->oFile = new ofstream(out_v->at(n), ios::out | ios::trunc);

            if (!this->oFile->is_open())
                throw;

            StreamIterator iterator(&(this->buf));
            while (iterator.hasLine())
                this->writeLine(this->format(iterator.nextLine()));
            this->oFile->close();
            delete this->oFile;

            this->buf.clear();
            this->fScope = false;
        }
        this->hasIfile = false;
    }

    if (this->hasIfile && !this->hasOfile) {
        for (size_t n = 0; n < inp_v->size(); n++) {
            this->iFile = new ifstream(inp_v->at(n), ios::in);

            if (!this->iFile->is_open())
                throw;
            this->iFile->get(ch);
            while (!this->iFile->eof()) {
                this->buf.put(ch);
                this->iFile->get(ch);
            }
            this->iFile->close();
            delete this->iFile;

            ch = 0;

            StreamIterator iterator(&(this->buf));
            while (iterator.hasLine())
                this->printLine(this->format(iterator.nextLine()));

            this->buf.clear();
            this->fScope = false;
        }
        this->hasIfile = false;
    }

    if (this->hasOfile && this->hasIfile) {
        for (size_t n = 0; n < out_v->size(); n++) {
            this->iFile = new ifstream(inp_v->at(n), ios::in);

            if (!this->iFile->is_open())
                throw;

            this->iFile->get(ch);
            while (!this->iFile->eof()) {
                this->buf.put(ch);
                this->iFile->get(ch);
            }
            this->iFile->close();
            delete this->iFile;

            ch = 0;

            this->oFile = new ofstream(out_v->at(n), ios::out | ios::trunc);

            if (!this->oFile->is_open())
                throw;

            StreamIterator iterator(&(this->buf));
            while (iterator.hasLine())
                this->writeLine(this->format(iterator.nextLine()));
            this->oFile->close();
            delete this->oFile;

            this->buf.clear();
            this->fScope = false;
        }
        this->hasOfile = false;
    }

    if (this->hasRfile) {
        for (size_t n = 0; n < rep_v->size(); n++) {
            this->iFile = new ifstream(rep_v->at(n), ios::in);

            if (!this->iFile->is_open())
                throw;

            this->iFile->get(ch);
            while (!this->iFile->eof()) {
                this->buf.put(ch);
                this->iFile->get(ch);
            }
            this->iFile->close();
            delete this->iFile;

            ch = 0;

            if (bflag) {
                string *backup = new string(rep_v->at(n).c_str());
                backup->append(".orig");
                rename(rep_v->at(n).c_str(), backup->c_str());
                delete backup;
            }

            this->oFile = new ofstream(rep_v->at(n), ios::out | ios::trunc);

            if (!this->oFile->is_open())
                throw;

            StreamIterator iterator(&(this->buf));
            while (iterator.hasLine())
                this->writeLine(this->format(iterator.nextLine()));
            this->oFile->close();
            delete this->oFile;

            this->buf.clear();
        }
        this->hasRfile = false;
    }
    return EXIT_SUCCESS;
}

void Formatter::writeLine(string *out)
{
    if (out->length())
        this->oFile->write(out->c_str(), out->length());
    else
        this->oFile->write("\n", 1);
    this->oFile->flush();
    delete out;
}

void Formatter::printLine(string *out)
{
    if (out->length())
        cout << out->c_str();
    else
        cout << endl;
    cout.flush();
    delete out;
}

string *Formatter::format(string *in)
{
    string s;
    string *out = new string;

    out->assign(in->c_str());
    delete in;
    // TAB remove
    replace(out, "^\t*", "");
    // SPACE around Colons
    replace(out, "\\s*(:)\\s*", " $1 ");
    // Remove trailing Whitespaces and align Commas
    replace(out, "\\s*(,)\\s+?$", "$1\n");
    // Remove prepended and trailing Whitespaces inside square-brackets
    replace(out, "(\\[)\\s*", "$1");
    replace(out, "\\s*(\\],?)\\s*", "$1");
    // SPACE around Variable Assignment
    replace(out, "^\\s*(\\w+)\\s*(=)\\s*(\\w+)\\s*(\\()\\s*", "$1 $2 $3$4");
    // 2 Args on the same line as Function
    replace(out, "\\s*(\\()\\s*('?.+?'?)\\s*(,)\\s*('?.+?'?)\\s*(,|\\))\\s*$", "$1$2$3 $4$5\n");

    if (this->fScope) {
        align_t sc = 0;
        do
            if (out->at(sc++) != ' ')
                break;
        while (true);

        if (sc < this->indent)
            out->insert(0, this->indent - (sc - 1), ' ');

        if (sc > this->indent) {
            out->erase(0, sc - 1);
            out->insert(0, this->indent, ' ');
        }

        if (out->npos != out->find(')'))
            this->fScope = false;
    }

    if (out->npos != out->find('(')) {
        this->indent = out->find('(') + 1UL;
        this->fScope = true;
    }

    return out;
}

void Formatter::parseCmdLine(int argc, char **argv)
{
    int ch;

    inp_v = new vector<string>;
    out_v = new vector<string>;
    rep_v = new vector<string>;

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
                inp_v->push_back(string(optarg));
            else
                throw;
            break;

        case 'o':
            out_v->push_back(string(optarg));
            break;

        case 'r':
            if (0 == access(optarg, F_OK))
                rep_v->push_back(string(optarg));
            else
                throw;
            break;

        default:
            delete out_v;
            delete inp_v;
            delete rep_v;
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

    if (0 < rep_v->size())
        this->hasRfile = true;

    if (0 < inp_v->size())
        this->hasIfile = true;

    if (0 < out_v->size())
        this->hasOfile = true;
}

static void replace(string *target, const char *rexp, const char *rfmt)
{
    regex expr(rexp);

    target->assign(regex_replace(target->c_str(), expr, rfmt));
}
}
