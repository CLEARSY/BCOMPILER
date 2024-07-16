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
    std::ostream &os {(c == CRITICITY_ERROR || c == CRITICITY_WARNING) ? std::cerr : std::cout};

    switch (c) {
    case CRITICITY_ERROR:
    case CRITICITY_WARNING:
      os << getCriticityString(c) << ": ";
      break;
    case CRITICITY_INFO:
    case CRITICITY_MESSAGE:
      // no special tag for info and message
      break;
    }

    if(!file.empty())
    {
        os << file << ":";
        if(line != UNKNOWN_POS)
        {
            os << line;
            if(column != UNKNOWN_POS)
            {
                os << "," << column;
            }
            os << ":";
        }
    }

    os << desc << std::endl;
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
