#include "bxml_api.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XmlWriter.h"
#include <BCOMPLoader.h>
#include <MessageHandler.h>
#include "c_api.h"


std::ostream *get_output_file2(T_betree *btree, const std::string& output_directory)
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

EXTERN int  m_component_bxml(char* mach_as, char *db_file, const char *converter_name,
                              char* output_directory) {
    bool pos_attributes = false;
    bool semantic_analysis = true;
    bool b0_check = false;

    int indent_size = 0;

    MessageHandler messages_handler;

    BCOMPLoader loader(&messages_handler);

    loader.loadDbFile(db_file);

    std::string s_converter_name = converter_name?converter_name:"";

    T_betree * tree = loader.loadComponent(mach_as, s_converter_name, semantic_analysis, b0_check);

    if (tree == NULL) {
        return 1;
    }

    std::unique_ptr<std::ostream> output_stream;

    output_stream.reset(get_output_file2(tree, output_directory));

    XmlWriter writer(output_stream.get() ? output_stream.get() : &std::cout);

    writer.setIndentSize(indent_size);
    writer.setWritePositionAttributes(pos_attributes);

    jmp_buf bcomp_handler;

    set_exit_handler(&bcomp_handler);

    writer.visit(tree);

    return 0;

}

//EXTERN int m_component_bxml(T_component *comp, const char* converterName,
//                                                          char* output_directory) {
//    T_machine *sem_mach = NULL;
//    int indent_size = 0;
//    bool pos_attributes = false;

//    sem_mach = comp->get_semantic_machine(converterName);
//    if (sem_mach == NULL)
//      {
//        TRACE0("set_component_checksums -> KO");
//        return 1;
//      }
//    T_betree * tree = sem_mach->get_betree();

//    std::unique_ptr<std::ostream> output_stream;

//    output_stream.reset(get_output_file2(tree, output_directory));

//    XmlWriter writer(output_stream.get() ? output_stream.get() : &std::cout);

//    writer.setIndentSize(indent_size);
//    writer.setWritePositionAttributes(pos_attributes);

//    jmp_buf bcomp_handler;

//    set_exit_handler(&bcomp_handler);

//    writer.visit(tree);

//    // On libere les betree crees par get_semantic_machine
//    get_project_manager()->free_betrees();
//    return 0;
//}
