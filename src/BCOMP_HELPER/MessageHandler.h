#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include <string>

/**
 * Class that is used to display message to the user. The default implementation is
 * to display the messages as text on the standard output.
 */
class MessageHandler
{
public:
    typedef enum Criticity {
        CRITICITY_ERROR,
        CRITICITY_WARNING,
        CRITICITY_INFO,
        CRITICITY_MESSAGE
    } Criticity;

    static const int UNKNOWN_POS = -1;


    virtual ~MessageHandler();

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
    virtual void message(Criticity c,
                         const std::string& desc,
                         const std::string& file = std::string(""),
                         int line = UNKNOWN_POS,
                         int column = UNKNOWN_POS);


    /**
     * Return a string description of the given criticity
     * 
     * @param c the criticity to convert to a string
     * @return a string corresponding to the given criticity
     */
    static std::string getCriticityString(Criticity c);

};

#endif // MESSAGE_HANDLER_H
