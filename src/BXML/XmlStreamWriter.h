#ifndef _XmlStreamWriter_H
#define _XmlStreamWriter_H
#include <string>
#include <list>
#include <iostream>
#include <exception>

/*!
 * Exceptions that may be thrown by the XmlStreamWriter class.
 */
class XmlException : public std::exception
{
public:
    XmlException(const char *desc);
    XmlException(const std::string& str);
    ~XmlException() throw();

    virtual const char *what() const throw();

private:
    std::string description;
};

/*!
 * Helper class for writing xml to an arbitrary stream. This class can be used
 * to write arbitrary Xml, and is not tied to the B xml format.
 */
class XmlStreamWriter
{
public:
    XmlStreamWriter(std::ostream *output_stream = 0);
    ~XmlStreamWriter();

    void beginDocument();
    void endDocument();

    void beginElement(const std::string& element_name);
    void writeAttribute(const std::string& name, const std::string& value);
    void writeAttribute(const std::string& name, unsigned int value);
    void writeAttribute(const std::string& name, int value);

    void writeText(const std::string& text);
    void writeXml(const std::string& xml);

    /*!
     * Closes the last element. If closed_element is specified, then this method
     * is checking that the currently closed element is equal to closed_element,
     * and throws an exception if there is a mismatch.
     */
    void endElement(const std::string& expected_element = "");

    void setIndentString(const std::string& indent_string);

    void newLine();

    std::ostream* getOutputStream();
private:
    /*!
     * Structure holding information on elements.
     */
    struct ElementInfo
    {
        ElementInfo() : closedWithNL(false) { }
        ElementInfo(const std::string element_name) : name(element_name), closedWithNL(false) { }

        std::string name;
        bool        closedWithNL;
    };

    void writeEscaped(const std::string& text);
    void writeIndent();

    /*!
     * Closes the current element, if needed.
     */
    void closeElement(bool cr_after);

    std::ostream *outputStream;
    std::list<XmlStreamWriter::ElementInfo> elementStack;

    /*!
     * Indicates whether the current element is closed.
     */
    bool        elementClosed;
    int         indentLevel;
    std::string indentString;
};

#endif /* _XmlStreamWriter_H */
