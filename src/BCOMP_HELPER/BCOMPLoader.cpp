#include <iostream>
#include <sstream>
#include <fstream>
#include <c_port.h>
#include <c_api.h>
#include <p_api.h>
#include "BCOMPLoader.h"
#include "MessageHandler.h"

BCOMPLoader::BCOMPLoader(MessageHandler *h)
    : handler(h), hasDb(false), deleteHandler(false), eventB(false)
{
    if(!handler)
    {
        handler = new MessageHandler;
        deleteHandler = true;
    }
}

BCOMPLoader::~BCOMPLoader()
{
    if(deleteHandler)
    {
        delete handler;
    }
}

void BCOMPLoader::loadResourceFile(const std::string& file)
{
    init_resource_manager();
    load_resources(file.c_str(), NULL, NULL);
}

void BCOMPLoader::setSearchPath(const std::vector<std::string>& path)
{
    std::vector<std::string>::const_iterator i = path.begin();
    for(;i != path.end(); ++i)
    {
        add_source_path(i->c_str());
    }
}


std::string BCOMPLoader::readUntil(std::ifstream &input, const char *expected) const
{
    std::stringstream result;

    do
    {
        int read = input.get();
        if(input.eof())
        {
            return result.str();
        }

        const char *current = expected;
        while(*current != '\0')
        {
            if(*current == read)
            {
                return result.str();
            }
            ++current;
        }
        // append char that has been read to the result
        result << (char)read;
    } while(1);
}

void BCOMPLoader::readAllStrings(const std::string&file_name,
                                   const char *separators,
                                   std::vector<std::string>& result)
{
    // read libraries
    std::ifstream lib_file;
    // read all bdp path from .lib file
    lib_file.open(file_name.c_str(), std::ios::in|std::ios::binary);
    if(lib_file.is_open())
    {
        std::string current;
        do
        {
            current = readUntil(lib_file, separators);
            if(!current.empty())
            {
                if(current[0] == '"')
                {
                    current = current.substr(1, current.length()-2);
                }
                result.push_back(current);
            }
        } while(!current.empty());
        lib_file.close();
    }
}

void BCOMPLoader::loadLibrary(const std::string& bdp_path, const std::string& name)
{
    std::string full_name = bdp_path
                            + "/" + name + ".db";
    get_project_manager()->load_library(full_name.c_str());
    loadLibraries(bdp_path, name);
}

void BCOMPLoader::loadLibraries(const std::string& bdp_path, const std::string& name)
{
    std::vector<std::string> bdp_path_list;
    std::vector<std::string> project_names;

    std::string dotlib_path = bdp_path + "/.lib";
    readAllStrings(dotlib_path, ";\n\r", bdp_path_list);

    std::string name_lib_path = bdp_path + "/" + name + ".lib";
    readAllStrings(name_lib_path, ":\n\r", project_names);

    if(!bdp_path_list.empty())
    {
        size_t count = bdp_path_list.size();
        if(count == project_names.size())
        {
            for(size_t i=0; i<count; ++i)
            {
                std::string current_bdp = bdp_path_list[i];
                std::string current_name = project_names[i];
                loadLibrary(current_bdp, current_name);
            }
        }
    }
}

void BCOMPLoader::loadDbFile(const std::string& file)
{
    init_project_manager() ;
    std::string bdp_dir;
    std::string name;
    int idx = file.find_last_of("/\\");
    if(idx != string::npos){
	bdp_dir = file.substr(0, idx);
	// TODO: check that the file ends with .db
	name = file.substr(idx+1, file.size()-idx-1-3);
    } else {
	bdp_dir = ".";
	name = file.substr(0, file.size()-idx-1-3);
    }
    get_project_manager()->load_current_project(bdp_dir.c_str(),
                                                name.c_str());
    loadLibraries(bdp_dir, name);

    hasDb = true;

    eventB = get_project_manager()->get_current()->isEventB();
}

bool BCOMPLoader::isEventB()
{
    return eventB;
}

bool BCOMPLoader::handleBCOMPErrors()
{
    bool result = true;

    T_msg_line_manager *manager = get_msg_line_manager();
    T_msg_line *messages = manager->get_messages();


    while(messages != NULL)
    {
        int line = messages->get_file_line();
        int col = messages->get_file_column();
        T_symbol *msg_symbol = messages->get_contents();
        std::string msg(msg_symbol->get_value());


        std::string file_name;
        T_symbol *file_name_symbol = messages->get_file_name();
        if(file_name_symbol != NULL)
        {
            file_name = file_name_symbol->get_value();
//            file_name = getFileName(file_name, true);
        }

        T_msg_stream stream = messages->get_stream();

        if(stream == MSG_ERROR_STREAM)
        {
            handler->message(MessageHandler::CRITICITY_ERROR, msg, file_name, line, col);
            result = false;
        }
        else
        {
            handler->message(MessageHandler::CRITICITY_WARNING, msg, file_name, line, col);
        }

        messages = (T_msg_line*)messages->get_next();
    }

    return result;
}



T_betree* BCOMPLoader::loadComponent(const std::string& file_name, const std::string& converterName, bool semantic, bool b0)
{
    jmp_buf bcomp_exit_handler;
    jmp_buf *previous_handler = get_exit_handler();
//    fileName = file_name;
    T_betree *betree = 0;

    if (hasDb) { set_file_fetch_mode(FFMODE_ORIGINAL_MS); }

    if(!setjmp(bcomp_exit_handler))
    {
        // If no previous handler was set, we install our own so that
        // the bcomp does not call exit() in case of error
        if(!previous_handler)
        {
            set_exit_handler(&bcomp_exit_handler);
        }

        // bcomp initialisation
        set_msg_destination(MSG_DEST_BUFFERS);
        reset_msg_line_manager();
        reset_error_count();
        reset_warning_count();

//        std::string f = getFileName(file_name, false);
        std::string f = file_name;

        betree = compiler_syntax_analysis(f.c_str(), converterName.c_str());

        if(betree == NULL)
        {
            handleBCOMPErrors();
            handler->message(MessageHandler::CRITICITY_ERROR, "Unable to load ", file_name);
            return 0;
        }

        while (strcmp(betree->get_file_name()->get_value(),f.c_str()) != 0)
            betree = betree->get_next_betree();
        // read the machine name
        T_machine *mch = betree->get_root();
        if(mch->get_node_type() != NODE_MACHINE)
        {
            handleBCOMPErrors();
            handler->message(MessageHandler::CRITICITY_ERROR, "Not a machine", file_name);
            return 0;
        }

        if(semantic && betree->get_status() != BST_SEMANTIC)
        {
            compiler_semantic_analysis(betree, converterName.c_str());
        }

        if(b0)
        {
            compiler_B0_check(betree);
        }
    }

    set_exit_handler(previous_handler);

    if(handleBCOMPErrors())
    {
        return betree;
    }
    else
    {
        return 0;
    }
}

void BCOMPLoader::unloadComponent(T_betree *betree)
{
    T_betree_manager *manager = get_betree_manager();
    manager->delete_betree(betree);
    delete betree;
}

