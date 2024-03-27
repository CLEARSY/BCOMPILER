#include <iostream>

#include "MessageHandler.h"

MessageHandler::~MessageHandler()
{
}

void MessageHandler::message(Criticity c,
                             const std::string& desc,
                             const std::string& file,
                             int line,
                             int column)
{
    switch(c)
    {
        case CRITICITY_ERROR:
        case CRITICITY_WARNING:
            std::cout << getCriticityString(c) << ": ";
            break;
        case CRITICITY_INFO:
        case CRITICITY_MESSAGE:
            // no special tag for info and message
            break;
    }

    if(!file.empty())
    {
        std::cout << file << ":";
        if(line != UNKNOWN_POS)
        {
            std::cout << line;
            if(column != UNKNOWN_POS)
            {
                std::cout << "," << column;
            }
            std::cout << ":";
        }
    }

    std::cout << desc << std::endl;
}

std::string MessageHandler::getCriticityString(Criticity c)
{
    switch(c)
    {
        case CRITICITY_ERROR:
        default:
            return "error";
        case CRITICITY_WARNING:
            return "warning";
        case CRITICITY_INFO:
            return "info";
        case CRITICITY_MESSAGE:
            return "message";
    }
}
