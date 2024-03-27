#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XmlWriter.h"
#include <BCOMPLoader.h>
#include <m_projec.h>
#include <MessageHandler.h>
#include <ctime>
#include <sys/stat.h>

#include "version.h"

#ifdef WIN32
int getopt(int nargc,char * const nargv[], const char *ostr);
extern char *optarg;
extern int   optind;
#else // WIN32
#include <unistd.h>
#endif // WIN32

void display_help()
{
  std::cout <<
    R"(
Based on the ICU4C library, copyright (c) 2016 and later Unicode, Inc. and others.
A copy of the license should have been distributed with this program.
See also <https://icu.unicode.org>.

Options:
 -h               : displays this help message and exits.
 -v               : displays version (and sha) and exits.
 -p project.db    : loads project information from the given .db file.
 -r resource_file : loads the given resource file.
 -I directory     : adds the given directoy to the list of seached directories.
 -E encoding      : uses the specified encoding.
 -a               : performs semantic analysis (adds B type information).
 -c               : performs B0 check (adds B0 type information). This option also activates semantic analysis.
 -P               : add position attributes.
 -x               : display messages in xml format
 -i count         : sets the indentation size (default to zero).
 -O directory     : sets the output directory. If nothing is specified, the file is written to stdout.
 -M mode          : sets the default project mode (SOFTWARE|SYSTEM|VALIDATION).
 -d               : do not create constraint forcing a set machine parameter to be a FIN(INTEGER).
)";
}

void display_version()
{
        printf("%s (%s)\n", BXML_VERSION, BXML_SOURCE_CODE_SHA);
        printf("%s\n", BXML_COPYRIGHT);
}

std::ostream *get_output_file(T_betree *btree, const std::string& output_directory)
{
    std::ostream *result = 0;

    T_machine *mch = btree->get_root();
    if(mch)
    {
        T_ident *mch_name = mch->get_machine_name();
        if(mch_name)
        {
            T_symbol *symb = mch_name->get_name();

            if(symb)
            {
                std::string path = output_directory;

                switch(path[path.size()-1])
                {
                case '/':
                case '\\':
                    break;
                default:
                    path += "/";
                    break;
                }

                path = path + symb->get_value() + ".bxml";

                result = new std::fstream(path.c_str(), std::fstream::out|std::fstream::trunc);
            }
        }
    }

    return result;
}


class BxmlMessageHandler : public MessageHandler {
public:
    ~BxmlMessageHandler();

    /**
        * Display a message with the given criticity. The parameters regarding the file
        * and position are optionals
        *
        * @param c the criticity of the message
        * @param desc the description of the message
        * @param file the file for which the error or information is relevant
        * @param line the line number, if any
        * @param column the column number, if any
        */
    void message(Criticity c,
                 const std::string& desc,
                 const std::string& file = std::string(""),
                 int line = UNKNOWN_POS,
                 int column = UNKNOWN_POS);
};



int main(int argc, char **argv)
{
    char c;
    char *db_file = 0;
    char *resource_file = 0;
    std::vector<std::string> include_directories;
    std::string converter_name = "UTF-8";
    bool semantic_analysis = false;
    bool b0_check = false;
    bool xml = false;

    bool pos_attributes = false;
    bool delete_set_constraint = false;

    int  return_code = 0;
    int indent_size = 0;
    std::string output_directory;

    include_directories.push_back(".");

    if(argc <= 1) {
        display_help();
        exit(0);
    }

    while((c = getopt(argc, argv, "xhp:I:r:E:acdPi:O:M:v")) != EOF)
    {
        switch(c)
        {
        case 'p':
            db_file = optarg;
            break;
        case 'I':
            include_directories.push_back(optarg);
            break;
        case 'r':
            resource_file = optarg;
            break;
        case 'E':
            converter_name = optarg;
            break;
        case 'a':
            semantic_analysis = true;
            break;
        case 'c':
            b0_check = true;
            semantic_analysis = true;
            break;
        case 'P':
            pos_attributes = true;
            break;
        case 'i':
            indent_size = atoi(optarg);
            break;
        case 'O':
            {
                struct stat info;
                if( stat( optarg, &info ) == 0 && S_ISDIR(info.st_mode)){
                    output_directory = optarg;
                } else {
                    std::cout << "Unknown directory '" << optarg << "'." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            }
        case 'x':
            xml = true;
            break;
        case 'M':
	    {
		std::string mode = optarg;
		if(mode == "SOFTWARE")
		    T_project::set_default_project_type(PROJECT_SOFTWARE);
		else if(mode == "SYSTEM")
		    T_project::set_default_project_type(PROJECT_SYSTEM);
		else if(mode == "VALIDATION")
		    T_project::set_default_project_type(PROJECT_VALIDATION);
		else {
		    std::cout << "Error: unknwown project mode '" << mode << "'." << std::endl;
		    display_help();
		    exit(1);
		}
		break;
	    }
        case 'd':
            delete_set_constraint = true;
            break;
	case 'v':
	  display_version();
	  // falls through on purpose
        case 'h':
            display_help();
            exit(0);
        default:
          std::cout << "Error: unknwown parameter " << c << std::endl;
          display_help();
          exit(1);
        }
    }

    MessageHandler* messages_handler;

    if (xml)
        messages_handler = new BxmlMessageHandler();
    else
        messages_handler = new MessageHandler();

    BCOMPLoader loader(messages_handler);

    if(db_file)
    {
	loader.loadDbFile(db_file);
    }
    else
    {
        init_project_manager();
        loader.setSearchPath(include_directories);
    }

    if(resource_file)
    {
        loader.loadResourceFile(resource_file);
    }

    if(optind<argc)
    {
        for(int i=optind; i<argc; ++i)
        {
            try
            {
                T_betree * tree = loader.loadComponent(argv[i], converter_name, semantic_analysis, b0_check);

                if(tree)
                {
                    std::unique_ptr<std::ostream> output_stream;
                    if(!output_directory.empty())
                    {
                        output_stream.reset(get_output_file(tree, output_directory));
                    }
                    XmlWriter writer(output_stream.get() ? output_stream.get() : &std::cout);
                    writer.setIsEventB(loader.isEventB());
                    writer.setAddConstraintOnParamSet(not delete_set_constraint);

                    writer.setIndentSize(indent_size);
                    writer.setWritePositionAttributes(pos_attributes);
                    writer.setSemanticAnalysis(semantic_analysis);
                    writer.setB0Check(b0_check);

                    jmp_buf bcomp_handler;
                    if(!setjmp(bcomp_handler))
                    {
                        set_exit_handler(&bcomp_handler);

                        writer.visit(tree);

                        loader.unloadComponent(tree);
                    }
                    else
                    {
                        // Indicates that a breakpoint should be set in the method
                        // stop() located in c_out.cpp to analyse the error
                        return_code = 3;
                        if(!semantic_analysis)
                        {
                            std::cout << "An unrecoverable error has been encountered while writing the Xml" << std::endl;
                            std::cout << "This can be caused by a type error." << std::endl;
                            std::cout << "You should try running the program with the syntax analysis option" << std::endl;
                        }
                        else
                        {
                            std::cout << "Unrecoverable error encountered while generating xml" << std::endl;
                        }
                    }
                    set_exit_handler(NULL);
                }
                else
                {
                    return_code = 1;
                }
            }
            catch(std::exception& e)
            {
                std::cout << "Exception caught: " << e.what() << std::endl;
                return_code = 2;
            }
        }
    }
    else
    {
        std::cout << "No file provided." << std::endl;
    }

    exit(return_code);
}

BxmlMessageHandler::~BxmlMessageHandler()
{
}

void BxmlMessageHandler::message(Criticity c,
                             const std::string& desc,
                             const std::string& file,
                             int line,
                             int column)
{
    std::cout << "<bcompMessage>";

//    switch(c)
//    {
//        case CRITICITY_ERROR:
//        case CRITICITY_WARNING:
//            std::cout << getCriticityString(c) << ": ";
//            break;
//        case CRITICITY_INFO:
//        case CRITICITY_MESSAGE:
//            // no special tag for info and message
//            break;
//    }

    if(!file.empty())
    {
        std::cout <<  file + ":";
        if(line != UNKNOWN_POS)
        {
            std::cout <<  line;
            if(column != UNKNOWN_POS)
            {
                std::cout <<  ":" << column;
            }
            std::cout <<  " ";
        }
    }

    std::cout <<  desc;

    std::cout << "</bcompMessage>\n";
}
