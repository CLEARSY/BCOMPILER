#include "XmlStreamWriter.h"
#include <sstream>

XmlException::XmlException(const char *desc)
    : description(desc)
{

}

XmlException::XmlException(const std::string& desc)
    : description(desc)
{

}

XmlException::~XmlException() throw ()
{

}

const char *XmlException::what() const throw()
{
    return description.c_str();
}

XmlStreamWriter::XmlStreamWriter(std::ostream *output_stream)
    : outputStream(output_stream),
      elementClosed(true),
      indentLevel(0),
      indentString("  ")
{
    if(!outputStream)
    {
        outputStream = &std::cout;
    }
}

XmlStreamWriter::~XmlStreamWriter()
{ }

void XmlStreamWriter::setIndentString(const std::string &indent_string)
{
    indentString = indent_string;
}

void XmlStreamWriter::writeIndent()
{
    for(int i=0; i<indentLevel; ++i)
    {
        (*outputStream) << indentString;
    }
}

void XmlStreamWriter::newLine()
{
    (*outputStream) << std::endl;
    writeIndent();
}

void XmlStreamWriter::beginElement(const std::string &element_name)
{
    closeElement(true);
    if(!elementStack.empty())
    {
        newLine();
    }
    elementStack.push_back(element_name);
    elementClosed = false;
    (*outputStream) << "<" << element_name;
}

void XmlStreamWriter::closeElement(bool cr_after)
{
    if(!elementStack.empty())
    {
        if(!elementClosed)
        {
            (*outputStream) << ">";
            elementClosed = true;
            ++indentLevel;
            if(cr_after)
            {
                elementStack.back().closedWithNL = true;
            }
        }
    }
}

void XmlStreamWriter::writeText(const std::string &text)
{
    closeElement(false);
    if(!elementStack.empty())
        elementStack.back().closedWithNL = false;
    writeEscaped(text);
}

void XmlStreamWriter::writeXml(const std::string &xml)
{
    closeElement(false);
    newLine();
    *outputStream << xml;
    newLine();
}

void XmlStreamWriter::writeEscaped(const std::string &text)
{
    for(std::string::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        char c = *it;
        switch(c)
        {
        case '&':
            (*outputStream) << "&amp;";
            break;
        case '<':
            (*outputStream) << "&lt;";
            break;
        case '>':
            (*outputStream) << "&gt;";
            break;
        case '\'':
            (*outputStream) << "&apos;";
            break;
        case '"':
            (*outputStream) << "&quot;";
            break;
        default:
            (*outputStream) << c;
            break;
        }
    }
}

void XmlStreamWriter::writeAttribute(const std::string &name, int value)
{
    std::stringstream stream;
    stream << value;
    writeAttribute(name, stream.str());
}

void XmlStreamWriter::writeAttribute(const std::string &name, unsigned int value)
{
    std::stringstream stream;
    stream << value;
    writeAttribute(name, stream.str());
}

void XmlStreamWriter::writeAttribute(const std::string &name, const std::string &value)
{
    if(!elementClosed)
    {
        (*outputStream) << " " << name << "='";
        writeEscaped(value);
        (*outputStream) << "'";
    }
    else
    {
        throw XmlException("XmlStreamWriter::writeAttribute called on closed element");
    }
}

void XmlStreamWriter::endElement(const std::string& expected_element)
{
    if(!elementStack.empty())
    {
        ElementInfo closed_element = elementStack.back();
        if(!expected_element.empty() && closed_element.name != expected_element)
        {
            std::stringstream stream;
            stream << "XmlStreamWriter::endElement: inconsistency detected: expecting "
                      << expected_element
                      << ", got " << closed_element.name << " instead";
            std::string message = stream.str();

            throw XmlException(message.c_str());
        }

        elementStack.pop_back();

        if(elementClosed)
        {
            --indentLevel;
            if(closed_element.closedWithNL)
            {
                newLine();
            }
            (*outputStream) << "</" << closed_element.name << ">";
        }
        else
        {
            (*outputStream) << "/>";
        }
        elementClosed = true;
    }
    else
    {
        throw XmlException("XmlStreamWriter::endElement called without opened element");
    }
}

void XmlStreamWriter::beginDocument()
{
    (*outputStream) << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
}

void XmlStreamWriter::endDocument()
{

}

std::ostream* XmlStreamWriter::getOutputStream() {
    return outputStream;
}
