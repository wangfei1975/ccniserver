/***************************************************************************************/
/*                                                                                     */
/*  Copyright(c)   .,Ltd                                                               */
/*                                                                                     */
/***************************************************************************************/
/*  file name                                                                          */
/*             ccni_xml.h                                                              */
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
/*             2008-11-21     initial draft                                            */
/***************************************************************************************/
#ifndef CCNI_XML_H_
#define CCNI_XML_H_

#include <libxml/parser.h>
#include <string>

class CCNIXmlMsg;

class CCNIXmlDoc
{
protected:
    xmlDocPtr _doc;
public:
    CCNIXmlDoc(xmlDocPtr d) :
        _doc(d)
    {
    }
    CCNIXmlDoc();
    ~CCNIXmlDoc()
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
    void attach(xmlDocPtr doc);
    void detach();

    bool create();
    bool createFromXmlCdata(const char * xmlstring, int len);
    void free();

    bool appendMsg(CCNIXmlMsg msg);

    int getMsgCount();
    CCNIXmlMsg getFirstMsg();
    CCNIXmlMsg getNextMsg(CCNIXmlMsg cur);

    CCNIXmlMsg getRoot();

    std::string & toString(std::string & strXml);

    std::string & toUtf8String(std::string & strXml);

};

class CCNIXmlNode
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

    CCNIXmlNode();
    CCNIXmlNode(xmlNodePtr nd);
    ~CCNIXmlNode();

    bool create(const char * name);
    bool create();
    void free();

    void attach(xmlNodePtr nd);
    void detach();

    bool setContent(const char * content);
    std::string & getContent(std::string & v);
    bool setName(const char * name);
    bool addProp(const char * propName, const char * value);
    const char * getProp(const char * propName);

};

class CCNIXmlNodeTree : public CCNIXmlNode
{
public:

    bool addChild(CCNIXmlNode child);

    bool addChild(CCNIXmlNodeTree child);

    CCNIXmlNodeTree findChildElement(const char * name) const;

    std::string & toString(std::string & strXml);

};

class CCNIXmlMsg : public CCNIXmlNodeTree
{
public:

    bool setLabel(const char * ccniMsgLabel);
    bool setDescription(const char * des);

    bool addParameter(const char * paramName, const char * value);
    bool addParameter(CCNIXmlNode param);

    bool setParameter(const char * paramName, const char * value);
    bool setParameter(const char * paramName, int v);
    const char * getLabel() const;
    std::string & getTextParameter(const char * paramName, std::string & value) const;
    int getIntParameter(const char * paramName) const;

    bool getParameter(const char * paramName, std::string & v) const;
    bool getParameter(const char * paramName, int & v) const;
    CCNIXmlMsg getComplexParameter(const char * paramName) const;
};

#endif //CCNI_XML_H_
