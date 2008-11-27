/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             xml.h                                                                   */
/*                                                                                     */
/*  version                                                                            */
/*             1.0                                                                     */
/*                                                                                     */
/*  description                                                                        */
/*                                                                                     */
/*                                                                                     */
/*  component                                                                          */
/*                                                                                     */
/*  author                                                                             */
/*             bjwf       bjwf2000@gmail.com                                           */
/*                                                                                     */
/*  histroy                                                                            */
/*             2008-11-22     initial draft                                            */
/***************************************************************************************/

#ifndef XML_H_
#define XML_H_

#include <libxml/parser.h>
#include <string>

/*
 * a simple wrapper of xmlNodePtr
 */

class CXmlNode
{
protected:
    xmlNodePtr _node;
public:
    operator xmlNodePtr()
    {
        return _node;
    }
    bool isEmpty() const
    {
        return (_node == NULL);
    }
    //  operator 
public:

    CXmlNode() :
        _node(NULL)
    {
    }
    CXmlNode(xmlNodePtr nd) :
        _node(nd)
    {
    }
    ~CXmlNode()
    {
    }

    bool create(const char * name);
    bool create();
    void free();

    CXmlNode attach(xmlNodePtr nd);
    void detach();

    bool setContent(const char * content);
    std::string & getContent(std::string & v);

    int getIntContent();

    bool setName(const char * name);

    bool addProp(const char * propName, const char * value);
    std::string & getProp(const char * propName, std::string &v);

    bool addChild(CXmlNode child);

    CXmlNode findChild(const char * name) const;
    std::string & toString(std::string & strXml);

public:
    CXmlNode child() const
    {
        return _node->children;
    }
    CXmlNode next() const
    {
        return _node->next;
    }

    const char * name() const
    {
        return (const char *)(_node->name);
    }
    xmlElementType type() const
    {
        return  (_node->type);
    }
   
};
/*
 * a simple wrapper of xmlDocPtr
 */
class CXmlDoc
{
protected:
    xmlDocPtr _doc;
public:

    CXmlDoc(xmlDocPtr d) :
        _doc(d)
    {
    }
    CXmlDoc() :
        _doc(NULL)
    {

    }
    ~CXmlDoc()
    {
    }

public:
    operator xmlDocPtr()
    {
        return _doc;
    }

public:
    bool isEmpty() const
    {
        return (_doc == NULL);
    }
    void attach(xmlDocPtr doc)
    {
        _doc = doc;
    }
    void detach()
    {
        _doc = NULL;
    }
    bool createFromFile(const char * fname);
    bool create();
    bool createFromXmlCdata(const char * xmlstring, int len);
    void free();

    CXmlNode getRoot();
    std::string & toString(std::string & strXml);
    
    void dump(char * & outbuf, int & len)
    {
        xmlChar *xmlbuff;
        xmlDocDumpFormatMemoryEnc(_doc, &xmlbuff, &len, "UTF-8", 1);
        outbuf = (char *)xmlbuff;
    }
    void setRoot(CXmlNode nd)
    {
        xmlDocSetRootElement(_doc, nd);
    }
};
#endif /*XML_H_*/
